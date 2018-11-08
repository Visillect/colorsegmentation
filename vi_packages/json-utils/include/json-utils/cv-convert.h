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
#include <string>


template <>
struct ConvertibleToJsonTraits<cv::Point3d>
{
  static void asJsonValueInplace(Json::Value &dst, cv::Point3d const& p)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(asJsonValue(p.x));
    dst.append(asJsonValue(p.y));
    dst.append(asJsonValue(p.z));
  }
};

template<typename T>
struct ConvertibleToJsonTraits<cv::Size_<T>>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 cv::Size_<T> const &sz)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(::asJsonValue(sz.width));
    dst.append(::asJsonValue(sz.height));
  }
};

template<typename T>
struct ConvertibleFromJsonTraits<cv::Size_<T> >
{
  static cv::Size_<T> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 2)
      throw JsonConvertException(v, "fromJson<cv::Size>: bad size of json array (" +
                                     std::to_string(v.size()) + ") (2 expected)");

    return { ::fromJson<T>(v[0u]),
             ::fromJson<T>(v[1u]) };
  }
};

template<typename T>
struct ConvertibleToJsonTraits<cv::Point_<T>>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 cv::Point_<T> const &pt)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(::asJsonValue(pt.x));
    dst.append(::asJsonValue(pt.y));
  }
};

template<typename T>
struct ConvertibleFromJsonTraits<cv::Point_<T> >
{
  static cv::Point_<T> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 2)
      throw JsonConvertException(v, "fromJson<cv::Point>bad size of json array (" +
                                     std::to_string(v.size()) + ") (2 expected)");

    return { ::fromJson<T>(v[0u]),
             ::fromJson<T>(v[1u]) };
  }
};

template<typename T>
struct ConvertibleToJsonTraits<cv::Point3_<T>>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 cv::Point3_<T> const &pt)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(::asJsonValue(pt.x));
    dst.append(::asJsonValue(pt.y));
    dst.append(::asJsonValue(pt.z));
  }
};

template<typename T>
struct ConvertibleFromJsonTraits<cv::Point3_<T> >
{
  static cv::Point3_<T> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 3)
      throw JsonConvertException(v, "fromJson<cv::Point3>bad size of json array (" +
                                     std::to_string(v.size()) + ") (3 expected)");

    return { ::fromJson<T>(v[0u]),
             ::fromJson<T>(v[1u]),
             ::fromJson<T>(v[2u]) };
  }
};

template<typename T>
struct ConvertibleToJsonTraits<cv::Rect_<T>>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 cv::Rect_<T> const &r)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(::asJsonValue(r.x));
    dst.append(::asJsonValue(r.y));
    dst.append(::asJsonValue(r.width));
    dst.append(::asJsonValue(r.height));
  }
};

template<typename T>
struct ConvertibleFromJsonTraits<cv::Rect_<T> >
{
  static cv::Rect_<T> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 4)
      throw JsonConvertException(v, "fromJson<cv::Rect>bad size of json array (" +
                                     std::to_string(v.size()) + ") (4 expected)");

    return { ::fromJson<T>(v[0u]),
             ::fromJson<T>(v[1u]),
             ::fromJson<T>(v[2u]),
             ::fromJson<T>(v[3u]) };
  }
};
