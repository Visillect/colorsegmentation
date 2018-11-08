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

#include "json-convert.h"

#include <boost/optional.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/array.hpp>

#include <string>

template <>
struct ConvertibleToJsonTraits<boost::posix_time::ptime>
{
  static void asJsonValueInplace(Json::Value &dst, boost::posix_time::ptime const& t)
  {
    dst = ::asJsonValue<std::string>(boost::posix_time::to_iso_string(t));
  }
};

template <>
struct ConvertibleFromJsonTraits<boost::posix_time::ptime>
{
  static boost::posix_time::ptime fromJson(Json::Value const &v)
  {
    return boost::posix_time::from_iso_string(::fromJson<std::string>(v));
  }
};

template <>
struct ConvertibleToJsonTraits<boost::posix_time::time_duration>
{
  static void asJsonValueInplace(Json::Value &dst, boost::posix_time::time_duration const& t)
  {
    dst = ::asJsonValue<std::string>(boost::posix_time::to_simple_string(t));
  }
};

template <>
struct ConvertibleFromJsonTraits<boost::posix_time::time_duration>
{
  static boost::posix_time::time_duration fromJson(Json::Value const &v)
  {
    return boost::posix_time::duration_from_string(::fromJson<std::string>(v));
  }
};


template <typename T>
struct ConvertibleToJsonTraits<boost::optional<T> >
{
  static void asJsonValueInplace(Json::Value &dst, boost::optional<T> const& opt)
  {
    if (opt)
      dst = asJsonValue(*opt);
    else
      dst = Json::Value(Json::nullValue);
  }
};

template <typename T>
struct ConvertibleFromJsonTraits<boost::optional<T> >
{
  static boost::optional<T> fromJson(Json::Value const &v)
  {
    boost::optional<T> result;
    if (!v.isNull())
      result = ::fromJson<T>(v);
    return result;
  }
};

template <class T, std::size_t Size>
struct ConvertibleToJsonTraits<boost::array<T, Size> >
{
  static void asJsonValueInplace(Json::Value &dst, boost::array<T, Size> const& s)
  {
    ensureValueType(dst, Json::arrayValue);
    for (size_t i = 0; i < s.size(); ++i)
      dst.append(asJsonValue(s[i]));
  }
};

template <class T, std::size_t Size>
struct ConvertibleFromJsonTraits<boost::array<T, Size> >
{
  static boost::array<T, Size> fromJson(Json::Value const &v)
  {
    boost::array<T, Size> dst;

    assertValueType(v, Json::arrayValue);
    if (static_cast<size_t>(v.size()) != Size)
      throw JsonConvertException(v, "fromJson<boost::array>: sizeof json array (" +
                                    std::to_string(v.size()) + ") != sizeof dst array (" +
                                    std::to_string(Size) + ")");

    for (Json::UInt i = 0; i < v.size(); ++i)
      dst[static_cast<size_t>(i)] = ::fromJson<T>(v[i]);

    return dst;
  }
};

template <>
struct ConvertibleToJsonTraits<boost::logic::tribool>
{
  static void asJsonValueInplace(Json::Value &dst, boost::logic::tribool const& v)
  {
    if (boost::logic::indeterminate(v))
      return;
    ::asJsonValue<bool>(v == true);
  }
};

