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

#include <cstring>
#include <cstdlib>
#include <algorithm>

#include <minimgapi/minimgapi-inl.h>
#include <minbase/minresult.h>
#include <minutils/smartptr.h>
#include <minbase/crossplat.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/imgguard.hpp>


MINIMGAPI_API int NewMinImagePrototype(
    MinImg          *p_image,
    int              width,
    int              height,
    int              channels,
    MinTyp           element_type,
    int              address_space,
    AllocationOption allocation) {
  if (!p_image || p_image->pScan0)
    return BAD_ARGS;

  ::memset(p_image, 0, sizeof(*p_image));
  p_image->width = width;
  p_image->height = height;
  p_image->channels = channels;
  p_image->addressSpace = address_space;
  PROPAGATE_ERROR(_SetMinImageType(p_image, element_type));

  if (allocation == AO_PREALLOCATED)
    PROPAGATE_ERROR(AllocMinImage(p_image));

  return NO_ERRORS;
}

MINIMGAPI_API int AllocMinImage(
    MinImg *p_image,
    int     alignment) {
  PROPAGATE_ERROR(_AssureMinImagePrototypeIsValid(p_image));
  if (p_image->pScan0)
    return BAD_ARGS;
  if (alignment <= 0 || alignment & (alignment - 1))
    return BAD_ARGS;
  if (std::abs(p_image->stride) % alignment)
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_image) == NO_ERRORS)
    return NO_ERRORS;
  if (p_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  const int line_size = _GetMinImageBytesPerLine(p_image);
  if (line_size <= 0)
    return INTERNAL_ERROR;

  if (p_image->stride && std::abs(p_image->stride) < line_size)
    return BAD_ARGS;

  if (!p_image->stride)
    p_image->stride = (line_size + alignment - 1) & ~(alignment - 1);
  const int abs_stride = std::abs(p_image->stride);

  uint8_t *p_buffer = reinterpret_cast<uint8_t *>(
                      ::alignedmalloc(p_image->height * abs_stride, alignment));
  if (!p_buffer)
    return NO_MEMORY;

  p_image->pScan0 = p_buffer;
  if (p_image->stride < 0)
    p_image->pScan0 += (p_image->height - 1) * abs_stride;

  return NO_ERRORS;
}

MINIMGAPI_API int FreeMinImage(
    MinImg *p_image) {
  if (!p_image)
    return BAD_ARGS;
  if (!p_image->pScan0) {
    ::memset(p_image, 0, sizeof(*p_image));
    return NO_ERRORS;
  }
  if (p_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  uint8_t *const p_buffer = p_image->stride > 0 ? p_image->pScan0 :
                                 _GetMinImageLine(p_image, p_image->height - 1);
  if (!p_buffer)
    return INTERNAL_ERROR;
  ::alignedfree(p_buffer);
  ::memset(p_image, 0, sizeof(*p_image));

  return NO_ERRORS;
}

MINIMGAPI_API int CloneMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    AllocationOption allocation) {
  return _CloneMinImagePrototype(p_dst_image, p_src_image, allocation);
}

MINIMGAPI_API int CloneTransposedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    AllocationOption allocation) {
  return _CloneTransposedMinImagePrototype(p_dst_image, p_src_image,
                                           allocation);
}

MINIMGAPI_API int CloneRetypifiedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    MinTyp           type,
    AllocationOption allocation) {
  return _CloneRetypifiedMinImagePrototype(p_dst_image, p_src_image,
                                           type, allocation);
}

MINIMGAPI_API int CloneDimensionedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    int              channels,
    AllocationOption allocation) {
  return _CloneDimensionedMinImagePrototype(p_dst_image, p_src_image,
                                            channels, allocation);
}

MINIMGAPI_API int CloneResizedMinImagePrototype(
    MinImg          *p_dst_image,
    const MinImg    *p_src_image,
    int              width,
    int              height,
    AllocationOption allocation) {
  return _CloneResizedMinImagePrototype(p_dst_image, p_src_image,
                                        width, height, allocation);
}

MINIMGAPI_API int WrapScalarWithMinImage(
    MinImg      *p_image,
    void        *p_scalar,
    MinTyp       element_type,
    RulesOption  rules) {
  return _WrapScalarWithMinImage(p_image, p_scalar, element_type, rules);
}

MINIMGAPI_API int WrapPixelWithMinImage(
    MinImg      *p_image,
    void        *p_pixel,
    int          channels,
    MinTyp       element_type,
    RulesOption  rules) {
  return _WrapPixelWithMinImage(p_image, p_pixel, channels,
                                element_type, rules);
}

MINIMGAPI_API int WrapScalarVectorWithMinImage(
    MinImg          *p_image,
    void            *p_vector,
    int              size,
    DirectionOption  direction,
    MinTyp           element_type,
    RulesOption      rules) {
  return _WrapScalarVectorWithMinImage(p_image, p_vector, size,
                                       direction, element_type, rules);
}

MINIMGAPI_API int WrapPixelVectorWithMinImage(
    MinImg          *p_image,
    void            *p_vector,
    int              size,
    DirectionOption  direction,
    int              channels,
    MinTyp           element_type,
    RulesOption      rules) {
  return _WrapPixelVectorWithMinImage(p_image, p_vector, size,
                                      direction, channels, element_type, rules);
}

MINIMGAPI_API int WrapSolidBufferWithMinImage(
    MinImg      *p_image,
    void        *p_buffer,
    int          width,
    int          height,
    int          channels,
    MinTyp       element_type,
    RulesOption  rules) {
  return _WrapSolidBufferWithMinImage(p_image, p_buffer,
                                      width, height, channels,
                                      element_type, rules);
}

