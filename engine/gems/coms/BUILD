"""
Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
"""

load("//engine/build:isaac.bzl", "isaac_cc_binary", "isaac_cc_library")

isaac_cc_library(
    name = "socket",
    hdrs = [
        "socket.hpp",
    ],
    visibility = ["//visibility:public"],
)

isaac_cc_library(
    name = "serial_baudrate",
    srcs = [
        "serial_baudrate.cpp",
    ],
    hdrs = [
        "serial_baudrate.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//engine/core",
    ],
)

isaac_cc_library(
    name = "serial",
    srcs = [
        "serial.cpp",
    ],
    hdrs = [
        "serial.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//engine/core",
        "//engine/gems/coms:serial_baudrate",
    ],
)

isaac_cc_library(
    name = "i2c",
    srcs = [
        "i2c.cpp",
    ],
    hdrs = [
        "i2c.hpp",
    ],
    linkopts = select({
        "//engine/build:platform_aarch64": [
        ],
        "//engine/build:platform_x86_64": [
            "-li2c",
        ],
    }),
    visibility = ["//visibility:public"],
    deps = [
        "//engine/core",
    ] + select({
        "//engine/build:platform_aarch64": [
            "@libi2c_aarch64",
        ],
        "//engine/build:platform_x86_64": [
            # no extra deps needed
        ],
    }),
)

isaac_cc_library(
    name = "i2c_stub",
    srcs = [
        "i2c_stub.cpp",
    ],
    hdrs = [
        "i2c_stub.hpp",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "//engine/gems/coms:i2c",
    ],
)
