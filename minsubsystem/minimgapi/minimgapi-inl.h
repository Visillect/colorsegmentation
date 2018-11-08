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
 * @file   minimgapi-inl.h
 * @brief  MinImgAPI inlining interface.
 *
 * This header contains inline versions of some MinimgAPI functions.
 * See description of the functions in file minimgapi.h
 */

#pragma once
#ifndef MINIMGAPI_MINIMGAPI_INL_H_INCLUDED
#define MINIMGAPI_MINIMGAPI_INL_H_INCLUDED

#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <algorithm>

#include <minbase/minresult.h>
#include <minbase/crossplat.h>
#include <minimgapi/minimgapi.h>

MUSTINLINE int _GetFmtByTyp(
    MinTyp typ) {
  switch (typ) {
  case TYP_UINT1:
  case TYP_UINT8:
  case TYP_UINT16:
  case TYP_UINT32:
  case TYP_UINT64:
    return FMT_UINT;
  case TYP_INT8:
  case TYP_INT16:
  case TYP_INT32:
  case TYP_INT64:
    return FMT_INT;
  case TYP_REAL16:
  case TYP_REAL32:
  case TYP_REAL64:
    return FMT_REAL;
  default:
    return BAD_ARGS;
  }
}

MUSTINLINE int _GetDepthByTyp(
    MinTyp typ) {
  switch (typ) {
  case TYP_UINT1:
    return 0;
  case TYP_UINT8:
  case TYP_INT8:
    return 1;
  case TYP_UINT16:
  case TYP_INT16:
  case TYP_REAL16:
    return 2;
  case TYP_UINT32:
  case TYP_INT32:
  case TYP_REAL32:
    return 4;
  case TYP_UINT64:
  case TYP_INT64:
  case TYP_REAL64:
    return 8;
  default:
    return BAD_ARGS;
  }
}

MUSTINLINE int _GetTypByFmtAndDepth(
    MinFmt fmt,
    int    depth) {
  switch (fmt) {
  case FMT_UINT:
    switch (depth) {
    case 0:
      return TYP_UINT1;
    case 1:
      return TYP_UINT8;
    case 2:
      return TYP_UINT16;
    case 4:
      return TYP_UINT32;
    case 8:
      return TYP_UINT64;
    default:
      return BAD_ARGS;
    }
  case FMT_INT:
    switch (depth) {
    case 1:
      return TYP_INT8;
    case 2:
      return TYP_INT16;
    case 4:
      return TYP_INT32;
    case 8:
      return TYP_INT64;
    default:
      return BAD_ARGS;
    }
  case FMT_REAL:
    switch (depth) {
    case 2:
      return TYP_REAL16;
    case 4:
      return TYP_REAL32;
    case 8:
      return TYP_REAL64;
    default:
      return BAD_ARGS;
    }
  default:
    return BAD_ARGS;
  }
}

MUSTINLINE int _GetMinImageType(
    const MinImg *p_image) {
  if (!p_image)
    return BAD_ARGS;
  return _GetTypByFmtAndDepth(p_image->format, p_image->channelDepth);
}

MUSTINLINE int _SetMinImageType(
    MinImg *p_image,
    MinTyp  element_type) {
  if (!p_image || p_image->pScan0)
    return BAD_ARGS;

  int fmt = _GetFmtByTyp(element_type);
  PROPAGATE_ERROR(fmt);
  p_image->format = static_cast<MinFmt>(fmt);
  int depth = _GetDepthByTyp(element_type);
  PROPAGATE_ERROR(depth);
  p_image->channelDepth = depth;

  return NO_ERRORS;
}

MUSTINLINE int _AssureMinImagePrototypeIsValid(
    const MinImg *p_image) {
  PROPAGATE_ERROR(_GetMinImageType(p_image));
  if (p_image->width < 0 || p_image->height < 0 || p_image->channels < 0)
    return BAD_ARGS;
  return NO_ERRORS;
}

