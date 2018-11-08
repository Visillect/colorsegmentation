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
#include <mingeo/mingeo.h>
#include <geometry_msgs/Point.h>
#include <geometry_msgs/Point32.h>


namespace vi { namespace cvt { namespace ros {

inline MinPoint3d as_minpoint(geometry_msgs::Point const& pt) {
  return { pt.x, pt.y, pt.z };
}

inline geometry_msgs::Point as_point_msg(MinPoint3d const& pt) {
  geometry_msgs::Point r;
  r.x = pt.x;
  r.y = pt.y;
  r.z = pt.z;
  return r;
}

inline MinPoint3f as_minpoint(geometry_msgs::Point32 const& pt) {
  return { pt.x, pt.y, pt.z };
}

inline geometry_msgs::Point32 as_point_msg(MinPoint3f const& pt) {
  geometry_msgs::Point32 r;
  r.x = pt.x;
  r.y = pt.y;
  r.z = pt.z;
  return r;
}

inline geometry_msgs::Point32 to_point32_msg(MinPoint3d const& pt) {
  geometry_msgs::Point32 r;
  r.x = static_cast<float>(pt.x);
  r.y = static_cast<float>(pt.y);
  r.z = static_cast<float>(pt.z);
  return r;
}

inline geometry_msgs::Point as_point_msg(MinPoint2f const& pt) {
  geometry_msgs::Point r;
  r.x = pt.x;
  r.y = pt.y;
  r.z = 0.0;
  return r;
}

}}} // ns vi::cvt::ros
