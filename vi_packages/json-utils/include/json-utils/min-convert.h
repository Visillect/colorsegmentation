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
#include <minbase/mintyp.h>
#include <minbase/minimg.h>

#include "json-convert.h"

//-------------to json----------------------------------------------------------
template<>
struct ConvertibleToJsonTraits<MinTyp>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 MinTyp const &typ)
  {
    ensureValueType(dst, Json::objectValue);

    std::string type_id;
    switch (typ)
    {
      case TYP_UINT1:
        type_id = "TYP_UINT1";
        break;
      case TYP_UINT8:
        type_id = "TYP_UINT8";
        break;
      case TYP_INT8:
        type_id = "TYP_INT8";
        break;
      case TYP_UINT16:
        type_id = "TYP_UINT16";
        break;
      case TYP_INT16:
        type_id = "TYP_INT16";
        break;
      case TYP_REAL16:
        type_id = "TYP_REAL16";
        break;
      case TYP_UINT32:
        type_id = "TYP_UINT32";
        break;
      case TYP_INT32:
        type_id = "TYP_INT32";
        break;
      case TYP_REAL32:
        type_id = "TYP_REAL32";
        break;
      case TYP_UINT64:
        type_id = "TYP_UINT64";
        break;
      case TYP_INT64:
        type_id = "TYP_INT64";
        break;
      case TYP_REAL64:
        type_id = "TYP_REAL64";
        break;
      default:
        throw JsonConvertException("asJsonValueInplace<MinTyp>: not implemented conversion from MinTyp=" +
                                   std::to_string(static_cast<int>(typ)) + " to JsonValue");
    }
    dst = ::asJsonValue<std::string>(type_id);
  }
};

template<typename CT>
struct ConvertibleToJsonTraits<se::rect2<CT> >
{
  static void asJsonValueInplace(Json::Value &dst,
                                 se::rect2<CT> const &rect)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(rect.x);
    dst.append(rect.y);
    dst.append(rect.width);
    dst.append(rect.height);
  }
};

template<typename CT>
struct ConvertibleToJsonTraits<se::pnt2<CT> >
{
  static void asJsonValueInplace(Json::Value &dst,
                                 se::pnt2<CT>  const &point)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(point.x);
    dst.append(point.y);
  }
};

template<typename CT>
struct ConvertibleToJsonTraits<se::pnt3<CT> >
{
  static void asJsonValueInplace(Json::Value &dst,
                                 se::pnt3<CT>  const &point)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(point.x);
    dst.append(point.y);
    dst.append(point.z);
  }
};


template<typename CT>
struct ConvertibleToJsonTraits<se::size2<CT> >
{
  static void asJsonValueInplace(Json::Value &dst,
                                 se::size2<CT>  const &size)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(size.width);
    dst.append(size.height);
  }
};

template<typename CT>
struct ConvertibleToJsonTraits<se::quad2<CT> >
{
  static void asJsonValueInplace(Json::Value &dst,
                                 se::quad2<CT> const &quadr)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(asJsonValue(quadr.a));
    dst.append(asJsonValue(quadr.b));
    dst.append(asJsonValue(quadr.c));
    dst.append(asJsonValue(quadr.d));
  }
};

//-------------from json--------------------------------------------------------

template<>
struct ConvertibleFromJsonTraits<MinTyp>
{
  static MinTyp fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::stringValue);
    std::string const type_id = ::fromJson<std::string>(v);

    MinTyp ret;

    if (type_id == "TYP_UINT1")
      ret = TYP_UINT1;
    else if (type_id == "TYP_UINT8")
      ret = TYP_UINT8;
    else if (type_id == "TYP_INT8")
      ret = TYP_INT8;
    else if (type_id == "TYP_UINT16")
      ret = TYP_UINT16;
    else if (type_id == "TYP_INT16")
      ret = TYP_INT16;
    else if (type_id == "TYP_REAL16")
      ret = TYP_REAL16;
    else if (type_id == "TYP_UINT32")
      ret = TYP_UINT32;
    else if (type_id == "TYP_INT32")
      ret = TYP_INT32;
    else if (type_id == "TYP_REAL32")
      ret = TYP_REAL32;
    else if (type_id == "TYP_UINT64")
      ret = TYP_UINT64;
    else if (type_id == "TYP_INT64")
      ret = TYP_INT64;
    else if (type_id == "TYP_REAL64")
      ret = TYP_REAL64;
    else
      throw JsonConvertException(v, "not implemented conversion from type id " + type_id +
                                 " to MinTyp");

    return ret;
  }
};

template<>
struct ConvertibleToJsonTraits<MinLineSegment>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 MinLineSegment const &s)
  {
    ensureValueType(dst, Json::arrayValue);
    dst.append(asJsonValue(s.u));
    dst.append(asJsonValue(s.v));
  }
};

template<typename CT>
struct ConvertibleFromJsonTraits<se::rect2<CT> >
{
  static se::rect2<CT> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 4)
      throw JsonConvertException(v, "fromJson<se::rect2>: unexpected array size");

    se::rect2<CT> result;

    return se::rect2<CT>(::fromJson<CT>(v[0u]),
                         ::fromJson<CT>(v[1u]),
                         ::fromJson<CT>(v[2u]),
                         ::fromJson<CT>(v[3u]));
  }
};

template<typename CT>
struct ConvertibleFromJsonTraits<se::pnt2<CT> >
{
  static se::pnt2<CT> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 2)
      throw JsonConvertException(v, "fromJson<se::pnt2>: unexpected array size");

    return se::pnt2<CT>(::fromJson<CT>(v[0u]),
                        ::fromJson<CT>(v[1u]));
  }
};

template<typename CT>
struct ConvertibleFromJsonTraits<se::pnt3<CT> >
{
  static se::pnt3<CT> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 3)
      throw JsonConvertException(v, "fromJson<se::pnt3>: unexpected array size");

    return se::pnt3<CT>(::fromJson<CT>(v[0u]),
                        ::fromJson<CT>(v[1u]),
                        ::fromJson<CT>(v[2u]));
  }
};

template<typename CT>
struct ConvertibleFromJsonTraits<se::size2<CT> >
{
  static se::size2<CT> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 2)
      throw JsonConvertException(v, "fromJson<se::size2>: unexpected array size");

    return se::size2<CT>(::fromJson<CT>(v[0u]),
                         ::fromJson<CT>(v[1u]));
  }
};

template<typename CT>
struct ConvertibleFromJsonTraits<se::quad2<CT> >
{
  static se::quad2<CT> fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::arrayValue);
    if (v.size() != 4)
      throw JsonConvertException(v, "fromJson<se::quad2>: unexpected array size");

    se::quad2<CT> result;
    result.a = ::fromJson<se::pnt2<CT> >(v[0u]);
    result.b = ::fromJson<se::pnt2<CT> >(v[1u]);
    result.c = ::fromJson<se::pnt2<CT> >(v[2u]);
    result.d = ::fromJson<se::pnt2<CT> >(v[3u]);

    return result;
  }
};