MUSTINLINE int _GetMinImageBitsPerPixel(
    const MinImg *p_image) {
  PROPAGATE_ERROR(_AssureMinImagePrototypeIsValid(p_image));

  int bit_depth = p_image->channelDepth ? p_image->channelDepth << 3 : 1;
  return p_image->channels * bit_depth;
}

MUSTINLINE int _GetMinImageBytesPerLine(
    const MinImg *p_image) {
  PROPAGATE_ERROR(_AssureMinImagePrototypeIsValid(p_image));

  int elements_per_line = p_image->width * p_image->channels;
  return p_image->channelDepth > 0 ? elements_per_line * p_image->channelDepth :
                                     (elements_per_line + 7) >> 3;
}

MUSTINLINE int _AssureMinImageIsValid(
    const MinImg *p_image) {
  PROPAGATE_ERROR(_AssureMinImagePrototypeIsValid(p_image));
  if (!p_image->width || !p_image->height || !p_image->channels)
    return NO_ERRORS;
  if (!p_image->pScan0)
    return BAD_ARGS;
  if (p_image->height > 1 &&
      std::abs(p_image->stride) < _GetMinImageBytesPerLine(p_image))
    return BAD_ARGS;
  return NO_ERRORS;
}

MUSTINLINE int _AssureMinImageIsEmpty(
    const MinImg *p_image) {
  PROPAGATE_ERROR(_AssureMinImagePrototypeIsValid(p_image));
  return p_image->width && p_image->height && p_image->channels;
}

MUSTINLINE int _AssureMinImageIsSolid(
    const MinImg *p_image) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_image));

  int bits_per_pixel = _GetMinImageBitsPerPixel(p_image);
  return p_image->height > 1 &&
         p_image->stride << 3 != p_image->width * bits_per_pixel;
}

MUSTINLINE int _AssureMinImageFits(
    const MinImg *p_image,
    MinTyp        element_type,
    int           channels = -1,
    int           width = -1,
    int           height = -1) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_image));

  int retVal = 0;
  PROPAGATE_ERROR(retVal = _GetMinImageType(p_image));
  if (element_type >= 0 && element_type != retVal)
    return 1;
  if (channels >= 0 && channels != p_image->channels)
    return 1;
  if (width >= 0 && width != p_image->width)
    return 1;
  if (height >= 0 && height != p_image->height)
    return 1;

  return NO_ERRORS;
}

MUSTINLINE int _AssureMinImageIsScalar(
    const MinImg *p_image) {
  return _AssureMinImageFits(p_image, static_cast<MinTyp>(-1), 1, 1, 1);
}

MUSTINLINE int _AssureMinImageIsPixel(
    const MinImg *p_image) {
  return _AssureMinImageFits(p_image, static_cast<MinTyp>(-1), -1, 1, 1);
}

MUSTINLINE uint8_t *_GetMinImageLineUnsafe(
    const MinImg *p_image,
    int           y,
    BorderOption  border   = BO_VOID,
    void         *p_canvas = NULL) {
  const int ht = p_image->height;
  if (y < 0 || y >= ht)
    switch (border) {
      case BO_REPEAT: {
        y = std::min(std::max(0, y), ht - 1);
        break;
      }
      case BO_CYCLIC: {
        y = (y % ht + ht) % ht;
        break;
      }
      case BO_SYMMETRIC: {
        int ht2 = ht * 2;
        y = (y % ht2 + ht2) % ht2;
        y = std::min(y, ht2 - 1 - y);
        break;
      }
      case BO_CONSTANT: {
        return reinterpret_cast<uint8_t *>(p_canvas);
      }
      case BO_VOID: {
        return NULL;
      }
      default: {
      }
    }

  return p_image->pScan0 + static_cast<ptrdiff_t>(p_image->stride) * y;
}

MUSTINLINE uint8_t *_GetMinImageLine(
    const MinImg *p_image,
    int           y,
    BorderOption  border   = BO_VOID,
    void         *p_canvas = NULL) {
  if (_AssureMinImageIsValid(p_image) != NO_ERRORS)
    return NULL;

  if (_AssureMinImageIsEmpty(p_image) == NO_ERRORS)
    switch (border) {
    case BO_CONSTANT:
      return reinterpret_cast<uint8_t *>(p_canvas);
    case BO_IGNORE:
      return p_image->pScan0 ? p_image->pScan0 + static_cast<ptrdiff_t>(p_image->stride) * y : NULL;
    default:
      return NULL;
    }
  return _GetMinImageLineUnsafe(p_image, y, border, p_canvas);
}

