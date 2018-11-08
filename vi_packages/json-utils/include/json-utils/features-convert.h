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

#include <features/features.h>

#include "json-convert.h"
#include "min-convert.h"

template<>
struct  ConvertibleToJsonTraits<features::KeyPoint>
{
  static void asJsonValueInplace(Json::Value &dst,
                                 features::KeyPoint const &pt)
  {
    ensureValueType(dst, Json::objectValue);
    dst["coordinates"] = asJsonValue<MinPoint>(pt.getCoordinates());
    dst["size"]        = asJsonValue<int>(pt.getSize());
    dst["orientation"] = asJsonValue<double>(pt.getOrientation());
    dst["score"]       = asJsonValue<double>(pt.getScore());
  }
};

template<>
struct ConvertibleFromJsonTraits<features::KeyPoint>
{
  static features::KeyPoint fromJson(Json::Value const &v)
  {
    assertValueType(v, Json::objectValue);

    features::KeyPoint pt;
    pt.setCoordinates(::fromJson<MinPoint>(v["coordinates"]));
    pt.setSize(::fromJson<int>(v["size"]));
    pt.setOrientation(::fromJson<double>(v["orientation"]));
    pt.setScore(::fromJson<double>(v["score"]));

    return pt;
  }
};