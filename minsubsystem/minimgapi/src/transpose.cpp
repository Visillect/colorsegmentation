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
#include <minbase/minresult.h>
#include <minutils/smartptr.h>
#include <minbase/crossplat.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/minimgapi-inl.h>
#include <minimgapi/imgguard.hpp>
#include "vector/transpose-inl.h"
#include "bitcpy.h"

#if defined(MINSTOPWATCH_ENABLED)
#  include <minstopwatch/stopwatch.hpp>
DECLARE_MINSTOPWATCH(gsw_TransposeMinImage, "TransposeMinImage");
#endif // defined(MINSTOPWATCH_ENABLED)

#if defined(USE_ELBRUS_SIMD)
#include <eml/eml_image.h>
#endif

static int Transpose1BitImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height) {

  int src_wd32 = (src_width  >> 5) << 2;
  int src_ht32 = (src_height >> 5) << 2;
  int src_wd8 = src_width >> 3;
  int src_ht8 = src_height >> 3;
  int src_wd1 = src_width & 7;
  int src_ht1 = src_height & 7;

  uint8_t mask_to_leave = 0xFFU >> src_wd1;

  for (int src_y = 0; src_y < src_ht32; src_y += 4)
    for (int src_x = 0; src_x < src_wd32; src_x += 4)
      Transpose32x32Bits(p_dst_buffer + 8 * src_x * dst_stride + src_y,
                         dst_stride,
                         p_src_buffer + 8 * src_y * src_stride + src_x,
                         src_stride);

  for (int src_y = 0; src_y < src_ht32; ++src_y)
    for (int src_x = src_wd32; src_x < src_wd8; ++src_x)
      Transpose8x8Bits(p_dst_buffer + 8 * src_x * dst_stride + src_y,
                       dst_stride,
                       p_src_buffer + 8 * src_y * src_stride + src_x,
                       src_stride);

  for (int src_y = src_ht32; src_y < src_ht8; ++src_y)
    for (int src_x = 0; src_x < src_wd8; ++src_x)
      Transpose8x8Bits(p_dst_buffer + 8 * src_x * dst_stride + src_y,
                       dst_stride,
                       p_src_buffer + 8 * src_y * src_stride + src_x,
                       src_stride);

  uint8_t src_buf[8];
  uint8_t dst_buf[8];
  uint8_t *p_dst_t = NULL;
  const uint8_t *p_src_t = NULL;

  for (int src_y = 0; src_y < src_ht8; ++src_y) {
    p_src_t = p_src_buffer + 8 * src_y * src_stride + src_wd8;
    for (int i = 0; i < 8; ++i)
      src_buf[i] = p_src_t[i * src_stride];
    Transpose8x8BitsInternal(reinterpret_cast<uint64_t *>(dst_buf),
                             reinterpret_cast<uint64_t *>(src_buf));
    for (int i = 0; i < src_wd1; ++i)
      *(p_dst_buffer + (8 * src_wd8 + i) * dst_stride + src_y) = dst_buf[i];
  }

  for (int src_x = 0; src_x < src_wd8; ++src_x) {
    *(reinterpret_cast<uint64_t *>(src_buf)) = 0;
    for (int i = 0; i < src_ht1; ++i)
      src_buf[i] = *(p_src_buffer + (8 * src_ht8 + i) * src_stride + src_x);
    Transpose8x8BitsInternal(reinterpret_cast<uint64_t *>(dst_buf),
                             reinterpret_cast<uint64_t *>(src_buf));
    p_dst_t = p_dst_buffer + 8 * src_x * dst_stride + src_ht8;
    for (int i = 0; i < 8; ++i) {
      p_dst_t[i * dst_stride] &= mask_to_leave;
      p_dst_t[i * dst_stride] |= dst_buf[i];
    }
  }

  *(reinterpret_cast<uint64_t *>(src_buf)) = 0;
  p_src_t = p_src_buffer + 8 * src_ht8 * src_stride + src_wd8;
  p_dst_t = p_dst_buffer + 8 * src_wd8 * dst_stride + src_ht8;
  for (int i = 0; i < src_ht1; ++i)
    src_buf[i] = p_src_t[i * src_stride];
  Transpose8x8BitsInternal(reinterpret_cast<uint64_t *>(dst_buf),
                           reinterpret_cast<uint64_t *>(src_buf));
  for (int i = 0; i < src_wd1; ++i) {
    p_dst_t[i * dst_stride] &= mask_to_leave;
    p_dst_t[i * dst_stride] |= dst_buf[i];
  }

  return NO_ERRORS;
}