MUSTINLINE int _CloneMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    AllocationOption allocation = AO_PREALLOCATED) {
  if (!p_dst_image || !p_src_image || p_dst_image->pScan0)
    return BAD_ARGS;

  if (p_dst_image != p_src_image) {
    *p_dst_image = *p_src_image;
    p_dst_image->stride = 0;
    p_dst_image->pScan0 = 0;
  }
  if (allocation == AO_PREALLOCATED)
    PROPAGATE_ERROR(AllocMinImage(p_dst_image));

  return NO_ERRORS;
}

MUSTINLINE int _CloneResizedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    int              width,
    int              height,
    AllocationOption allocation = AO_PREALLOCATED) {
  if (width < 0 || height < 0)
    return BAD_ARGS;

  PROPAGATE_ERROR(_CloneMinImagePrototype(p_dst_image, p_src_image, AO_EMPTY));
  p_dst_image->width = width;
  p_dst_image->height = height;
  if (allocation == AO_PREALLOCATED)
    PROPAGATE_ERROR(AllocMinImage(p_dst_image));

  return NO_ERRORS;
}

MUSTINLINE int _CloneTransposedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    AllocationOption allocation = AO_PREALLOCATED) {
  PROPAGATE_ERROR(_CloneResizedMinImagePrototype(p_dst_image, p_src_image,
                          p_src_image->height, p_src_image->width, allocation));

  return NO_ERRORS;
}

MUSTINLINE int _CloneRetypifiedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    MinTyp           type,
    AllocationOption allocation = AO_PREALLOCATED) {
  PROPAGATE_ERROR(_CloneMinImagePrototype(p_dst_image, p_src_image, AO_EMPTY));
  PROPAGATE_ERROR(_SetMinImageType(p_dst_image, type));
  if (allocation == AO_PREALLOCATED)
    PROPAGATE_ERROR(AllocMinImage(p_dst_image));

  return NO_ERRORS;
}

MUSTINLINE int _CloneDimensionedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    int              channels,
    AllocationOption allocation = AO_PREALLOCATED) {
  if (channels < 0)
    return BAD_ARGS;

  PROPAGATE_ERROR(_CloneMinImagePrototype(p_dst_image, p_src_image, AO_EMPTY));
  p_dst_image->channels = channels;
  if (allocation == AO_PREALLOCATED)
    PROPAGATE_ERROR(AllocMinImage(p_dst_image));

  return NO_ERRORS;
}

MUSTINLINE int _CompareMinImagePrototypes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  MinImg prototype_a = {0};
  PROPAGATE_ERROR(_CloneMinImagePrototype(&prototype_a, p_image_a, AO_EMPTY));
  MinImg prototype_b = {0};
  PROPAGATE_ERROR(_CloneMinImagePrototype(&prototype_b, p_image_b, AO_EMPTY));
  return ::memcmp(&prototype_a, &prototype_b, sizeof(prototype_a)) != 0;
}

MUSTINLINE int _CompareMinImage2DSizes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  if (!p_image_a || !p_image_b)
    return BAD_ARGS;

  return p_image_a->width != p_image_b->width ||
         p_image_a->height != p_image_b->height;
}

MUSTINLINE int _CompareMinImage3DSizes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  if (!p_image_a || !p_image_b)
    return BAD_ARGS;

  return p_image_a->width != p_image_b->width ||
         p_image_a->height != p_image_b->height ||
         p_image_a->channels != p_image_b->channels;
}

MUSTINLINE int _CompareMinImageTypes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  if (!p_image_a || !p_image_b)
    return BAD_ARGS;

  return p_image_a->channelDepth != p_image_b->channelDepth ||
         p_image_a->format       != p_image_b->format;
}

