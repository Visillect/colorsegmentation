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

#include <set>
#include <vector>
#include <array>
#include <stdexcept>
#include <cmath>
#include <string>

#include <json-cpp/json.h>

class JsonConvertException : public std::runtime_error
{
public:
  JsonConvertException(std::string const& what)
  : std::runtime_error("JsonConvertException: " + what)
  { }

  ~JsonConvertException() throw() { }

  JsonConvertException(Json::Value const& json, std::string const& what)
    : std::runtime_error("JsonConvertException: (json=`" + value_str(json) + "`): " + what)
  { }

private:
  static std::string value_str(Json::Value const& js)
  {
    static Json::FastWriter writer;
    return writer.write(js);
  }
};

inline void ensureValueType(Json::Value &v, Json::ValueType type)
{
  if (v.type() != type)
    v = Json::Value(type);
}

inline void assertValueType(Json::Value const &v, Json::ValueType type)
{
  if (v.type() != type)
    throw JsonConvertException(v, "assertValueType failed (" + std::to_string(type) +
                               " expected, real type=" + std::to_string(v.type()) + ")");
}

template <typename T>
struct ConvertibleToJsonTraits
{
  static void asJsonValueInplace(Json::Value &dst, T const& v)
  {
    dst = Json::Value(v);
  }
};

template <typename T>
inline void asJsonValueInplace(Json::Value &dst, T const& v)
{
  ConvertibleToJsonTraits<T>::asJsonValueInplace(dst, v);
}

template <typename T>
inline Json::Value asJsonValue(T const& v)
{
  Json::Value dst;
  asJsonValueInplace(dst, v);
  return dst;
}

template<>
struct ConvertibleToJsonTraits<char>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 char const &typ)
  {
    ensureValueType(dst, Json::objectValue);
    dst = ::asJsonValue<int>(static_cast<int>(typ));
  }
};

template<>
struct ConvertibleToJsonTraits<unsigned char>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 unsigned char const &typ)
  {
    ensureValueType(dst, Json::objectValue);
    dst = ::asJsonValue<int>(static_cast<unsigned int>(typ));
  }
};

template <>
struct ConvertibleToJsonTraits<char const*>
{
  static void asJsonValueInplace(Json::Value &dst, char const* const& v)
  {
    if (!v)
      dst = Json::Value(Json::nullValue);
    else
      dst = asJsonValue<std::string>(v);
  }
};

template <>
struct ConvertibleToJsonTraits<double>
{
  static void asJsonValueInplace(Json::Value &dst, double const& v)
  {
    if (std::isfinite(v))
      dst = Json::Value(v);
    else
      dst = Json::Value(Json::nullValue);
  }
};

template<>
struct ConvertibleToJsonTraits<float>
{
  static void asJsonValueInplace(Json::Value &dst, float const& v)
  {
    if (std::isfinite(v))
      dst = asJsonValue(static_cast<double>(v));
    else
      dst = Json::Value(Json::nullValue);
  }
};

template<>
struct ConvertibleToJsonTraits<long double>
{
  static void asJsonValueInplace(Json::Value &dst, long double const& v)
  {
    if (std::isfinite(v))
      dst = asJsonValue(static_cast<double>(v));
    else
      dst = Json::Value(Json::nullValue);
  }
};

template<>
struct ConvertibleToJsonTraits<short>
{
  static void asJsonValueInplace(Json::Value &dst, short const& v)
  {
    dst = asJsonValue(static_cast<Json::Int>(v));
  }
};

template<>
struct ConvertibleToJsonTraits<long>
{
  static void asJsonValueInplace(Json::Value &dst, long const& v)
  {
    dst = asJsonValue(static_cast<Json::Int>(v));
  }
};

template<>
struct ConvertibleToJsonTraits<long long>
{
  static void asJsonValueInplace(Json::Value &dst, long long const& v)
  {
    dst = asJsonValue(static_cast<Json::Int>(v));
  }
};

