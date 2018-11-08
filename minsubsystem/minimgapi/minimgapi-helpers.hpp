/*
Copyright (c) 2011-2013, Smart Engines Limited. All rights reserved.

All rights reserved.

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

/**
 * @file   minimgapi-helpers.hpp
 * @brief  MinImgAPI c++ helpers interface.
 */

#pragma once
#ifndef MINIMGAPI_MINIMGAPI_HELPERS_HPP_INCLUDED
#define MINIMGAPI_MINIMGAPI_HELPERS_HPP_INCLUDED

#include <minbase/minresult.h>
#include <minbase/crossplat.h>
#include <mingeo/mingeo.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/minimgapi-inl.h>

/**
 * @brief   Returns format that corresponds to the given template parameter
 *          type.
 * @arg     <Type> parameter type.
 * @returns Appropriate format (see @c #MinFmt).
 * @ingroup MinImgAPI_API
 *
 * The function returns the format that corresponds to the given template
 * parameter type (see @c #MinFmt).
 */

template<typename Type> MUSTINLINE MinFmt GetMinFmtByCType();

/// @cond
template<> MinFmt MUSTINLINE GetMinFmtByCType<uint8_t>()  { return FMT_UINT; }
template<> MinFmt MUSTINLINE GetMinFmtByCType<uint16_t>() { return FMT_UINT; }
template<> MinFmt MUSTINLINE GetMinFmtByCType<uint32_t>() { return FMT_UINT; }
template<> MinFmt MUSTINLINE GetMinFmtByCType<uint64_t>() { return FMT_UINT; }
template<> MinFmt MUSTINLINE GetMinFmtByCType<int8_t>()   { return FMT_INT;  }
template<> MinFmt MUSTINLINE GetMinFmtByCType<int16_t>()  { return FMT_INT;  }
template<> MinFmt MUSTINLINE GetMinFmtByCType<int32_t>()  { return FMT_INT;  }
template<> MinFmt MUSTINLINE GetMinFmtByCType<int64_t>()  { return FMT_INT;  }
template<> MinFmt MUSTINLINE GetMinFmtByCType<real16_t>() { return FMT_REAL; }
template<> MinFmt MUSTINLINE GetMinFmtByCType<real32_t>() { return FMT_REAL; }
template<> MinFmt MUSTINLINE GetMinFmtByCType<real64_t>() { return FMT_REAL; }
/// @endcond

/**
 * @brief   Returns type (MinTyp value) that corresponds to the given template
 *          parameter type.
 * @arg     <Type> parameter type.
 * @returns Appropriate type (MinTyp value) (see @c #MinTyp).
 * @ingroup MinImgAPI_API
 *
 * The function returns the type (MinTyp value) that corresponds to the given
 * template parameter type (see @c #MinTyp).
 */

template<typename Type> MUSTINLINE MinTyp GetMinTypByCType() {
  return static_cast<MinTyp>(_GetTypByFmtAndDepth(GetMinFmtByCType<Type>(),
                                                  sizeof(Type)));
}

/// @cond
template<> MUSTINLINE MinTyp GetMinTypByCType<bool>() {
  return TYP_UINT1;
}
/// @endcond

template<typename T> MUSTINLINE T *GetMinImageLineAs(
    const MinImg* image,
    int           y,
    BorderOption  border   = BO_VOID,
    T*            p_canvas = NULL) {
  return reinterpret_cast<T*>(_GetMinImageLine(image, y, border, p_canvas));
}

/**
 * @brief   Properly copies image to another one regardless of destination image
 *          allocation status.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 *
 * The function copies @c p_src_image to @c p_dst_image whatever @c p_dst_image
 * status is. If @c p_dst_image is already allocated it's freed and then
 * allocated again.
 */
static MUSTINLINE int AssignMinImage(
    MinImg*       p_dst_image,
    const MinImg* p_src_image) {
  PROPAGATE_ERROR(FreeMinImage(p_dst_image));
  PROPAGATE_ERROR(CloneMinImagePrototype(p_dst_image, p_src_image));
  return CopyMinImage(p_dst_image, p_src_image);
}

/**
 * @brief   Gets a region of an image by given rect.
 * @param   p_dst_image The destination image.
 * @param   p_src_image The source image.
 * @param   rect        The rect defining the region to get.
 * @param   rules       The degree of validation.
 * @returns @c NO_ERRORS on success or an error code otherwise (see @c #MinErr).
 * @ingroup MinImgAPI_API
 *
 * The function get a subimage from the source image. Note, that the function
 * <b>does not</b> make a copy of the specified region. Therefore, <b>it is
 * strongly forbidden</b> to call @c FreeMinImage() for the @c p_dst_image.
 */
static MUSTINLINE int GetMinImageRegionByRect(
    MinImg*       p_dst_image,
    const MinImg* p_src_image,
    MinRect       rect,
    RulesOption   rules = RO_STRICT) {
  return GetMinImageRegion(p_dst_image, p_src_image,
                           rect.x, rect.y, rect.width, rect.height, rules);
}

#endif // #ifndef MINIMGAPI_MINIMGAPI_HELPERS_HPP_INCLUDED
