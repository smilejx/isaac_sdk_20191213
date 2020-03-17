#!/usr/bin/env bash
#####################################################################################
# Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.

# NVIDIA CORPORATION and its licensors retain all intellectual property
# and proprietary rights in and to this software, related documentation
# and any modifications thereto. Any use, reproduction, disclosure or
# distribution of this software and related documentation without an express
# license agreement from NVIDIA CORPORATION is strictly prohibited.
#####################################################################################

#####################################################################################
# This is a helper script for generating ROS packages to build the Isaac Ros Bridge
# First argument is the ROS version. e.g., kinetic, melodic
# Second argument is the output package name.
# Example usage:
# ./engine/build/scripts/ros_package_generation.sh melodic ros_melodic_x86_64
# This will generate a tar ball that can be uploaded as needed.
# ROS doesn't really support cross compilation so the assumption is that this
# script will run on the local system, i.e., to create a Xavier package, this script
# needs to be run on Xavier.
#####################################################################################
set -e

# This script requires catkin_tools to be installed
sudo pip install -U trollius
sudo pip install -U catkin_tools
PACKAGES="roscpp rospy actionlib_msgs control_msgs diagnostic_msgs geometry_msgs
 map_msgs nav_msgs pcl_msgs sensor_msgs shape_msgs std_msgs stereo_msgs
 tf2_geometry_msgs tf2_msgs trajectory_msgs visualization_msgs move_base_msgs"

rm -rf ros_isaac_ws
mkdir ros_isaac_ws
pushd ros_isaac_ws

rosinstall_generator \
    --rosdistro $1 \
    --deps \
    --flat \
    $PACKAGES > ws.rosinstall
wstool init -j8 src ws.rosinstall

catkin config \
    --install \
    --source-space src \
    --build-space build \
    --devel-space devel \
    --log-space log \
    --install-space install \
    --isolate-devel \
    --no-extend

catkin build

popd
rm -rf $2
mkdir  $2
pushd $2

cp -r ../ros_isaac_ws/install/lib .
cp -r ../ros_isaac_ws/install/include .

OUTNAME=$2".tar.gz"

tar cfz $OUTNAME include/ lib/
mv $OUTNAME ..
popd

rm -rf ros_isaac_ws
rm -rf $2

echo 'For Xavier, follow the steps listed below:'
echo '1. manually add the following libraries to the tar.gz generated by this script (Version numbers may change),'
echo '   liblog4cxx.so.10, libboost_system.so.1.65.1, libboost_regex.so.1.65.1, libboost_thread.so.1.65.1, libboost_chrono.so.1.65.1,'
echo '   libboost_filesystem.so.1.65.1, libconsole_bridge.so.0.4'
echo '   libapr-1.so.0, libaprutil-1.so.0, libicui18n.so.60, libicuuc.so.60'
echo '   libexpat.so.1, libicudata.so.60'
echo '2. In case include directory does not have boost, copy boost include folder, potentially located at "/usr/include/boost/".'