MUSTINLINE int _CompareMinImagePixels(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  if (!p_image_a || !p_image_b)
    return BAD_ARGS;

  return _CompareMinImageTypes(p_image_a, p_image_b) ||
         p_image_a->channels != p_image_b->channels;
}

MUSTINLINE int _CompareMinImages(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_image_a));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_image_b));
  int proto_res = _CompareMinImagePrototypes(p_image_a, p_image_b);
  if (proto_res)
    return proto_res;
  int height = p_image_a->height;
  int byte_line_width = _GetMinImageBytesPerLine(p_image_a);
  const uint8_t* p_line_a = _GetMinImageLine(p_image_a, 0);
  const uint8_t* p_line_b = _GetMinImageLine(p_image_b, 0);
  for (int y = 0; y < height; ++y) {
    if (::memcmp(p_line_a, p_line_b, byte_line_width))
      return 1;
    p_line_a += p_image_a->stride;
    p_line_b += p_image_b->stride;
  }
  return NO_ERRORS;
}

MUSTINLINE int _WrapAlignedBufferWithMinImage(
    MinImg      *p_image,
    void        *p_buffer,
    int          width,
    int          height,
    int          channels,
    MinTyp       element_type,
    int          stride,
    RulesOption  rules = RO_STRICT) {
  if (!p_image || !p_buffer)
    return BAD_ARGS;
  if (width < 0 || height < 0 || channels < 0)
    return BAD_ARGS;
  if (!(rules & RO_REUSE_CONTAINER) && p_image->pScan0)
    return BAD_ARGS;

  // TODO: Add check of std::abs(stride);
  // TODO: Add check of stride being >= width * channels (by abs value);

  ::memset(p_image, 0, sizeof(*p_image));
  PROPAGATE_ERROR(_SetMinImageType(p_image, element_type));
  p_image->height = height;
  p_image->width = width;
  p_image->channels = channels;
  p_image->stride = stride ? stride : _GetMinImageBytesPerLine(p_image);
  p_image->pScan0 = reinterpret_cast<uint8_t *>(p_buffer);

  return NO_ERRORS;
}

MUSTINLINE int _WrapSolidBufferWithMinImage(
    MinImg      *p_image,
    void        *p_buffer,
    int          width,
    int          height,
    int          channels,
    MinTyp       element_type,
    RulesOption  rules = RO_STRICT) {
  return _WrapAlignedBufferWithMinImage(p_image, p_buffer, width, height,
                                        channels, element_type, 0, rules);
}

MUSTINLINE int _WrapScalarWithMinImage(
    MinImg      *p_image,
    void        *p_scalar,
    MinTyp       element_type,
    RulesOption  rules = RO_STRICT) {
  return _WrapSolidBufferWithMinImage(p_image, p_scalar, 1, 1, 1,
                                      element_type, rules);
}

MUSTINLINE int _WrapPixelWithMinImage(
    MinImg      *p_image,
    void        *p_pixel,
    int          channels,
    MinTyp       element_type,
    RulesOption  rules = RO_STRICT) {
  return _WrapSolidBufferWithMinImage(p_image, p_pixel, 1, 1,
                                      channels, element_type, rules);
}

MUSTINLINE int _WrapScalarVectorWithMinImage(
    MinImg          *p_image,
    void            *p_vector,
    int              size,
    DirectionOption  direction,
    MinTyp           element_type,
    RulesOption  rules = RO_STRICT) {
  if (direction == DO_BOTH)
    return BAD_ARGS;
  return _WrapSolidBufferWithMinImage(p_image, p_vector,
                                      direction == DO_VERTICAL ? 1 : size,
                                      direction == DO_VERTICAL ? size : 1,
                                      1, element_type, rules);
}

MUSTINLINE int _WrapPixelVectorWithMinImage(
    MinImg          *p_image,
    void            *p_vector,
    int              size,
    DirectionOption  direction,
    int              channels,
    MinTyp           element_type,
    RulesOption  rules = RO_STRICT) {
  if (direction == DO_BOTH)
    return BAD_ARGS;
  return _WrapSolidBufferWithMinImage(p_image, p_vector,
                                      direction == DO_VERTICAL ? 1 : size,
                                      direction == DO_VERTICAL ? size : 1,
                                      channels, element_type, rules);
}