MINIMGAPI_API int WrapAlignedBufferWithMinImage(
    MinImg      *p_image,
    void        *p_buffer,
    int          width,
    int          height,
    int          channels,
    MinTyp       element_type,
    int          stride,
    RulesOption  rules) {
  return _WrapAlignedBufferWithMinImage(p_image, p_buffer,
                                      width, height, channels,
                                      element_type, stride, rules);
}

MINIMGAPI_API int GetMinImageRegion(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    int           x0,
    int           y0,
    int           width,
    int           height,
    RulesOption   rules) {
  return _GetMinImageRegion(p_dst_image, p_src_image,
                            x0, y0, width, height, rules);
}

MINIMGAPI_API int FlipMinImageVertically(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules) {
  return _FlipMinImageVertically(p_dst_image, p_src_image, rules);
}

MINIMGAPI_API int UnfoldMinImageChannels(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules) {
  return _UnfoldMinImageChannels(p_dst_image, p_src_image, rules);
}

MINIMGAPI_API int SliceMinImageVertically(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    int           begin,
    int           period,
    int           end,
    RulesOption   rules) {
  return _SliceMinImageVertically(p_dst_image, p_src_image, begin, period, end,
                                  rules);
}

MINIMGAPI_API int UnrollSolidMinImage(
    MinImg       *p_dst_image,
    const MinImg *p_src_image,
    RulesOption   rules) {
  return _UnrollSolidMinImage(p_dst_image, p_src_image, rules);
}

MINIMGAPI_API int GetFmtByTyp(
    MinTyp typ) {
  return _GetFmtByTyp(typ);
}

MINIMGAPI_API int GetDepthByTyp(
    MinTyp typ) {
  return _GetDepthByTyp(typ);
}

MINIMGAPI_API int GetTypByFmtAndDepth(
    MinFmt fmt,
    int    depth) {
  return _GetTypByFmtAndDepth(fmt, depth);
}

MINIMGAPI_API int GetMinImageType(
    const MinImg *p_image) {
  return _GetMinImageType(p_image);
}

MINIMGAPI_API int SetMinImageType(
    MinImg *p_image,
    MinTyp  element_type) {
  return _SetMinImageType(p_image, element_type);
}

MINIMGAPI_API int GetMinImageBitsPerPixel(
    const MinImg *p_image) {
  return _GetMinImageBitsPerPixel(p_image);
}

MINIMGAPI_API int GetMinImageBytesPerLine(
    const MinImg *p_image) {
  return _GetMinImageBytesPerLine(p_image);
}

MINIMGAPI_API int AssureMinImagePrototypeIsValid(
    const MinImg *p_image) {
  return _AssureMinImagePrototypeIsValid(p_image);
}

MINIMGAPI_API int AssureMinImageIsValid(
    const MinImg *p_image) {
  return _AssureMinImageIsValid(p_image);
}

MINIMGAPI_API int AssureMinImageIsEmpty(
    const MinImg *p_image) {
  return _AssureMinImageIsEmpty(p_image);
}

MINIMGAPI_API int AssureMinImageIsSolid(
    const MinImg *p_image) {
  return _AssureMinImageIsSolid(p_image);
}

MINIMGAPI_API int AssureMinImageIsScalar(
    const MinImg *p_image) {
  return _AssureMinImageIsScalar(p_image);
}

MINIMGAPI_API int AssureMinImageIsPixel(
    const MinImg *p_image) {
  return _AssureMinImageIsPixel(p_image);
}

MINIMGAPI_API int AssureMinImageFits(
    const MinImg *p_image,
    MinTyp        element_type,
    int           channels,
    int           width,
    int           height) {
  return _AssureMinImageFits(p_image, element_type, channels, width, height);
}

MINIMGAPI_API uint8_t *GetMinImageLine(
    const MinImg *p_image,
    int           y,
    BorderOption  border,
    void         *p_canvas) {
  return _GetMinImageLine(p_image, y, border, p_canvas);
}

MINIMGAPI_API int CompareMinImagePrototypes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  return _CompareMinImagePrototypes(p_image_a, p_image_b);
}

MINIMGAPI_API int CompareMinImage2DSizes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  return _CompareMinImage2DSizes(p_image_a, p_image_b);
}

MINIMGAPI_API int CompareMinImage3DSizes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  return _CompareMinImage3DSizes(p_image_a, p_image_b);
}

MINIMGAPI_API int CompareMinImagePixels(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  return _CompareMinImagePixels(p_image_a, p_image_b);
}

MINIMGAPI_API int CompareMinImageTypes(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  return _CompareMinImageTypes(p_image_a, p_image_b);
}

MINIMGAPI_API int CompareMinImages(
    const MinImg *p_image_a,
    const MinImg *p_image_b) {
  return _CompareMinImages(p_image_a, p_image_b);
}

