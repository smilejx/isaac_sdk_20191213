"""
Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
"""

# tensorrt is necessary for running inference in the nvstereonet codelet
# Follow the installation steps here
# https://docs.nvidia.com/deeplearning/sdk/tensorrt-install-guide/index.html

cc_library(
    name = "tensorrt_x86_64",
    srcs = [
        "usr/lib/x86_64-linux-gnu/libnvcaffe_parser.so",
        "usr/lib/x86_64-linux-gnu/libnvcaffe_parser.so.6",
        "usr/lib/x86_64-linux-gnu/libnvcaffe_parser.so.6.0.1",
        "usr/lib/x86_64-linux-gnu/libnvinfer.so",
        "usr/lib/x86_64-linux-gnu/libnvinfer.so.6",
        "usr/lib/x86_64-linux-gnu/libnvinfer.so.6.0.1",
        "usr/lib/x86_64-linux-gnu/libnvinfer_plugin.so",
        "usr/lib/x86_64-linux-gnu/libnvinfer_plugin.so.6",
        "usr/lib/x86_64-linux-gnu/libnvinfer_plugin.so.6.0.1",
        "usr/lib/x86_64-linux-gnu/libnvonnxparser.so",
        "usr/lib/x86_64-linux-gnu/libnvonnxparser.so.6",
        "usr/lib/x86_64-linux-gnu/libnvonnxparser.so.6.0.1",
        "usr/lib/x86_64-linux-gnu/libnvonnxparser_runtime.so",
        "usr/lib/x86_64-linux-gnu/libnvonnxparser_runtime.so.6",
        "usr/lib/x86_64-linux-gnu/libnvonnxparser_runtime.so.6.0.1",
        "usr/lib/x86_64-linux-gnu/libnvparsers.so",
        "usr/lib/x86_64-linux-gnu/libnvparsers.so.6",
        "usr/lib/x86_64-linux-gnu/libnvparsers.so.6.0.1",
    ],
    hdrs = [
        "usr/include/x86_64-linux-gnu/NvCaffeParser.h",
        "usr/include/x86_64-linux-gnu/NvInfer.h",
        "usr/include/x86_64-linux-gnu/NvInferPlugin.h",
        "usr/include/x86_64-linux-gnu/NvInferPluginUtils.h",
        "usr/include/x86_64-linux-gnu/NvInferRuntime.h",
        "usr/include/x86_64-linux-gnu/NvInferRuntimeCommon.h",
        "usr/include/x86_64-linux-gnu/NvInferVersion.h",
        "usr/include/x86_64-linux-gnu/NvOnnxConfig.h",
        "usr/include/x86_64-linux-gnu/NvOnnxParser.h",
        "usr/include/x86_64-linux-gnu/NvOnnxParserRuntime.h",
        "usr/include/x86_64-linux-gnu/NvUffParser.h",
    ],
    includes = ["include"],
    strip_include_prefix = "usr/include/x86_64-linux-gnu",
    visibility = ["//visibility:public"],
    deps = [
        "@com_nvidia_isaac//third_party:cudnn",
    ],
)

py_library(
    name = "uff_library",
    srcs = glob(["usr/lib/python3.6/dist-packages/uff/**",
                 "usr/lib/python3.6/dist-packages/graphsurgeon/**"]),
    imports = ["usr/lib/python3.6/dist-packages/"],
    visibility = ["//visibility:public"],
)