static int Transpose8BitImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height) {
  int src_aligned_width = src_width & ~0x0F;
  int src_aligned_height = src_height & ~0x0F;

  if (src_aligned_width > 0)
    for (int src_y = 0; src_y < src_aligned_height; src_y += 16) {
      const uint8_t *p_src_row = p_src_buffer + src_y * src_stride;
      uint8_t *p_dst_column = p_dst_buffer + src_y;
      for (int src_x = 0; src_x < src_aligned_width; src_x += 16)
        Transpose16x16(p_dst_column + src_x * dst_stride, dst_stride,
                       p_src_row + src_x, src_stride);
    }

  if (src_aligned_width < src_width)
    for (int src_y = 0; src_y < src_aligned_height; ++src_y) {
      const uint8_t *p_src_row = p_src_buffer + src_y * src_stride;
      uint8_t *p_dst_column = p_dst_buffer + src_y;
      for (int src_x = src_aligned_width; src_x < src_width; ++src_x)
        p_dst_column[src_x * dst_stride] = p_src_row[src_x];
    }

  for (int src_y = src_aligned_height; src_y < src_height; ++src_y) {
    const uint8_t *p_src_row = p_src_buffer + src_y * src_stride;
    uint8_t *p_dst_column = p_dst_buffer + src_y;
    for (int src_x = 0; src_x < src_width; ++src_x)
      p_dst_column[src_x * dst_stride] = p_src_row[src_x];
  }

  return NO_ERRORS;
}