MINIMGAPI_API int CheckMinImagesTangle(
    uint32_t     *p_result,
    const MinImg *p_dst_image,
    const MinImg *p_src_image) {
  if (!p_result)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));

  if (p_dst_image->pScan0 == p_src_image->pScan0 &&
    p_dst_image->stride == p_src_image->stride) {
    *p_result = TCR_SAME_IMAGE;
    return NO_ERRORS;
  }

  *p_result = TCR_TANGLED_IMAGES;

  uint8_t *p_dst_begin = std::min(p_dst_image->pScan0,
                        _GetMinImageLine(p_dst_image, p_dst_image->height - 1));
  uint8_t *p_dst_end = std::max(p_dst_image->pScan0,
                       _GetMinImageLine(p_dst_image, p_dst_image->height - 1)) +
                        _GetMinImageBytesPerLine(p_dst_image);
  uint8_t *p_src_begin = std::min(p_src_image->pScan0,
                        _GetMinImageLine(p_src_image, p_src_image->height - 1));
  uint8_t *p_src_end = std::max(p_src_image->pScan0,
                       _GetMinImageLine(p_src_image, p_src_image->height - 1)) +
                        _GetMinImageBytesPerLine(p_src_image);
  if (!p_dst_begin || !p_dst_end || !p_src_begin || !p_src_end)
    return INTERNAL_ERROR;
  if (p_dst_begin >= p_src_end || p_src_begin >= p_dst_end) {
    *p_result = TCR_INDEPENDENT_IMAGES;
    return NO_ERRORS;
  }

  if (std::abs(p_dst_image->stride) == std::abs(p_src_image->stride)) {
    intptr_t dst_begin_phase = (p_dst_image->pScan0 - p_src_image->pScan0) %
                               std::abs(p_src_image->stride);
    if (dst_begin_phase >= _GetMinImageBytesPerLine(p_src_image) &&
      dst_begin_phase + _GetMinImageBytesPerLine(p_dst_image) <=
      std::abs(p_src_image->stride)) {
      *p_result = TCR_INDEPENDENT_IMAGES;
      return NO_ERRORS;
    }
  }

  if (p_src_image->stride >= 0 &&
      p_src_image->pScan0 >= p_dst_image->pScan0 &&
      p_src_image->stride >= p_dst_image->stride) {
    *p_result |= TCR_FORWARD_PASS_POSSIBLE;
    if (p_src_image->pScan0 >=
        p_dst_image->pScan0 + _GetMinImageBytesPerLine(p_dst_image))
    *p_result |= TCR_INDEPENDENT_LINES;
  }

  return NO_ERRORS;
}

MINIMGAPI_API int ZeroFillMinImage(
    const MinImg *p_image) {
  uint8_t zero = 0;
  return FillMinImage(p_image, &zero, 1);
}

MINIMGAPI_API int FillMinImage(
    const MinImg *p_image,
    const void   *p_canvas,
    int           value_size) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_image));
  if (!p_canvas || value_size < 0)
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_image) == NO_ERRORS)
    return NO_ERRORS;
  if (p_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  int bits_per_pixel = _GetMinImageBitsPerPixel(p_image);
  int line_bit_width = bits_per_pixel * p_image->width;
  int bit_tail_width = line_bit_width & 0x07U;
  int line_byte_width = line_bit_width >> 3;
  uint8_t tail_mask = ~(0xFFU >> bit_tail_width);
  const uint8_t *p_canvas_bytes = reinterpret_cast<const uint8_t *>(p_canvas);
  if (!value_size && !(bits_per_pixel & 0x07U))
    value_size = bits_per_pixel >> 3;
  DECLARE_GUARDED_MINIMG(buffer_line);

  uint8_t *p_buffer = p_image->pScan0;
  int aligned_size = line_byte_width;

  size_t scan0_address = reinterpret_cast<size_t>(static_cast<const void *>(
      p_image->pScan0));

  switch (value_size) {
    case 0: {
      PROPAGATE_ERROR(_CloneResizedMinImagePrototype(&buffer_line, p_image,
                                                     p_image->width, 1));
      p_buffer = buffer_line.pScan0;
      ::memset(p_buffer, 0, line_byte_width);
      if (tail_mask)
        p_buffer[line_byte_width] &= ~tail_mask;
      for (int x = 0; x < line_bit_width; ++x) {
        int i = x % bits_per_pixel;
        if (GET_IMAGE_LINE_BIT(p_canvas_bytes, i))
          SET_IMAGE_LINE_BIT(p_buffer, x);
      }
      break;
    }
    case 1: {
      ::memset(p_image->pScan0, *p_canvas_bytes, line_byte_width);
      break;
    }
    case 2: {
      const int image_misaligned = scan0_address & 0x01U;
      uint16_t value = 0;
      ::memcpy(&value, p_canvas, 2);
      if (image_misaligned) {
        p_image->pScan0[0] = *reinterpret_cast<const uint8_t *>(&value);
        value = value << 1 | value >> 1;
      }
      uint16_t *p_line = reinterpret_cast<uint16_t *>(p_image->pScan0 +
                                                      image_misaligned);
      const int line_width_from_alignment = line_byte_width - image_misaligned;
      const int typed_line_width = line_width_from_alignment >> 1;
      for (int x = 0; x < typed_line_width; ++x)
        p_line[x] = value;
      if (line_width_from_alignment & 0x01U)
        p_image->pScan0[line_byte_width - 1] = p_image->pScan0[0];
      break;
    }
    case 4: {
      const int shift_from_alignment = scan0_address & 0x03U;
      const int to_next_alignment = (4 - shift_from_alignment) & 0x03U;
      uint32_t value = 0;
      ::memcpy(&value, p_canvas, 4);
      ::memcpy(p_image->pScan0, &value, to_next_alignment);
      value = value << shift_from_alignment | value >> to_next_alignment;
      uint32_t *p_line = reinterpret_cast<uint32_t *>(p_image->pScan0 +
                                                      to_next_alignment);
      const int line_width_from_alignment = line_byte_width - to_next_alignment;
      const int typed_line_width = line_width_from_alignment >> 2;
      for (int x = 0; x < typed_line_width; ++x)
        p_line[x] = value;
      ::memcpy(p_line + typed_line_width, &value,
               line_width_from_alignment & 0x03U);
      break;
    }
    case 8: {
      const int shift_from_alignment = scan0_address & 0x07U;
      const int to_next_alignment = (8 - shift_from_alignment) & 0x07U;
      uint64_t value = 0;
      ::memcpy(&value, p_canvas, 8);
      ::memcpy(p_image->pScan0, &value, to_next_alignment);
      value = value << shift_from_alignment | value >> to_next_alignment;
      uint64_t *p_line = reinterpret_cast<uint64_t *>(p_image->pScan0 +
                                                      to_next_alignment);
      const int line_width_from_alignment = line_byte_width - to_next_alignment;
      const int typed_line_width = line_width_from_alignment >> 3;
      for (int x = 0; x < typed_line_width; ++x)
        p_line[x] = value;
      ::memcpy(p_line + typed_line_width, &value,
               line_width_from_alignment & 0x07U);
      break;
    }
    default: {
      if (p_canvas_bytes + value_size > p_image->pScan0 &&
          p_canvas_bytes < p_image->pScan0 + line_byte_width) {
        PROPAGATE_ERROR(_CloneResizedMinImagePrototype(&buffer_line, p_image,
                                                       p_image->width, 1));
        p_buffer = buffer_line.pScan0;
      }
      aligned_size = line_byte_width - line_byte_width % value_size;
      for (int x = 0; x < aligned_size; x += value_size)
        ::memcpy(p_buffer + x, p_canvas_bytes, value_size);
      ::memcpy(p_buffer + aligned_size, p_canvas_bytes,
               line_byte_width - aligned_size);
    }
  }

  if (value_size && tail_mask) {
    p_buffer[line_byte_width] &= ~tail_mask;
    p_buffer[line_byte_width] |=
        p_canvas_bytes[line_byte_width - aligned_size] & tail_mask;
  }

  MinImg first_line = {0};
  PROPAGATE_ERROR(_GetMinImageRegion(&first_line, p_image, 0, 0,
                                     p_image->width, 1));
  if (buffer_line.pScan0)
    PROPAGATE_ERROR(CopyMinImage(&first_line, &buffer_line));
  MinImg current_line = first_line;
  for (int y = 1; y < p_image->height; ++y) {
    current_line.pScan0 += current_line.stride;
    ::memcpy(current_line.pScan0, first_line.pScan0, line_byte_width);
    if (tail_mask) {
      current_line.pScan0[line_byte_width] &= ~tail_mask;
      current_line.pScan0[line_byte_width] |=
          first_line.pScan0[line_byte_width] & tail_mask;
    }
  }

  return NO_ERRORS;
}

