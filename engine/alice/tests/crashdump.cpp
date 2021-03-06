/*
Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
#include "engine/alice/alice.hpp"
#include "engine/alice/tools/parse_command_line.hpp"
#include "gflags/gflags.h"

namespace {
void Crash() {
  volatile int* a = static_cast<int*>(nullptr);
  *a = 1;
}
}  // namespace

int main(int argc, char** argv) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  isaac::alice::Application app(isaac::alice::ParseApplicationCommandLine("crashdump"));
  Crash();
}
