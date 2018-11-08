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

#include <minbase/crossplat.h>
THIRDPARTY_INCLUDES_BEGIN
#include <Eigen/Dense>
THIRDPARTY_INCLUDES_END

#include <string>

template <class T, int Rows>
struct ConvertibleToJsonTraits<Eigen::Matrix<T, Rows, 1> >
{
  static void asJsonValueInplace(Json::Value& dst, Eigen::Matrix<T, Rows, 1> const& m)
  {
    ensureValueType(dst, Json::arrayValue);
    ::asJsonValueInplace(dst, std::vector<T>(m.data(), m.data() + m.rows() * m.cols()));
  }
};

template <class T, int Rows>
struct ConvertibleFromJsonTraits<Eigen::Matrix<T, Rows, 1> >
{
  static Eigen::Matrix<T, Rows, 1> fromJson(Json::Value const& v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != Rows)
      throw JsonConvertException(v, "fromJson<Eigen::Matrix>: size of json array (" +
                                 std::to_string(v.size()) + "!= Rows (" +
                                 std::to_string(Rows) + ")");

    Eigen::Matrix<T, Rows, 1> dst;

    for (Json::Value::UInt row_i = 0; row_i < v.size(); ++row_i)
      dst(static_cast<size_t>(row_i)) = ::fromJson<T>(v[row_i]);

    return dst;
  }
};


template <class T>
struct ConvertibleToJsonTraits<Eigen::Quaternion<T> >
{
  static void asJsonValueInplace(Json::Value &dst, Eigen::Quaternion<double> const& q)
  {
    ensureValueType(dst, Json::arrayValue);
    ::asJsonValueInplace(dst, std::vector<T> { q.x(), q.y(), q.z(), q.w() });
  }
};

template <class T>
struct ConvertibleFromJsonTraits<Eigen::Quaternion<T> >
{
  static Eigen::Quaternion<T> fromJson(Json::Value const& v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 4)
      throw JsonConvertException(v, "fromJson<Eigen::Quaternion>: bad size of json array (" +
                                 std::to_string(v.size()) + ") (4 expected)");

    Eigen::Quaternion<T> dst;

    dst.x() = ::fromJson<T>(v[0u]);
    dst.y() = ::fromJson<T>(v[1u]);
    dst.z() = ::fromJson<T>(v[2u]);
    dst.w() = ::fromJson<T>(v[3u]);

    return dst;
  }
};

template <class T>
struct ConvertibleFromJsonTraits<Eigen::Matrix<T, Eigen::Dynamic, 1> >
{
  static Eigen::Matrix<T, Eigen::Dynamic, 1> fromJson(Json::Value const& v)
  {
    assertValueType(v, Json::arrayValue);

    Eigen::Matrix<T, Eigen::Dynamic, 1> dst(v.size());

    for (Json::Value::UInt vi = 0; vi < v.size(); ++vi)
      dst[static_cast<size_t>(vi)] = ::fromJson<T>(v[vi]);

    return dst;
  }
};