MINIMGAPI_API int CopyMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  if (_CompareMinImagePrototypes(p_dst_image, p_src_image))
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_dst_image) == NO_ERRORS)
    return NO_ERRORS;

  uint32_t tangling = 0;
  PROPAGATE_ERROR(CheckMinImagesTangle(&tangling, p_dst_image, p_src_image));
  if (tangling == TCR_SAME_IMAGE)
    return NO_ERRORS;

  const MinImg *p_work_dst_image = p_dst_image;
  const MinImg *p_work_src_image = p_src_image;

  MinImg dst_image = {0};
  MinImg src_image = {0};
  DECLARE_GUARDED_MINIMG(tmp_image);
  if (~tangling & TCR_FORWARD_PASS_POSSIBLE &&
      tangling & TCR_BACKWARD_PASS_POSSIBLE) {
    SHOULD_WORK(_FlipMinImageVertically(&dst_image, p_dst_image));
    SHOULD_WORK(_FlipMinImageVertically(&src_image, p_src_image));
    p_work_dst_image = &dst_image;
    p_work_src_image = &src_image;
  } else if (tangling == TCR_TANGLED_IMAGES) {
    PROPAGATE_ERROR(_CloneMinImagePrototype(&tmp_image, p_src_image));
    SHOULD_WORK(CopyMinImage(&tmp_image, p_src_image));
    p_work_src_image = &tmp_image;
  } else if (~tangling & TCR_FORWARD_PASS_POSSIBLE)
    return INTERNAL_ERROR;

  if (p_work_dst_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  int bit_line_width = p_work_dst_image->width *
                      _GetMinImageBitsPerPixel(p_work_dst_image);
  int byte_line_width = bit_line_width >> 3;
  int bits_tail_width = bit_line_width & 0x07U;
  uint8_t bit_mask = 0xFFU << (8 - bits_tail_width);
  uint8_t src_bits = 0;

  if (_AssureMinImageIsSolid(p_work_src_image) == NO_ERRORS &&
      _AssureMinImageIsSolid(p_work_dst_image) == NO_ERRORS) {
    ::memmove(p_work_dst_image->pScan0, p_work_src_image->pScan0,
              p_work_dst_image->height * byte_line_width);
    if (bit_mask) {
      p_work_dst_image->pScan0[byte_line_width] &= ~bit_mask;
      p_work_dst_image->pScan0[byte_line_width] |=
                           p_work_src_image->pScan0[byte_line_width] & bit_mask;
    }

    return NO_ERRORS;
  }

  uint8_t *p_dst_line = _GetMinImageLine(p_work_dst_image, 0);
  const uint8_t *p_src_line = _GetMinImageLine(p_work_src_image, 0);
  if (!p_dst_line || !p_src_line)
    return INTERNAL_ERROR;
  for (int y = 0; y < p_work_dst_image->height; ++y) {
    if (bit_mask)
      src_bits = p_src_line[byte_line_width] & bit_mask;
    if (tangling & TCR_INDEPENDENT_LINES)
      ::memcpy(p_dst_line, p_src_line, byte_line_width);
    else
      ::memmove(p_dst_line, p_src_line, byte_line_width);
    if (bit_mask)
      p_dst_line[byte_line_width] = src_bits |
                                    (p_dst_line[byte_line_width] & ~bit_mask);
    p_dst_line += p_work_dst_image->stride;
    p_src_line += p_work_src_image->stride;
  }

  return NO_ERRORS;
}