template<>
struct ConvertibleToJsonTraits<unsigned short>
{
  static void asJsonValueInplace(Json::Value &dst, unsigned short const& v)
  {
    dst = asJsonValue(static_cast<Json::Int>(v));
  }
};


template<>
struct ConvertibleToJsonTraits<unsigned long>
{
  static void asJsonValueInplace(Json::Value &dst, unsigned long const& v)
  {
    dst = asJsonValue(static_cast<Json::UInt>(v));
  }
};

template<>
struct ConvertibleToJsonTraits<unsigned long long>
{
  static void asJsonValueInplace(Json::Value &dst, unsigned long long const& v)
  {
    dst = asJsonValue(static_cast<Json::UInt>(v));
  }
};


template<typename T, size_t S>
struct ConvertibleToJsonTraits<T[S]>
{
  static void asJsonValueInplace(Json::Value& dst, T const(& v)[S])
  {
    ensureValueType(dst, Json::arrayValue);
    for (size_t i = 0; i < S; ++i)
      dst.append(asJsonValue(v[i]));
  }
};

template <typename T>
struct ConvertibleToJsonTraits<std::set<T> >
{
  static void asJsonValueInplace(Json::Value &dst, std::set<T> const& s)
  {
    ensureValueType(dst, Json::arrayValue);
    for (typename std::set<T>::const_iterator i = s.begin(); i != s.end(); ++i)
      dst.append(asJsonValue(*i));
  }
};

template <typename T>
struct ConvertibleToJsonTraits<std::vector<T> >
{
  static void asJsonValueInplace(Json::Value &dst, std::vector<T> const& s)
  {
    ensureValueType(dst, Json::arrayValue);
    for (size_t i = 0; i < s.size(); ++i)
      dst.append(asJsonValue(s[i]));
  }
};

template <class T, std::size_t Size>
struct ConvertibleToJsonTraits<std::array<T, Size> >
{
  static void asJsonValueInplace(Json::Value &dst, std::array<T, Size> const& s)
  {
    ensureValueType(dst, Json::arrayValue);
    for (size_t i = 0; i < s.size(); ++i)
      dst.append(asJsonValue(s[i]));
  }
};


template <typename T>
struct ConvertibleToJsonTraits<std::map<std::string, T> >
{
  static void asJsonValueInplace(Json::Value &dst, std::map<std::string, T> const& s)
  {
    ensureValueType(dst, Json::objectValue);
    for (auto const kv : s)
      ::asJsonValueInplace(dst[kv->first], kv->second);
  }
};

template <typename T1, typename T2>
struct ConvertibleToJsonTraits<std::pair<T1, T2> >
{
  static void asJsonValueInplace(Json::Value &dst, std::pair<T1, T2> const& v)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(::asJsonValue(v.first));
    dst.append(::asJsonValue(v.second));
  }
};

template <typename T>
struct ConvertibleFromJsonTraits
{
  static T fromJson(Json::Value const &v)
  {
    return T::fromJson(v);
  }
  static void fromJsonInplace(T& v, Json::Value const &js)
  {
    v = fromJson(js);
  }
};

template <typename T>
inline T fromJson(Json::Value const &v)
{
  return ConvertibleFromJsonTraits<T>::fromJson(v);
};

template <typename T>
inline void fromJsonInplace(T& v, Json::Value const &js)
{
  ConvertibleFromJsonTraits<T>::fromJsonInplace(v, js);
};

template <typename T>
inline T fromJson(Json::Value const &v, T const& default_value)
{
  if (v.isNull())
    return default_value;
  else
    return ConvertibleFromJsonTraits<T>::fromJson(v);
};

template <>
struct ConvertibleFromJsonTraits<int>
{
  static int fromJson(Json::Value const &v)
  {
    return v.asInt();
  }
};

template <>
struct ConvertibleFromJsonTraits<unsigned int>
{
  static unsigned int fromJson(Json::Value const &v)
  {
    return v.asUInt();
  }
};

template <>
struct ConvertibleFromJsonTraits<short>
{
  static int fromJson(Json::Value const &v)
  {
    return v.asInt();
  }
};