static int Transpose8BitImage16x128Vertical(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height) {
  // Comparing to Transpose8BitImage 16x16 blocks traversal order is modified to reduce number of L1D cache misses.
  // Source image is vertically traversed by 16x128 blocks.
  // 16x128 blocks are horizontally traversed by 16x16 blocks.
  // Such algorithm reduces number of L1D cache misses when accessing destination image data.
  int src_aligned_width = src_width & ~0x0F;
  int src_aligned_width_128 = src_width & ~0x7F;
  int src_aligned_height = src_height & ~0x0F;

  if (src_aligned_height > 0) {
    for (int src_x = 0; src_x < src_aligned_width_128; src_x += 128) {
      const uint8_t *p_src_row = p_src_buffer + src_x;
      uint8_t *p_dst_column = p_dst_buffer + src_x * dst_stride;
      for (int src_y = 0; src_y < src_aligned_height - 16; src_y += 16) {
        const uint8_t *p_src_row2 = p_src_row + src_y * src_stride;
        uint8_t *p_dst_column2 = p_dst_column + src_y;

        MIN_PREFETCH(p_src_row2 + 16 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 17 * src_stride, 0);
        Transpose16x16(p_dst_column2, dst_stride,
                       p_src_row2, src_stride);
        MIN_PREFETCH(p_src_row2 + 18 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 19 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 16 * dst_stride, dst_stride,
                       p_src_row2 + 16, src_stride);
        MIN_PREFETCH(p_src_row2 + 20 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 21 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 32 * dst_stride, dst_stride,
                       p_src_row2 + 32, src_stride);
        MIN_PREFETCH(p_src_row2 + 22 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 23 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 48 * dst_stride, dst_stride,
                       p_src_row2 + 48, src_stride);
        MIN_PREFETCH(p_src_row2 + 24 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 25 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 64 * dst_stride, dst_stride,
                       p_src_row2 + 64, src_stride);
        MIN_PREFETCH(p_src_row2 + 26 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 27 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 80 * dst_stride, dst_stride,
                       p_src_row2 + 80, src_stride);
        MIN_PREFETCH(p_src_row2 + 28 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 29 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 96 * dst_stride, dst_stride,
                       p_src_row2 + 96, src_stride);
        MIN_PREFETCH(p_src_row2 + 30 * src_stride, 0);
        MIN_PREFETCH(p_src_row2 + 31 * src_stride, 0);
        Transpose16x16(p_dst_column2 + 112 * dst_stride, dst_stride,
                       p_src_row2 + 112, src_stride);
      }
      const uint8_t *p_src_row2 = p_src_row + (src_aligned_height - 16) * src_stride;
      uint8_t *p_dst_column2 = p_dst_column + src_aligned_height - 16;
      Transpose16x16(p_dst_column2, dst_stride,
                     p_src_row2, src_stride);
      Transpose16x16(p_dst_column2 + 16 * dst_stride, dst_stride,
                     p_src_row2 + 16, src_stride);
      Transpose16x16(p_dst_column2 + 32 * dst_stride, dst_stride,
                     p_src_row2 + 32, src_stride);
      Transpose16x16(p_dst_column2 + 48 * dst_stride, dst_stride,
                     p_src_row2 + 48, src_stride);
      Transpose16x16(p_dst_column2 + 64 * dst_stride, dst_stride,
                     p_src_row2 + 64, src_stride);
      Transpose16x16(p_dst_column2 + 80 * dst_stride, dst_stride,
                     p_src_row2 + 80, src_stride);
      Transpose16x16(p_dst_column2 + 96 * dst_stride, dst_stride,
                     p_src_row2 + 96, src_stride);
      Transpose16x16(p_dst_column2 + 112 * dst_stride, dst_stride,
                     p_src_row2 + 112, src_stride);
    }

    for (int src_y = 0; src_y < src_aligned_height; src_y += 16) {
      const uint8_t *p_src_row = p_src_buffer + src_y * src_stride;
      uint8_t *p_dst_column = p_dst_buffer + src_y;
      for (int src_x = src_aligned_width_128; src_x < src_aligned_width; src_x += 16) {
        Transpose16x16(p_dst_column + src_x * dst_stride, dst_stride,
                       p_src_row + src_x, src_stride);
      }

      for (int src_x = src_aligned_width; src_x < src_width; ++src_x) {
        for (int i = 0; i < 16; ++i)
          p_dst_column[src_x * dst_stride + i] = p_src_row[i * src_stride + src_x];
      }
    }
  }

  for (int src_x = 0; src_x < src_width; ++src_x) {
    const uint8_t *p_src_row = p_src_buffer + src_x;
    uint8_t *p_dst_column = p_dst_buffer + src_x * dst_stride;
    for (int src_y = src_aligned_height; src_y < src_height; ++src_y)
      p_dst_column[src_y] = p_src_row[src_y * src_stride];
  }

  return NO_ERRORS;
}
static int Transpose16BitImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height) {
  int src_aligned_width = src_width & ~0x07;
  int src_aligned_height = src_height & ~0x07;

  if (src_aligned_width > 0)
    for (int src_y = 0; src_y < src_aligned_height; src_y += 8) {
      const uint16_t *p_src_row =
        reinterpret_cast<const uint16_t *>(p_src_buffer + src_y * src_stride);
      uint8_t *p_dst_column = p_dst_buffer + src_y * 2;
      for (int src_x = 0; src_x < src_aligned_width; src_x += 8)
        Transpose8x8(
          reinterpret_cast<uint16_t *>(p_dst_column + src_x * dst_stride),
          dst_stride, p_src_row + src_x, src_stride);
    }

  if (src_aligned_width < src_width)
    for (int src_y = 0; src_y < src_aligned_height; ++src_y) {
      const uint16_t *p_src_row =
        reinterpret_cast<const uint16_t *>(p_src_buffer + src_y * src_stride);
      uint8_t *p_dst_column = p_dst_buffer + src_y * 2;
      for (int src_x = src_aligned_width; src_x < src_width; ++src_x)
        *reinterpret_cast<uint16_t *>(p_dst_column + src_x * dst_stride) =
          p_src_row[src_x];
    }

  for (int src_y = src_aligned_height; src_y < src_height; ++src_y) {
    const uint16_t *p_src_row =
      reinterpret_cast<const uint16_t *>(p_src_buffer + src_y * src_stride);
    uint8_t *p_dst_column = p_dst_buffer + src_y * 2;
    for (int src_x = 0; src_x < src_width; ++src_x)
      *reinterpret_cast<uint16_t *>(p_dst_column + src_x * dst_stride) =
        p_src_row[src_x];
  }

  return NO_ERRORS;
}