MINIMGAPI_API int CopyMinImageFragment(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    int           dst_x0,
    int           dst_y0,
    int           src_x0,
    int           src_y0,
    int           width,
    int           height) {
  if (_CompareMinImagePixels(p_dst_image, p_src_image))
    return BAD_ARGS;
  if (dst_x0 < 0 || dst_x0 + width > p_dst_image->width   ||
      dst_y0 < 0 || dst_y0 + height > p_dst_image->height ||
      src_x0 < 0 || src_x0 + width > p_src_image->width   ||
      src_y0 < 0 || src_y0 + height > p_src_image->height)
    return BAD_ARGS;
  if (!width || !height || !p_dst_image->channels)
    return NO_ERRORS;

  MinImg unfolded_dst_image = {0};
  PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_dst_image, p_dst_image));
  MinImg unfolded_src_image = {0};
  PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_src_image, p_src_image));
  dst_x0 *= p_dst_image->channels;
  src_x0 *= p_dst_image->channels;
  width  *= p_dst_image->channels;

  MinImg *p_dst_region = &unfolded_dst_image;
  MinImg *p_src_region = &unfolded_src_image;

  int bits_per_pixel = _GetMinImageBitsPerPixel(p_dst_region);
  int dst_bits_from_left = dst_x0 * bits_per_pixel;
  int dst_byte_shift = dst_bits_from_left >> 3;
  int dst_bit_shift = dst_bits_from_left & 0x07U;
  int dst_fragment_width = dst_bit_shift + width;
  int src_bits_from_left = src_x0 * bits_per_pixel;
  int src_byte_shift = src_bits_from_left >> 3;
  int src_bit_shift = src_bits_from_left & 0x07U;
  int src_fragment_width = src_bit_shift + width;
  RulesOption rules = static_cast<RulesOption>(RO_IGNORE_BORDERS |
                                               RO_REUSE_CONTAINER);

  PROPAGATE_ERROR(_GetMinImageRegion(p_dst_region, p_dst_region,
                                     (dst_byte_shift << 3) / bits_per_pixel,
                                     dst_y0,
                                     dst_fragment_width, height, rules));
  PROPAGATE_ERROR(_GetMinImageRegion(p_src_region, p_src_region,
                                     (src_byte_shift << 3) / bits_per_pixel,
                                     src_y0,
                                     src_fragment_width, height, rules));

  if (!dst_bit_shift && !src_bit_shift)
    return CopyMinImage(p_dst_region, p_src_region);

  uint32_t tangling = 0;
  PROPAGATE_ERROR(CheckMinImagesTangle(&tangling, p_dst_region, p_src_region));
  MinImg src_image = {0};
  MinImg dst_image = {0};
  DECLARE_GUARDED_MINIMG(tmp_image);
  if (~tangling & TCR_INDEPENDENT_LINES &&
      ~tangling & TCR_FORWARD_PASS_POSSIBLE) {
    if (tangling & TCR_BACKWARD_PASS_POSSIBLE) {
      SHOULD_WORK(_FlipMinImageVertically(&dst_image, p_dst_region));
      SHOULD_WORK(_FlipMinImageVertically(&src_image, p_src_region));
      p_dst_region = &dst_image;
      p_src_region = &src_image;
    } else {
      PROPAGATE_ERROR(_CloneMinImagePrototype(&tmp_image, p_src_region));
      SHOULD_WORK(CopyMinImage(&tmp_image, p_src_region));
      p_src_region = &tmp_image;
    }
  }

  int bit_shift = src_bit_shift - dst_bit_shift;
  int l_shift = std::max(0, bit_shift);
  int r_shift = std::max(0, -bit_shift);
  int back_8_shift = 8 - bit_shift;

  if (dst_fragment_width < 8) {
    uint8_t frontail_bit_mask = ((1 << width) - 1) << (8 - dst_fragment_width);
    uint8_t *p_dst = _GetMinImageLine(p_dst_region, 0);
    uint8_t *p_src = _GetMinImageLine(p_src_region, 0);
    if (!p_dst || !p_src)
      return INTERNAL_ERROR;
    for (int y = 0; y < height; ++y) {
      p_dst[0] = (p_dst[0] & ~frontail_bit_mask) |
                 (((p_src[0] << l_shift) >> r_shift) & frontail_bit_mask);
      if (src_fragment_width > 8)
        p_dst[0] |= p_src[1] >> back_8_shift & frontail_bit_mask;
      p_dst += p_dst_region->stride;
      p_src += p_src_region->stride;
    }

    return NO_ERRORS;
  }

  int dst_aligned_width = dst_fragment_width - 8 * !!dst_bit_shift;
  int dst_8_width = dst_aligned_width >> 3;
  int dst_64_width = std::max(0, (dst_aligned_width >> 6) - 1);
  int dst_tail_bit_width = dst_aligned_width & 0x07U;
  uint8_t front_bit_mask = 0xFFU >> dst_bit_shift;
  uint8_t tail_bit_mask = 0xFF00U >> dst_tail_bit_width;

  int inbyte_shift = (bit_shift + 8) & 0x07U;
  int crossbyte_shift = 16 - inbyte_shift;
  int cross_64_shift = 48 + inbyte_shift;
  back_8_shift = 8 - inbyte_shift;
  uint64_t inbyte_mask = 0xFFU << inbyte_shift & 0xFFU;
  inbyte_mask |= inbyte_mask << 8;
  inbyte_mask |= inbyte_mask << 16;
  inbyte_mask |= inbyte_mask << 32;
  uint64_t crossbyte_mask = ~inbyte_mask;
  uint64_t cross_64_mask = crossbyte_mask & 0xFF00000000000000ull;

  uint8_t *p_dst_line = _GetMinImageLine(p_dst_region, 0);
  const uint8_t *p_src_line = _GetMinImageLine(p_src_region, 0);
  if (!p_dst_line || !p_src_line)
    return INTERNAL_ERROR;
  for (int y = 0; y < height; ++y) {
    uint8_t *p_dst_8 = p_dst_line;
    const uint8_t *p_src_8 = p_src_line;
    p_dst_8[0] = (p_dst_8[0] & ~front_bit_mask) |
                 (((p_src_8[0] << l_shift) >> r_shift) & front_bit_mask);
    if (!r_shift && src_fragment_width > 8)
      p_dst_8[0] |= (++p_src_8)[0] >> back_8_shift & front_bit_mask;
    ++p_dst_8;
    uint64_t *p_dst_64 = reinterpret_cast<uint64_t *>(p_dst_8);
    const uint64_t *p_src_64 = reinterpret_cast<const uint64_t *>(p_src_8);
    for (int x = 0; x < dst_64_width; ++x)
      p_dst_64[x] = ((p_src_64[x]     << inbyte_shift)    & inbyte_mask)    |
                    ((p_src_64[x]     >> crossbyte_shift) & crossbyte_mask) |
                    ((p_src_64[x + 1] << cross_64_shift)  & cross_64_mask);
    for (int x = 8 * dst_64_width; x < dst_8_width; ++x)
      p_dst_8[x] = p_src_8[x]     << inbyte_shift |
                   p_src_8[x + 1] >> back_8_shift;
    if (tail_bit_mask) {
      p_dst_8 += dst_8_width;
      p_src_8 += dst_8_width;
      uint8_t src_bits = p_src_8[0] << inbyte_shift;
      if (src_fragment_width + 8 * !!r_shift > dst_fragment_width)
        src_bits |= p_src_8[1] >> back_8_shift;
      p_dst_8[0] = (p_dst_8[0] & ~tail_bit_mask) | (src_bits & tail_bit_mask);
    }
    p_dst_line += p_dst_region->stride;
    p_src_line += p_src_region->stride;
  }

  return NO_ERRORS;
}