template <>
struct ConvertibleFromJsonTraits<unsigned short>
{
  static unsigned int fromJson(Json::Value const &v)
  {
    return v.asUInt();
  }
};

template <>
struct ConvertibleFromJsonTraits<long>
{
  static long fromJson(Json::Value const &v)
  {
    return static_cast<long>(v.asInt());
  }
};

template <>
struct ConvertibleFromJsonTraits<unsigned long>
{
  static unsigned long fromJson(Json::Value const &v)
  {
    return static_cast<unsigned long>(v.asUInt());
  }
};

template <>
struct ConvertibleFromJsonTraits<long long>
{
  static long long fromJson(Json::Value const &v)
  {
    return static_cast<long long>(v.asInt());
  }
};

template <>
struct ConvertibleFromJsonTraits<unsigned long long>
{
  static unsigned long long fromJson(Json::Value const &v)
  {
    return static_cast<unsigned long long>(v.asUInt());
  }
};

template <>
struct ConvertibleFromJsonTraits<double>
{
  static double fromJson(Json::Value const &v)
  {
    return v.asDouble();
  }
};

template <>
struct ConvertibleFromJsonTraits<long double>
{
  static long double fromJson(Json::Value const &v)
  {
    return static_cast<long double>(v.asDouble());
  }
};

template <>
struct ConvertibleFromJsonTraits<float>
{
  static float fromJson(Json::Value const &v)
  {
    return static_cast<float>(v.asDouble());
  }
};

template <>
struct ConvertibleFromJsonTraits<bool>
{
  static bool fromJson(Json::Value const &v)
  {
    return v.asBool();
  }
};

template <>
struct ConvertibleFromJsonTraits<std::string>
{
  static std::string fromJson(Json::Value const &v)
  {
    return v.asString();
  }
};

template <>
struct ConvertibleFromJsonTraits<char>
{
  static char fromJson(Json::Value const &v)
  {
    return static_cast<char>(::fromJson<int>(v));
  }
};

template <>
struct ConvertibleFromJsonTraits<unsigned char>
{
  static unsigned char fromJson(Json::Value const &v)
  {
    return static_cast<unsigned char>(::fromJson<unsigned int>(v));
  }
};

template <typename T>
struct ConvertibleFromJsonTraits<std::vector<T> >
{
  static std::vector<T> fromJson(Json::Value const &v)
  {
    std::vector<T> dst;

    assertValueType(v, Json::arrayValue);
    for (Json::UInt i = 0; i < v.size(); ++i)
      dst.push_back(::fromJson<T>(v[i]));

    return dst;
  }
};

template <class T, std::size_t Size>
struct ConvertibleFromJsonTraits<std::array<T, Size> >
{
  static std::array<T, Size> fromJson(Json::Value const &v)
  {
    std::array<T, Size> dst;

    assertValueType(v, Json::arrayValue);
    if (static_cast<size_t>(v.size()) != Size)
      throw JsonConvertException(v, "fromJson<std::array>: size of json array (" + std::to_string(v.size()) +
                                 ") != size of dst array (" + std::to_string(Size) + ")");

    for (Json::UInt i = 0; i < v.size(); ++i)
      dst[static_cast<size_t>(i)] = ::fromJson<T>(v[i]);

    return dst;
  }
};

template <typename T>
struct ConvertibleFromJsonTraits<std::map<std::string, T> >
{
  static std::map<std::string, T> fromJson(Json::Value const &v)
  {
    std::map<std::string, T> dst;
    assertValueType(v, Json::objectValue);
    for (std::string const& k : v.getMemberNames())
      dst[k] = ::fromJson<T>(v[k]);
    return dst;
  }
};

template <typename T>
void asJsonArrayInplace(Json::Value & dst, T const* arr, size_t n)
{
  ensureValueType(dst, Json::arrayValue);
  for (size_t i = 0; i < n; ++i)
  {
    dst.append(::asJsonValue(arr[i]));
  }
}