static int Transpose32BitImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height) {
  int src_aligned_width = src_width & ~0x03;
  int src_aligned_height = src_height & ~0x03;

  if (src_aligned_width > 0)
    for (int src_y = 0; src_y < src_aligned_height; src_y += 4) {
      const uint32_t *p_src_row =
        reinterpret_cast<const uint32_t *>(p_src_buffer + src_y * src_stride);
      uint8_t *p_dst_column = p_dst_buffer + src_y * 4;
      for (int src_x = 0; src_x < src_aligned_width; src_x += 4)
        Transpose4x4(
          reinterpret_cast<uint32_t *>(p_dst_column + src_x * dst_stride),
          dst_stride, p_src_row + src_x, src_stride);
    }

  if (src_aligned_width < src_width)
    for (int src_y = 0; src_y < src_aligned_height; ++src_y) {
      const uint32_t *p_src_row =
        reinterpret_cast<const uint32_t *>(p_src_buffer + src_y * src_stride);
      uint8_t *p_dst_column = p_dst_buffer + src_y * 4;
      for (int src_x = src_aligned_width; src_x < src_width; ++src_x)
        *reinterpret_cast<uint32_t *>(p_dst_column + src_x * dst_stride) =
          p_src_row[src_x];
    }

  for (int src_y = src_aligned_height; src_y < src_height; ++src_y) {
    const uint32_t *p_src_row =
      reinterpret_cast<const uint32_t *>(p_src_buffer + src_y * src_stride);
    uint8_t *p_dst_column = p_dst_buffer + src_y * 4;
    for (int src_x = 0; src_x < src_width; ++src_x)
      *reinterpret_cast<uint32_t *>(p_dst_column + src_x * dst_stride) =
        p_src_row[src_x];
  }

  return NO_ERRORS;
}

static int Transpose64BitImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height) {
  for (int src_y = 0; src_y < src_height; ++src_y) {
    const uint64_t *p_src_row =
      reinterpret_cast<const uint64_t *>(p_src_buffer + src_y * src_stride);
    uint8_t *p_dst_column = p_dst_buffer + src_y * 8;
    for (int src_x = 0; src_x < src_width; ++src_x)
      *reinterpret_cast<uint64_t *>(p_dst_column + src_x * dst_stride) =
        p_src_row[src_x];
  }

  return NO_ERRORS;
}

static int TransposeNBytesImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_height,
    int            element_byte_size) {
  for (int src_y = 0; src_y < src_height; ++src_y) {
    const uint8_t *p_src_row = p_src_buffer + src_y * src_stride;
    uint8_t *p_dst_column = p_dst_buffer + src_y * element_byte_size;
    for (int src_x = 0; src_x < src_width; ++src_x)
      ::memcpy(p_dst_column + src_x * dst_stride,
               p_src_row + src_x * element_byte_size, element_byte_size);
  }

  return NO_ERRORS;
}

