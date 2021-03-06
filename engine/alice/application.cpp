/*
Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
#include "application.hpp"

#include <signal.h>

#include <clocale>
#include <cstdio>
#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "engine/alice/backend/application_json_loader.hpp"
#include "engine/alice/backend/backend.hpp"
#include "engine/alice/backend/codelet_backend.hpp"
#include "engine/alice/backend/config_backend.hpp"
#include "engine/alice/backend/error_handler.hpp"
#include "engine/alice/backend/message_ledger_backend.hpp"
#include "engine/alice/backend/modules.hpp"
#include "engine/alice/backend/node_backend.hpp"
#include "engine/alice/components/Config.hpp"
#include "engine/alice/components/Pose.hpp"
#include "engine/core/assert.hpp"
#include "engine/gems/serialization/json.hpp"
#include "engine/gems/uuid/uuid.hpp"

namespace isaac {
namespace alice {

namespace {

// Running applications
std::set<Application*> s_applications;

// Splits a name of the form 'nodename/compname/tag'
std::tuple<std::string, std::string, std::string> Split(const std::string& name) {
  const size_t p1 = name.find('/');
  const size_t p2 = name.find('/', p1 + 1);  // This should be safe if p1 is npos.
  ASSERT(p1 != std::string::npos && p2 != std::string::npos,
         "Invalid channel name '%s'. Make sure you are using the pattern node/component/tag to "
         "specifiy a message channel.", name.c_str());
  return std::tuple<std::string, std::string, std::string>{
      name.substr(0, p1), name.substr(p1 + 1, p2 - p1 - 1), name.substr(p2 + 1)};
}

// Writes scheduler job statistics to a JSON file
void WritePerformanceReport(const std::string& name,
                            const std::vector<scheduler::JobStatistics>& stats,
                            const nlohmann::json& graph_json, const std::string& filename) {
  nlohmann::json performance;
  for (const auto& stat : stats) {
    const std::string name = stat.descriptor.name;
    nlohmann::json item;
    item["name"] = name;
    item["mode"] = static_cast<int>(stat.descriptor.execution_mode);
    item["count"] = stat.num_executed;
    item["avg_time_ms"] = stat.getAverageExecutionTime() * 1000.0;
    item["median_time_ms"] = stat.execution_time_median.median() * 1000.0;
    item["10pile_time_ms"] = stat.execution_time_median.percentile(0.1) * 1000.0;
    item["90pile_time_ms"] = stat.execution_time_median.percentile(0.9) * 1000.0;
    item["max_time_ms"] = stat.execution_time_median.max() * 1000.0;
    item["min_time_ms"] = stat.execution_time_median.min() * 1000.0;
    item["overrun_mus"] = stat.execution_delay.value() * 1'000'000.0;
    performance[name] = item;
  }
  nlohmann::json report;
  report["perf"] = performance;
  report["app_graph"] = graph_json;
  report["name"] = name;
  std::ofstream ofs(filename);
  ofs << report.dump(2);
}

}  // namespace

Application::Application(const ApplicationJsonLoader& loader) {
  createApplication(loader);
}

Application::Application(const std::vector<std::string> module_paths,
                         const std::string& asset_path) {
  nlohmann::json app_json;
  app_json["name"] = Uuid::Generate().str();
  ApplicationJsonLoader loader(asset_path);
  loader.loadApp(app_json);
  loader.appendModulePaths(module_paths);
  createApplication(loader);
}

Application::Application(const nlohmann::json& json, const std::vector<std::string> module_paths,
                         const std::string& asset_path) {
  ApplicationJsonLoader loader(asset_path);
  loader.loadApp(json);
  loader.appendModulePaths(module_paths);
  createApplication(loader);
}

Application::~Application() {
  backend_ = nullptr;
}

void Application::loadFromFile(const std::string& json_file) {
  const std::string absolute_filename = getAssetPath(json_file);
  const auto json = serialization::TryLoadJsonFromFile(absolute_filename);
  ASSERT(json, "Could not load JSON from file '%s' (originally: '%s'", absolute_filename.c_str(),
         json_file.c_str());
  load(*json);
}

void Application::loadFromText(const std::string& json_text) {
  const auto json = serialization::ParseJson(json_text);
  ASSERT(json, "Could not parse JSON from text '%s'", json_text.c_str());
  load(*json);
}

void Application::load(const nlohmann::json& json) {
  ApplicationJsonLoader loader;
  loader.loadMore(json);
  createMore(loader);
}

Node* Application::createNode(const std::string& name) {
  return backend_->node_backend()->createNode(name);
}

void Application::destroyNode(const std::string& name) {
  backend_->node_backend()->destroyNode(name);
}

Node* Application::createMessageNode(const std::string& name) {
  return backend_->node_backend()->createMessageNode(name);
}

Node* Application::findNodeByName(const std::string& name) const {
  return backend_->node_backend()->findNodeByName(name);
}

Node* Application::getNodeByName(const std::string& name) const {
  Node* node = findNodeByName(name);
  ASSERT(node != nullptr, "No node with name '%s'", name.c_str());
  return node;
}

Component* Application::findComponentByName(const std::string& link) const {
  LOG_WARNING("The function Application::findComponentByName is deprecated. Please use "
              "`getNodeComponentOrNull` instead. Note that the new method requires a node name "
              "instead of a component name. (argument: '%s')", link.c_str());
  const auto pos = link.find_first_of('/');
  if (pos == std::string::npos) {
    LOG_ERROR("Invalid link: %s", link.c_str());
    return nullptr;
  }
  const Node* node = backend_->node_backend()->findNodeByName(link.substr(0, pos));
  if (node == nullptr) {
    return nullptr;
  }
  return node->findComponentByName(link.substr(pos + 1));
}

std::tuple<Component*, std::string> Application::getComponentAndTag(const std::string& channel) {
  const auto tokens = Split(channel);
  auto* node = getNodeByName(std::get<0>(tokens));
  auto* component = node->findComponentByName(std::get<1>(tokens));
  ASSERT(component, "No component with name '%s' in node '%s'", std::get<1>(tokens).c_str(),
         std::get<0>(tokens).c_str());
  return std::tuple<Component*, std::string>{component, std::get<2>(tokens)};
}

void Application::startWaitStop(double duration) {
  start();
  Sleep(SecondsToNano(duration));
  stop();
}

void Application::startWaitStop() {
  start();
  s_applications.insert(this);
  signal(SIGINT, [](int signum) {
    for (Application* app : s_applications) {
      app->interrupt();
    }
  });
  // Sleep until interruption is requested
  // TODO use a conditional variable or such
  while (is_running_) {
    Sleep(SecondsToNano(0.05));
  }
  stop();
}

void Application::interrupt() {
  is_running_ = false;
}

void Application::start() {
  LOG_INFO("Starting application '%s' (instance UUID: '%s') ...", name().c_str(), uuid().c_str());
  is_running_ = true;
  // Store application json before starting, since codelet could add component
  if (!application_backup_.empty()) {
    app_json_["graph"] = ApplicationJsonLoader::GetGraphJson(*this, false);
    app_json_["config"] = ApplicationJsonLoader::GetConfigJson(*this, false);
    ApplicationJsonLoader::WriteJsonToFile(application_backup_, app_json_);
  }
  backend_->start();
}

void Application::stop() {
  LOG_INFO("Stopping application '%s' (instance UUID: '%s') ...", name().c_str(), uuid().c_str());
  is_running_ = false;

  if (!performance_report_out_.empty()) {
    WritePerformanceReport(name(), backend_->scheduler()->getJobStatistics(),
                           ApplicationJsonLoader::GetGraphJson(*this), performance_report_out_);
  }
  // Store configuration
  if (!config_backup_.empty()) {
    ApplicationJsonLoader::WriteJsonToFile(config_backup_,
                                           ApplicationJsonLoader::GetConfigJson(*this, false));
  }

  backend_->stop();
}

std::vector<Node*> Application::nodes() const {
  return backend_->node_backend()->nodes();
}

void Application::createApplication(const ApplicationJsonLoader& loader) {
  asset_path_ = loader.getAssetPath();

  name_ = loader.name_;
  app_json_["name"] = loader.name_;
  uuid_ = Uuid::Generate();
  backend_ = std::make_unique<Backend>(this, loader.backend_json_);
  app_json_["backend"] = std::move(loader.backend_json_);

  // Overrides to en_US locale if not specified for Capnp JSON parsing
  const std::string locale = loader.locale_.empty() ? "en_US.UTF-8" : loader.locale_;
  std::setlocale(LC_ALL, locale.c_str());

  // Save parameters which we need when the application starts or shuts down
  application_backup_ = loader.application_backup_;
  config_backup_ = loader.config_backup_;
  performance_report_out_ = loader.performance_report_out_;
  if (!loader.minidump_path_.empty()) {
    backend_->error_handler()->setMinidumpDirectory(loader.minidump_path_);
  }

  // Load statically linked components
  backend_->module_manager()->loadStaticallyLinked();

  createMore(loader);
}

void Application::createMore(const ApplicationJsonLoader& loader) {
  // Load modules
  backend_->module_manager()->setModuleWorkingPath(asset_path_);
  backend_->module_manager()->appendModulePaths(loader.module_paths_);
  backend_->module_manager()->loadModules(loader.modules_);
  for (const auto& module : loader.module_names_) {
    app_json_["modules"].push_back(module);
  }
  {
    const auto component_names = backend_->module_manager()->getComponentNames();
    std::string text;
    for (const auto& name : component_names) {
      text += name + ", ";
    }
    LOG_INFO("Loaded %zd components: %s", component_names.size(), text.c_str());
  }

  // Load configuration by loading deepest nested config first
  for (auto it = loader.config_by_level_.rbegin(); it != loader.config_by_level_.rend(); ++it) {
    backend()->config_backend()->set(*it);
  }

  // Create nodes in order in which they where encountered.
  for (const auto& node : loader.nodes_) {
    backend()->node_backend()->createNodeFromJson(node.json, node.prefix);
  }

  // Now we read all the nodes, we can connect the edges.
  for (const auto& edge : loader.edges_) {
    const auto source = getComponentAndTag(edge.source);
    const auto target = getComponentAndTag(edge.target);
    Connect(std::get<0>(source), std::get<1>(source), std::get<0>(target), std::get<1>(target));
  }
}

std::string Application::getAssetPath(const std::string& path) const {
  // Do nothing about absolute path
  if (!path.empty() && path.at(0) == '/') {
    return path;
  }
  return asset_path_ + "/" + path;
}

}  // namespace alice
}  // namespace isaac