MINIMGAPI_API int FlipMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    DirectionOption direction) {
  if (direction == DO_BOTH)
    return RotateMinImageBy90(p_dst_image, p_src_image, 2);
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  if (_CompareMinImagePrototypes(p_dst_image, p_src_image))
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_dst_image) == NO_ERRORS)
    return NO_ERRORS;

  uint32_t tangling = 0;
  PROPAGATE_ERROR(CheckMinImagesTangle(&tangling, p_dst_image, p_src_image));

  if (direction == DO_VERTICAL) {
    if (tangling != TCR_SAME_IMAGE) {
      MinImg flipped_src_image = {0};
      PROPAGATE_ERROR(_FlipMinImageVertically(&flipped_src_image, p_src_image));
      return CopyMinImage(p_dst_image, &flipped_src_image);
    }

    MinImg src_line_image = {0}, dst_line_image = {0};
    DECLARE_GUARDED_MINIMG(tmp_line_image);
    PROPAGATE_ERROR(_CloneResizedMinImagePrototype(&tmp_line_image,
                                           p_dst_image, p_dst_image->width, 1));
    PROPAGATE_ERROR(_CloneMinImagePrototype(&src_line_image,
                                            &tmp_line_image, AO_EMPTY));
    PROPAGATE_ERROR(_CloneMinImagePrototype(&dst_line_image,
                                            &tmp_line_image, AO_EMPTY));

    PROPAGATE_ERROR(_GetMinImageRegion(&src_line_image, p_src_image,
                                       0, 0, p_src_image->width, 1));
    PROPAGATE_ERROR(_GetMinImageRegion(&dst_line_image, p_dst_image,
                                       0, p_dst_image->height - 1,
                                       p_dst_image->width, 1));
    for (int y = 0; y < p_dst_image->height / 2; ++y) {
      PROPAGATE_ERROR(CopyMinImage(&tmp_line_image, &src_line_image));
      PROPAGATE_ERROR(CopyMinImage(&src_line_image, &dst_line_image));
      PROPAGATE_ERROR(CopyMinImage(&dst_line_image, &tmp_line_image));
      src_line_image.pScan0 += p_src_image->stride;
      dst_line_image.pScan0 -= p_dst_image->stride;
    }

    return NO_ERRORS;
  }

  if (direction == DO_HORIZONTAL) {
    const MinImg *p_work_dst_image = p_dst_image;
    const MinImg *p_work_src_image = p_src_image;

    MinImg dst_image = {0};
    MinImg src_image = {0};
    DECLARE_GUARDED_MINIMG(tmp_image);
    if (~tangling & TCR_INDEPENDENT_LINES ||
        ~tangling & TCR_FORWARD_PASS_POSSIBLE) {
      if (tangling & TCR_INDEPENDENT_LINES &&
          tangling & TCR_BACKWARD_PASS_POSSIBLE) {
        SHOULD_WORK(_FlipMinImageVertically(&dst_image, p_dst_image));
        SHOULD_WORK(_FlipMinImageVertically(&src_image, p_src_image));
        p_work_dst_image = &dst_image;
        p_work_src_image = &src_image;
      } else {
        PROPAGATE_ERROR(_CloneMinImagePrototype(&tmp_image, p_src_image));
        SHOULD_WORK(CopyMinImage(&tmp_image, p_src_image));
        p_work_src_image = &tmp_image;
      }
    }

    if (p_work_dst_image->addressSpace != 0)
      return NOT_IMPLEMENTED;

    int bits_per_pixel = _GetMinImageBitsPerPixel(p_work_dst_image);
    if (bits_per_pixel & 0x07U) {
      int bit_line_width = p_work_dst_image->width * bits_per_pixel;
      int byte_line_width = bit_line_width >> 3;
      int bit_tail_width = bit_line_width & 0x07U;
      uint8_t *p_dst_line = _GetMinImageLine(p_work_dst_image, 0);
      const uint8_t *p_src_line = _GetMinImageLine(p_work_src_image, 0);
      if (!p_dst_line || !p_src_line)
        return INTERNAL_ERROR;
      for (int y = 0; y < p_work_dst_image->height; ++y) {
        ::memset(p_dst_line, 0, byte_line_width);
        if (bit_tail_width)
          p_dst_line[byte_line_width] &= 0xFFU >> bit_tail_width;
        for (int i = 0, j = bit_line_width - bits_per_pixel; j >= 0;
             i += bits_per_pixel, j -= bits_per_pixel) {
          for (int b = 0; b < bits_per_pixel; ++b) {
            if (GET_IMAGE_LINE_BIT(p_src_line, j + b))
              SET_IMAGE_LINE_BIT(p_dst_line, i + b);
          }
        }
        p_dst_line += p_work_dst_image->stride;
        p_src_line += p_work_src_image->stride;
      }

      return NO_ERRORS;
    }

    int bytes_per_pixel = bits_per_pixel >> 3;
    switch (bytes_per_pixel) {
      case 1: {
        uint8_t *p_dst_line = _GetMinImageLine(p_work_dst_image, 0);
        const uint8_t *p_src_line = _GetMinImageLine(p_work_src_image, 0);
        if (!p_dst_line || !p_src_line)
          return INTERNAL_ERROR;
        for (int y = 0; y < p_work_dst_image->height; ++y) {
          for (int i = 0, j = p_work_dst_image->width - 1; j >= 0; ++i, --j)
            p_dst_line[i] = p_src_line[j];
          p_dst_line += p_work_dst_image->stride;
          p_src_line += p_work_src_image->stride;
        }
        break;
      }
      case 2: {
        uint16_t *p_dst_line = reinterpret_cast<uint16_t *>(
                                           _GetMinImageLine(p_work_dst_image, 0));
        const uint16_t *p_src_line = reinterpret_cast<uint16_t *>(
                                           _GetMinImageLine(p_work_src_image, 0));
        if (!p_dst_line || !p_src_line)
          return INTERNAL_ERROR;
        for (int y = 0; y < p_work_dst_image->height; ++y) {
          for (int i = 0, j = p_work_dst_image->width - 1; j >= 0; ++i, --j)
            p_dst_line[i] = p_src_line[j];
          p_dst_line = ShiftPtr(p_dst_line, p_work_dst_image->stride);
          p_src_line = ShiftPtr(p_src_line, p_work_src_image->stride);
        }
        break;
      }
      case 4: {
        uint32_t *p_dst_line = reinterpret_cast<uint32_t *>(
                                           _GetMinImageLine(p_work_dst_image, 0));
        const uint32_t *p_src_line = reinterpret_cast<uint32_t *>(
                                           _GetMinImageLine(p_work_src_image, 0));
        if (!p_dst_line || !p_src_line)
          return INTERNAL_ERROR;
        for (int y = 0; y < p_work_dst_image->height; ++y) {
          for (int i = 0, j = p_work_dst_image->width - 1; j >= 0; ++i, --j)
            p_dst_line[i] = p_src_line[j];
          p_dst_line = ShiftPtr(p_dst_line, p_work_dst_image->stride);
          p_src_line = ShiftPtr(p_src_line, p_work_src_image->stride);
        }
        break;
      }
      case 8: {
        uint64_t *p_dst_line = reinterpret_cast<uint64_t *>(
                                           _GetMinImageLine(p_work_dst_image, 0));
        const uint64_t *p_src_line = reinterpret_cast<uint64_t *>(
                                           _GetMinImageLine(p_work_src_image, 0));
        if (!p_dst_line || !p_src_line)
          return INTERNAL_ERROR;
        for (int y = 0; y < p_work_dst_image->height; ++y) {
          for (int i = 0, j = p_work_dst_image->width - 1; j >= 0; ++i, --j)
            p_dst_line[i] = p_src_line[j];
          p_dst_line = ShiftPtr(p_dst_line, p_work_dst_image->stride);
          p_src_line = ShiftPtr(p_src_line, p_work_src_image->stride);
        }
        break;
      }
      default: {
        uint8_t *p_dst_line = _GetMinImageLine(p_work_dst_image, 0);
        const uint8_t *p_src_line = _GetMinImageLine(p_work_src_image, 0);
        if (!p_dst_line || !p_src_line)
          return INTERNAL_ERROR;
        for (int y = 0; y < p_work_dst_image->height; ++y) {
          for (int as = 0, ad = (p_work_dst_image->width - 1) * bytes_per_pixel;
               ad >= 0; as += bytes_per_pixel, ad -= bytes_per_pixel)
            ::memcpy(p_dst_line + ad, p_src_line + as, bytes_per_pixel);
          p_dst_line += p_work_dst_image->stride;
          p_src_line += p_work_src_image->stride;
        }
      }
    }

    return NO_ERRORS;
  }

  return INTERNAL_ERROR;
}