static int TransposeNBitsImage(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride,
    int            src_width,
    int            src_heigth,
    int            element_bit_size) {
  for (int src_y = 0; src_y < src_heigth; ++src_y) {
    int dst_bit_x = src_y * element_bit_size;
    uint8_t *p_dst_col = p_dst_buffer + dst_bit_x / 8;
    uint8_t dst_shift = dst_bit_x & 7;
    const uint8_t *p_src_row = p_src_buffer + src_y * src_stride;
    for (int src_x = 0; src_x < src_width; ++src_x) {
      int src_bit_x = src_x * element_bit_size;
      bitcpy(p_dst_col + src_x * dst_stride, dst_shift,
             p_src_row + src_bit_x / 8, src_bit_x & 7, element_bit_size);
    }
  }

  return NO_ERRORS;
}


MINIMGAPI_API int TransposeMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image) {
#if defined(MINSTOPWATCH_ENABLED)
  DECLARE_MINSTOPWATCH_CTL(gsw_TransposeMinImage);
#endif // defined(MINSTOPWATCH_ENABLED)

  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  DECLARE_GUARDED_MINIMG(tmp_image);
  PROPAGATE_ERROR(CloneTransposedMinImagePrototype(&tmp_image,
                                                   p_dst_image, AO_EMPTY));
  if (CompareMinImagePrototypes(&tmp_image, p_src_image))
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_src_image) == NO_ERRORS)
    return NO_ERRORS;
  if (p_dst_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  const MinImg *p_work_dst_image = p_dst_image;
  const MinImg *p_work_src_image = p_src_image;

  uint32_t tangling = 0;
  PROPAGATE_ERROR(CheckMinImagesTangle(&tangling, p_dst_image, p_src_image));
  if (tangling != TCR_INDEPENDENT_IMAGES) {
    PROPAGATE_ERROR(AllocMinImage(&tmp_image));
    PROPAGATE_ERROR(CopyMinImage(&tmp_image, p_src_image));
    p_work_src_image = &tmp_image;
  }

  int bits_per_pixel = GetMinImageBitsPerPixel(p_work_src_image);
  if (bits_per_pixel == 1)
    return Transpose1BitImage(p_work_dst_image->pScan0,
                              p_work_dst_image->stride,
                              p_work_src_image->pScan0,
                              p_work_src_image->stride,
                              p_work_src_image->width,
                              p_work_src_image->height);
  if (bits_per_pixel & 0x07)
    return TransposeNBitsImage(p_work_dst_image->pScan0,
                               p_work_dst_image->stride,
                               p_work_src_image->pScan0,
                               p_work_src_image->stride,
                               p_work_src_image->width,
                               p_work_src_image->height,
                               bits_per_pixel);

  int bytes_per_pixel = bits_per_pixel >> 3;
  switch (bytes_per_pixel) {
  case 1:
#if defined(USE_ELBRUS_SIMD)
    if ((p_work_src_image->stride > 0) && (p_work_dst_image->stride > 0))
    {
      eml_image *p_src_eml= eml_Image_CreateStruct(EML_UCHAR, p_work_src_image->channels,
                                                   p_work_src_image->width, p_work_src_image->height,
                                                   p_work_src_image->stride, p_work_src_image->pScan0);
      eml_image *p_src_trans_eml = eml_Image_CreateStruct(EML_UCHAR, p_work_dst_image->channels,
                                                          p_work_dst_image->width, p_work_dst_image->height,
                                                          p_work_dst_image->stride, p_work_dst_image->pScan0);
      NEED_NO_ERRORS(eml_Image_FlipMain(p_src_eml, p_src_trans_eml));
      return NO_ERRORS;
    }
    else
      return Transpose8BitImage(p_work_dst_image->pScan0,
                                p_work_dst_image->stride,
                                p_work_src_image->pScan0,
                                p_work_src_image->stride,
                                p_work_src_image->width,
                                p_work_src_image->height);
#elif defined(USE_NEON_SIMD)
    // Transpose8BitImage16x128Vertical is not always faster than Transpose8BitImage,
    // so it is not used here. It was tested on 'odroid' platform.
    return Transpose8BitImage(p_work_dst_image->pScan0,
                              p_work_dst_image->stride,
                              p_work_src_image->pScan0,
                              p_work_src_image->stride,
                              p_work_src_image->width,
                              p_work_src_image->height);
#else // !USE_NEON_SIMD
    if (p_work_src_image->height >= 16 && p_work_src_image->width >= 128)
      return Transpose8BitImage16x128Vertical (
            p_work_dst_image->pScan0,
            p_work_dst_image->stride,
            p_work_src_image->pScan0,
            p_work_src_image->stride,
            p_work_src_image->width,
            p_work_src_image->height);
    else
      return Transpose8BitImage(p_work_dst_image->pScan0,
                                p_work_dst_image->stride,
                                p_work_src_image->pScan0,
                                p_work_src_image->stride,
                                p_work_src_image->width,
                                p_work_src_image->height);
#endif // !USE_NEON_SIMD
  case 2:
#if defined(USE_ELBRUS_SIMD)
    if ((p_work_src_image->stride > 0) && (p_work_dst_image->stride > 0))
    {
      eml_image *p_src_eml= eml_Image_CreateStruct(EML_SHORT, p_work_src_image->channels,
                                                   p_work_src_image->width, p_work_src_image->height,
                                                   p_work_src_image->stride / 2, p_work_src_image->pScan0);
      eml_image *p_src_trans_eml = eml_Image_CreateStruct(EML_SHORT, p_work_dst_image->channels,
                                                          p_work_dst_image->width, p_work_dst_image->height,
                                                          p_work_dst_image->stride / 2, p_work_dst_image->pScan0);
      NEED_NO_ERRORS(eml_Image_FlipMain(p_src_eml, p_src_trans_eml));
      return NO_ERRORS;
    }
#endif
    return Transpose16BitImage(p_work_dst_image->pScan0,
                               p_work_dst_image->stride,
                               p_work_src_image->pScan0,
                               p_work_src_image->stride,
                               p_work_src_image->width,
                               p_work_src_image->height);
  case 4:
#if defined(USE_ELBRUS_SIMD)
    if ((p_work_src_image->stride > 0) && (p_work_dst_image->stride > 0))
    {
      eml_image *p_src_eml= eml_Image_CreateStruct(EML_FLOAT, p_work_src_image->channels,
                                                   p_work_src_image->width, p_work_src_image->height,
                                                   p_work_src_image->stride / 4, p_work_src_image->pScan0);
      eml_image *p_src_trans_eml = eml_Image_CreateStruct(EML_FLOAT, p_work_dst_image->channels,
                                                          p_work_dst_image->width, p_work_dst_image->height,
                                                          p_work_dst_image->stride / 4, p_work_dst_image->pScan0);
      NEED_NO_ERRORS(eml_Image_FlipMain(p_src_eml, p_src_trans_eml));
      return NO_ERRORS;
    }
#endif
    return Transpose32BitImage(p_work_dst_image->pScan0,
                               p_work_dst_image->stride,
                               p_work_src_image->pScan0,
                               p_work_src_image->stride,
                               p_work_src_image->width,
                               p_work_src_image->height);
  case 8:
    return Transpose64BitImage(p_work_dst_image->pScan0,
                               p_work_dst_image->stride,
                               p_work_src_image->pScan0,
                               p_work_src_image->stride,
                               p_work_src_image->width,
                               p_work_src_image->height);
  default:
    return TransposeNBytesImage(p_work_dst_image->pScan0,
                                p_work_dst_image->stride,
                                p_work_src_image->pScan0,
                                p_work_src_image->stride,
                                p_work_src_image->width,
                                p_work_src_image->height,
                                bytes_per_pixel);
  }
}
