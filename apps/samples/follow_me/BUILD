"""
Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
"""

load("//engine/build:isaac.bzl", "isaac_app")

isaac_app(
    name = "follow_me",
    data = [
        "//apps/carter:2d_carter_subgraph",
        "//packages/navigation/apps:differential_base_commander_subgraph",
        "//packages/navigation/apps:differential_base_control_subgraph",
        "//packages/navigation/apps:local_map_subgraph",
    ],
    modules = [
        "imu",
        "navigation",
        "perception",
        "perception:april_tags",
        "planner",
        "segway",
        "sensors:joystick",
        "velodyne_lidar",
        "viewers",
        "zed",
    ],
)