MINIMGAPI_API int RotateMinImageBy90(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    int           num_rotations) {
  num_rotations = (num_rotations % 4 + 4) % 4;
  MinImg tmp_image = {0};

  switch (num_rotations) {
    case 0: {
      return CopyMinImage(p_dst_image, p_src_image);
    }
    case 1: {
      PROPAGATE_ERROR(_FlipMinImageVertically(&tmp_image, p_src_image));
      return TransposeMinImage(p_dst_image, &tmp_image);
    }
    case 2: {
      PROPAGATE_ERROR(_FlipMinImageVertically(&tmp_image, p_src_image));
      return FlipMinImage(p_dst_image, &tmp_image, DO_HORIZONTAL);
    }
    case 3: {
      PROPAGATE_ERROR(_FlipMinImageVertically(&tmp_image, p_dst_image));
      return TransposeMinImage(&tmp_image, p_src_image);
    }
    default: {
      return INTERNAL_ERROR;
    }
  }
}

MINIMGAPI_API int InterleaveMinImages(
    const MinImg        *p_dst_image,
    const MinImg *const *p_p_src_images,
    int                  num_src_images) {
  if (!p_p_src_images || num_src_images <= 0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));

  int sum_src_channels = 0;
  int max_src_channels = 0;
  for (int i = 0; i < num_src_images; ++i) {
    const MinImg *p_src_image = p_p_src_images[i];
    PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
    MinImg tmp_image = {0};
    PROPAGATE_ERROR(_CloneDimensionedMinImagePrototype(&tmp_image, p_dst_image,
                                              p_src_image->channels, AO_EMPTY));
    if (_CompareMinImagePrototypes(p_src_image, &tmp_image))
      return BAD_ARGS;
    sum_src_channels += p_src_image->channels;
    max_src_channels = std::max(max_src_channels, p_src_image->channels);
  }
  if (p_dst_image->channels != sum_src_channels)
    return BAD_ARGS;

  MinImg unfolded_dst_image = {0};
  PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_dst_image, p_dst_image));
  DECLARE_GUARDED_MINIMG(transfolded_dst_image);
  PROPAGATE_ERROR(_CloneTransposedMinImagePrototype(&transfolded_dst_image,
                                                    &unfolded_dst_image));

  DECLARE_GUARDED_MINIMG(transfolded_src_image);
  if (max_src_channels > 1)
    PROPAGATE_ERROR(_CloneResizedMinImagePrototype(&transfolded_src_image,
                                                   &transfolded_dst_image,
                                                   transfolded_dst_image.width,
                                        p_dst_image->width * max_src_channels));

  for (int cur_src_image = 0, cur_dst_channel = 0;
       cur_src_image < num_src_images;
       cur_dst_channel += p_p_src_images[cur_src_image++]->channels) {
    const MinImg *p_src_image = p_p_src_images[cur_src_image];
    if (p_src_image->channels == 1) {
      MinImg transposed_dst_channel = {0};
      PROPAGATE_ERROR(_SliceMinImageVertically(&transposed_dst_channel,
                                               &transfolded_dst_image,
                                               cur_dst_channel,
                                               p_dst_image->channels));
      PROPAGATE_ERROR(TransposeMinImage(&transposed_dst_channel, p_src_image));
      continue;
    }

    for (int i = 0; i < p_src_image->channels; ++i) {
      MinImg transposed_src_channel = {0};
      PROPAGATE_ERROR(_SliceMinImageVertically(&transposed_src_channel,
                                               &transfolded_src_image,
                                               i, p_src_image->channels));
      MinImg transposed_dst_channel = {0};
      PROPAGATE_ERROR(_SliceMinImageVertically(&transposed_dst_channel,
                                               &transfolded_dst_image,
                                               cur_dst_channel + i,
                                               p_dst_image->channels));
      PROPAGATE_ERROR(CopyMinImage(&transposed_dst_channel,
                                   &transposed_src_channel));
    }
  }

  return TransposeMinImage(&unfolded_dst_image, &transfolded_dst_image);
}

