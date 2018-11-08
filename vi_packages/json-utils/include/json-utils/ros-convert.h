/*
Copyright (c) 2012-2018, Visillect Service LLC. All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of copyright holders.
*/


#pragma once
#include <json-utils/json-convert.h>
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Point.h"


template <>
struct ConvertibleToJsonTraits<ros::Time>
{
  static void asJsonValueInplace(Json::Value &dst, ros::Time const& v)
  {
    ::asJsonValueInplace(dst, v.toBoost());
  }
};

template <>
struct ConvertibleToJsonTraits<geometry_msgs::Point>
{
  static void asJsonValueInplace(Json::Value &dst, geometry_msgs::Point const& v)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(v.x);
    dst.append(v.y);
    dst.append(v.z);
  }
};

template <>
struct ConvertibleToJsonTraits<std_msgs::Header>
{
  static void asJsonValueInplace(Json::Value &dst, std_msgs::Header const& v)
  {
    ensureValueType(dst, Json::objectValue);
    ::asJsonValueInplace(dst["stamp"], v.stamp);
    ::asJsonValueInplace(dst["frame_id"], v.frame_id);
  }
};

template <>
struct ConvertibleToJsonTraits<geometry_msgs::Quaternion>
{
  static void asJsonValueInplace(Json::Value &dst, geometry_msgs::Quaternion const& v)
  {
    ensureValueType(dst, Json::arrayValue);
    ::asJsonValueInplace(dst, std::vector<double> { v.x, v.y, v.z, v.w });
  }
};

template <>
struct ConvertibleToJsonTraits<geometry_msgs::Vector3>
{
  static void asJsonValueInplace(Json::Value &dst, geometry_msgs::Vector3 const& v)
  {
    ensureValueType(dst, Json::arrayValue);
    ::asJsonValueInplace(dst, std::vector<double> { v.x, v.y, v.z });
  }
};

template <>
struct ConvertibleToJsonTraits<sensor_msgs::Imu>
{
  static void asJsonValueInplace(Json::Value &dst, sensor_msgs::Imu const& m)
  {
    ensureValueType(dst, Json::objectValue);
    ::asJsonValueInplace(dst["header"], m.header);
    ::asJsonValueInplace(dst["orientation"], m.orientation);
    ::asJsonValueInplace(dst["angular_velocity"], m.angular_velocity);
    ::asJsonValueInplace(dst["linear_acceleration"], m.linear_acceleration);
  }
};

template <>
struct ConvertibleFromJsonTraits<ros::Duration>
{
  static ros::Duration fromJson(Json::Value const &v)
  {
    return ros::Duration(::fromJson<double>(v));
  }
};

template <>
struct ConvertibleToJsonTraits<tf::Transform>
{
  static void asJsonValueInplace(Json::Value &dst, tf::Transform const& v)
  {
    ensureValueType(dst, Json::objectValue);
    ::asJsonValueInplace(dst["t"], v.getOrigin());
    ::asJsonValueInplace(dst["r"], v.getRotation());
  }
};

