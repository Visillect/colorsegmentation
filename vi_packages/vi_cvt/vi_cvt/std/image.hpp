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
#include <limits>
#include <array>
#include <vi_cvt/std/exceptions.hpp>
#include <vi_cvt/std/exception_macros.hpp>
#include <minimgapi/minimgapi-helpers.hpp>
#include <minbase/minresult.h>

namespace vi { namespace cvt {


template <class T>
inline MinImg as_scalar_minimg(T & val) {
  int typ = 0;
  THROW_ON_MINERR(typ = _GetTypByFmtAndDepth(GetMinFmtByCType<T>(), sizeof(T)));
  MinImg mi = {0};
  THROW_ON_MINERR(_WrapScalarWithMinImage(&mi, &val, static_cast<MinTyp>(typ)));
  return mi;
}

template <class T, size_t channels>
inline MinImg as_pixel_minimg(std::array<T, channels> & val) {
  int typ = 0;
  THROW_ON_MINERR(typ = _GetTypByFmtAndDepth(GetMinFmtByCType<T>(), sizeof(T)));
  MinImg mi = {0};
  THROW_ON_MINERR(_WrapPixelWithMinImage(&mi, val.data(), channels, static_cast<MinTyp>(typ)));
  return mi;
}

template <class T>
inline MinImg as_row_minimg(std::vector<T> & val) {
  int typ = 0;
  THROW_ON_MINERR(typ = _GetTypByFmtAndDepth(GetMinFmtByCType<T>(), sizeof(T)));
  MinImg mi = {0};
  THROW_ON_MINERR(_WrapScalarVectorWithMinImage(&mi, val.data(), static_cast<int>(val.size()), DO_HORIZONTAL, static_cast<MinTyp>(typ)));
  return mi;
}

template <class T>
inline MinImg as_col_minimg(std::vector<T> & val) {
  int typ = 0;
  THROW_ON_MINERR(typ = _GetTypByFmtAndDepth(GetMinFmtByCType<T>(), sizeof(T)));
  MinImg mi = {0};
  THROW_ON_MINERR(_WrapScalarVectorWithMinImage(&mi, val.data(), static_cast<int>(val.size()), DO_VERTICAL, static_cast<MinTyp>(typ)));
  return mi;
}

}} // ns vi::cvt
