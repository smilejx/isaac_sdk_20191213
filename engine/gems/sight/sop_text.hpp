/*
Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
#pragma once

#include <string>
#include <utility>

#include "engine/core/image/image.hpp"
#include "engine/core/optional.hpp"
#include "engine/gems/serialization/json.hpp"
#include "engine/gems/serialization/json_formatter.hpp"

namespace isaac {
namespace sight {

// Sight Operation Text
// Can be used to display text at a given position
// TODO: Add more styles and options
struct SopText {
  // Creates a text with not other information (will be rendered at (0, 0))
  SopText(std::string text) {
    json_["t"] = "text";
    json_["text"] = std::move(text);
  }
  SopText(const char* text) {
    json_["text"] = text;
  }

  // Creates a style with a color and wether object should be filled or not.
  template <typename Text, typename K, int N>
  SopText(const Text& text, const Vector<K, N>& pos) : SopText(text) {
    serialization::Set(json_["p"], pos);
  }

 private:
  friend const Json& ToJson(const SopText&);
  friend Json ToJson(SopText&&);

  // Creates empty style
  SopText() = default;

  Json json_;
};

// Returns the json of a SopText
const Json& ToJson(const SopText&);
Json ToJson(SopText&&);

}  // namespace sight
}  // namespace isaac
