"""
Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
"""

cc_library(
    name = "main",
    srcs = glob(
        ["src/*.cc"],
        exclude = ["src/gtest-all.cc"],
    ),
    hdrs = glob([
        "include/**/*.h",
        "src/*.h",
    ]),
    includes = ["include"],
    linkopts =
        select({
            "@com_nvidia_isaac//engine/build:windows": [],
            "@com_nvidia_isaac//engine/build:windows_msvc": [],
            "@com_nvidia_isaac//engine/build:platform_aarch64": [
                "-pthread",
            ],
            "@com_nvidia_isaac//engine/build:platform_x86_64": [
                "-pthread",
            ],
        }),
    visibility = ["//visibility:public"],
)
