"""
Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
"""

load("//engine/build:isaac.bzl", "isaac_pkg")

py_binary(
    name = "proportional_control_python",
    srcs = [
        "__init__.py",
        "proportional_control_python.py",
    ],
    data = [
        "proportional_control_python.app.json",
        "//apps:py_init",
        "//apps/tutorials:py_init",
        "//messages:state_proto",
        "//messages:differential_base_proto",
        "//packages/navigation:libnavigation_module.so",
        "//packages/segway:libsegway_module.so",
        "//packages/sensors:libjoystick_module.so",
    ],
    deps = ["//engine/pyalice"],
)

isaac_pkg(
    name = "proportional_control_python-pkg",
    data = [
        "//packages/navigation/apps:differential_base_commander_subgraph",
    ],
    srcs = ["proportional_control_python"],
)