MINIMGAPI_API int DeinterleaveMinImage(
    const MinImg *const *p_p_dst_images,
    const MinImg        *p_src_image,
    int                  num_dst_images) {
  if (!p_p_dst_images || num_dst_images <= 0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));

  int sum_dst_channels = 0;
  int max_dst_channels = 0;
  for (int i = 0; i < num_dst_images; ++i) {
    const MinImg *p_dst_image = p_p_dst_images[i];
    PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
    MinImg tmp_image = {0};
    PROPAGATE_ERROR(_CloneDimensionedMinImagePrototype(&tmp_image, p_src_image,
                                              p_dst_image->channels, AO_EMPTY));
    if (_CompareMinImagePrototypes(p_dst_image, &tmp_image))
      return BAD_ARGS;
    sum_dst_channels += p_dst_image->channels;
    max_dst_channels = std::max(max_dst_channels, p_dst_image->channels);
  }
  if (p_src_image->channels != sum_dst_channels)
    return BAD_ARGS;

  MinImg unfolded_src_image = {0};
  PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_src_image, p_src_image));
  DECLARE_GUARDED_MINIMG(transfolded_src_image);
  PROPAGATE_ERROR(_CloneTransposedMinImagePrototype(&transfolded_src_image,
                                                    &unfolded_src_image));
  PROPAGATE_ERROR(TransposeMinImage(&transfolded_src_image,
                                    &unfolded_src_image));

  DECLARE_GUARDED_MINIMG(transfolded_dst_image);
  if (max_dst_channels > 1)
    PROPAGATE_ERROR(_CloneResizedMinImagePrototype(&transfolded_dst_image,
                                                   &transfolded_src_image,
                                                   transfolded_src_image.width,
                                        p_src_image->width * max_dst_channels));

  for (int cur_dst_image = 0, cur_src_channel = 0;
       cur_dst_image < num_dst_images;
       cur_src_channel += p_p_dst_images[cur_dst_image++]->channels) {
    const MinImg *p_dst_image = p_p_dst_images[cur_dst_image];
    if (p_dst_image->channels == 1) {
      MinImg transposed_src_channel = {0};
      PROPAGATE_ERROR(_SliceMinImageVertically(&transposed_src_channel,
                                               &transfolded_src_image,
                                               cur_src_channel,
                                               p_src_image->channels));
      PROPAGATE_ERROR(TransposeMinImage(p_dst_image, &transposed_src_channel));
      continue;
    }

    MinImg unfolded_dst_image = {0};
    PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_dst_image, p_dst_image));

    transfolded_dst_image.height = p_src_image->width * p_dst_image->channels;
    for (int i = 0; i < p_dst_image->channels; ++i) {
      MinImg transposed_src_channel = {0};
      PROPAGATE_ERROR(_SliceMinImageVertically(&transposed_src_channel,
                                               &transfolded_src_image,
                                               cur_src_channel + i,
                                               p_src_image->channels));
      MinImg transposed_dst_channel = {0};
      PROPAGATE_ERROR(_SliceMinImageVertically(&transposed_dst_channel,
                                               &transfolded_dst_image,
                                               i, p_dst_image->channels));
      PROPAGATE_ERROR(CopyMinImage(&transposed_dst_channel,
                                   &transposed_src_channel));
    }
    PROPAGATE_ERROR(TransposeMinImage(&unfolded_dst_image,
                                      &transfolded_dst_image));
  }

  return NO_ERRORS;
}