MUSTINLINE int _GetMinImageRegion(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    int           x0,
    int           y0,
    int           width,
    int           height,
    RulesOption   rules = RO_STRICT) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  if (!p_dst_image || width < 0 || height < 0)
    return BAD_ARGS;
  if (!(rules & RO_REUSE_CONTAINER) && p_dst_image->pScan0)
    return BAD_ARGS;
  if (!(rules & RO_IGNORE_BORDERS)) {
    if (x0 < 0 || static_cast<uint32_t>(x0) + static_cast<uint32_t>(width) >
        static_cast<uint32_t>(p_src_image->width))
      return BAD_ARGS;
    if (y0 < 0 || static_cast<uint32_t>(y0) + static_cast<uint32_t>(height) >
        static_cast<uint32_t>(p_src_image->height))
      return BAD_ARGS;
  }

  *p_dst_image = *p_src_image;
  p_dst_image->width = width;
  p_dst_image->height = height;

  int bit_shift = x0 * _GetMinImageBitsPerPixel(p_dst_image);
  if (bit_shift & 0x07U)
    return BAD_ARGS;
  p_dst_image->pScan0 = _GetMinImageLine(p_src_image, y0, BO_IGNORE) +
                        (bit_shift >> 3);
  if (!p_dst_image->pScan0)
    return INTERNAL_ERROR;
  p_dst_image->stride = p_src_image->stride;

  return NO_ERRORS;
}

MUSTINLINE int _FlipMinImageVertically(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules = RO_STRICT) {
  if (!p_dst_image)
    return BAD_ARGS;
  if (!(rules & RO_REUSE_CONTAINER) && p_dst_image->pScan0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));

  *p_dst_image = *p_src_image;
  uint8_t *p = _GetMinImageLine(p_dst_image, p_dst_image->height - 1);
  if (!p)
    return INTERNAL_ERROR;
  p_dst_image->pScan0 = p;
  p_dst_image->stride *= -1;

  return NO_ERRORS;
}

MUSTINLINE int _UnfoldMinImageChannels(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules = RO_STRICT) {
  if (!p_dst_image)
    return BAD_ARGS;
  if (!(rules & RO_REUSE_CONTAINER) && p_dst_image->pScan0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));

  *p_dst_image = *p_src_image;
  p_dst_image->width *= p_dst_image->channels;
  p_dst_image->channels = 1;

  return NO_ERRORS;
}

MUSTINLINE int _SliceMinImageVertically(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    int           begin,
    int           period,
    int           end = -1,
    RulesOption   rules = RO_STRICT) {
  if (!p_dst_image || p_dst_image->pScan0)
    return BAD_ARGS;
  if (!(rules & RO_REUSE_CONTAINER) && p_dst_image->pScan0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  if (end < 0)
    end = p_src_image->height;
  if (begin < 0 || end < begin || p_src_image->height < end || period <= 0)
    return BAD_ARGS;


  *p_dst_image = *p_src_image;
  p_dst_image->pScan0 = _GetMinImageLine(p_dst_image, begin);
  if (!p_dst_image->pScan0)
    return INTERNAL_ERROR;
  p_dst_image->stride *= period;
  p_dst_image->height = (end - begin + period - 1) / period;

  return NO_ERRORS;
}

MUSTINLINE int _UnrollSolidMinImage(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules = RO_STRICT) {
  if (!p_dst_image)
    return BAD_ARGS;
  if (!(rules & RO_REUSE_CONTAINER) && p_dst_image->pScan0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  if (_AssureMinImageIsSolid(p_src_image) != NO_ERRORS)
    return BAD_ARGS;

  *p_dst_image = *p_src_image;
  p_dst_image->width *= p_dst_image->height;
  p_dst_image->height = 1;

  return NO_ERRORS;
}

#endif // #ifndef MINIMGAPI_MINIMGAPI_INL_H_INCLUDED
