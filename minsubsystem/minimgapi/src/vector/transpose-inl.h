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

#pragma once
#ifndef MINIMGAPI_SRC_VECTOR_TRANSPOSE_INL_H_INCLUDED
#define MINIMGAPI_SRC_VECTOR_TRANSPOSE_INL_H_INCLUDED

#include <minutils/smartptr.h>
#include <minbase/crossplat.h>

#if defined(USE_SSE_SIMD)
#include "sse/transpose-inl.h"
#elif defined(USE_NEON_SIMD)
#include "neon/transpose-inl.h"
#else

static MUSTINLINE void Transpose16x16(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride) {
  for (int src_y = 0; src_y < 16; ++src_y) {
    const uint8_t *p_src_row = ShiftPtr(p_src_buffer, src_y * src_stride);
    uint8_t *p_dst_column = p_dst_buffer + src_y;
    for (int src_x = 0; src_x < 16; ++src_x)
      *ShiftPtr(p_dst_column, src_x * dst_stride) = p_src_row[src_x];
  }
}

static MUSTINLINE void Transpose8x8(
    uint16_t       *p_dst_buffer,
    int             dst_stride,
    const uint16_t *p_src_buffer,
    int             src_stride) {
  for (int src_y = 0; src_y < 8; ++src_y) {
    const uint16_t *p_src_row = ShiftPtr(p_src_buffer, src_y * src_stride);
    uint16_t *p_dst_column = p_dst_buffer + src_y;
    for (int src_x = 0; src_x < 8; ++src_x)
      *ShiftPtr(p_dst_column, src_x * dst_stride) = p_src_row[src_x];
  }
}

static MUSTINLINE void Transpose4x4(
    uint32_t       *p_dst_buffer,
    int             dst_stride,
    const uint32_t *p_src_buffer,
    int             src_stride) {
  for (int src_y = 0; src_y < 4; ++src_y) {
    const uint32_t *p_src_row = ShiftPtr(p_src_buffer, src_y * src_stride);
    uint32_t *p_dst_column = p_dst_buffer + src_y;
    for (int src_x = 0; src_x < 4; ++src_x)
      *ShiftPtr(p_dst_column, src_x * dst_stride) = p_src_row[src_x];
  }
}

#endif

#define PERMUTE_32_01_01(p_dst, dst_stride, p_src, src_stride, buf)   \
  buf =      (*(p_src)                     << 1 & 0xAAAAAAAA) |       \
             (*ShiftPtr(p_src, src_stride)      & 0x55555555);        \
  *(p_dst) = (*(p_src)                          & 0xAAAAAAAA) |       \
             (*ShiftPtr(p_src, src_stride) >> 1 & 0x55555555);        \
  *ShiftPtr(p_dst, dst_stride) = buf;
#define PERMUTE_32_01_02(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_01_01(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_01_01(ShiftPtr(p_dst, 2 * dst_stride), dst_stride,       \
                   ShiftPtr(p_src, 2 * src_stride), src_stride, buf)
#define PERMUTE_32_01_04(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_01_02(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_01_02(p_dst + dst_stride, dst_stride,                    \
                   p_src + src_stride, src_stride, buf)
#define PERMUTE_32_01_08(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_01_04(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_01_04(p_dst + 2 * dst_stride, dst_stride,                \
                   p_src + 2 * src_stride, src_stride, buf)
#define PERMUTE_32_01_16(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_01_08(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_01_08(p_dst + 4 * dst_stride, dst_stride,                \
                   p_src + 4 * src_stride, src_stride, buf)

#define PERMUTE_32_02_01(p_dst, dst_stride, p_src, src_stride, buf)   \
  buf =      (*(p_src)                         << 2 & 0xCCCCCCCC) |   \
             (*ShiftPtr(p_src, 2 * src_stride)      & 0x33333333);    \
  *(p_dst) = (*(p_src)                              & 0xCCCCCCCC) |   \
             (*ShiftPtr(p_src, 2 * src_stride) >> 2 & 0x33333333);    \
  *ShiftPtr(p_dst, 2 * dst_stride) = buf
#define PERMUTE_32_02_02(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_02_01(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_02_01(ShiftPtr(p_dst, dst_stride), dst_stride,           \
                   ShiftPtr(p_src, src_stride), src_stride, buf)
#define PERMUTE_32_02_04(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_02_02(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_02_02(p_dst + dst_stride, dst_stride,                    \
                   p_src + src_stride, src_stride, buf)
#define PERMUTE_32_02_08(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_02_04(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_02_04(p_dst + 2 * dst_stride, dst_stride,                \
                   p_src + 2 * src_stride, src_stride, buf)
#define PERMUTE_32_02_16(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_02_08(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_02_08(p_dst + 4 * dst_stride, dst_stride,                \
                   p_src + 4 * src_stride, src_stride, buf)

#define PERMUTE_32_04_01(p_dst, dst_stride, p_src, src_stride, buf)   \
  buf =      (*(p_src)                         << 4 & 0xF0F0F0F0) |   \
             (*ShiftPtr(p_src, 4 * src_stride)      & 0x0F0F0F0F);    \
  *(p_dst) = (*(p_src)                              & 0xF0F0F0F0) |   \
             (*ShiftPtr(p_src, 4 * src_stride) >> 4 & 0x0F0F0F0F);    \
  *ShiftPtr(p_dst, 4 * dst_stride) = buf
#define PERMUTE_32_04_02(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_04_01(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_04_01(ShiftPtr(p_dst, dst_stride), dst_stride,           \
                   ShiftPtr(p_src, src_stride), src_stride, buf)
#define PERMUTE_32_04_04(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_04_02(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_04_02(ShiftPtr(p_dst, 2 * dst_stride), dst_stride,       \
                   ShiftPtr(p_src, 2 * src_stride), src_stride, buf)
#define PERMUTE_32_04_08(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_04_04(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_04_04(p_dst + 2 * dst_stride, dst_stride,                \
                   p_src + 2 * src_stride, src_stride, buf)
#define PERMUTE_32_04_16(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_04_08(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_04_08(p_dst + 4 * dst_stride, dst_stride,                \
                   p_src + 4 * src_stride, src_stride, buf)

#define PERMUTE_32_08_01(p_dst, dst_stride, p_src, src_stride, buf)   \
  buf =      (*(p_src)                         >> 8 & 0x00FF00FF) |   \
             (*ShiftPtr(p_src, 8 * src_stride)      & 0xFF00FF00);    \
  *(p_dst) = (*(p_src)                              & 0x00FF00FF) |   \
             (*ShiftPtr(p_src, 8 * src_stride) << 8 & 0xFF00FF00);    \
  *ShiftPtr(p_dst, 8 * dst_stride) = buf
#define PERMUTE_32_08_02(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_08_01(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_08_01(ShiftPtr(p_dst, dst_stride), dst_stride,           \
                   ShiftPtr(p_src, src_stride), src_stride, buf)
#define PERMUTE_32_08_04(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_08_02(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_08_02(ShiftPtr(p_dst, 2 * dst_stride), dst_stride,       \
                   ShiftPtr(p_src, 2 * src_stride), src_stride, buf)
#define PERMUTE_32_08_08(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_08_04(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_08_04(p_dst + dst_stride, dst_stride,                    \
                   p_src + src_stride, src_stride, buf)
#define PERMUTE_32_08_16(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_08_08(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_08_08(p_dst + 4 * dst_stride, dst_stride,                \
                   p_src + 4 * src_stride, src_stride, buf)

#define PERMUTE_32_16_01(p_dst, dst_stride, p_src, src_stride, buf)   \
  buf =      (*(p_src)                          >> 16 & 0x0000FFFF) | \
             (*ShiftPtr(p_src, 16 * src_stride)       & 0xFFFF0000);  \
  *(p_dst) = (*(p_src)                                & 0x0000FFFF) | \
             (*ShiftPtr(p_src, 16 * src_stride) << 16 & 0xFFFF0000);  \
  *ShiftPtr(p_dst, 16 * dst_stride) = buf
#define PERMUTE_32_16_02(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_16_01(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_16_01(ShiftPtr(p_dst, dst_stride), dst_stride,           \
                   ShiftPtr(p_src, src_stride), src_stride, buf)
#define PERMUTE_32_16_04(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_16_02(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_16_02(ShiftPtr(p_dst, 2 * dst_stride), dst_stride,       \
                   ShiftPtr(p_src, 2 * src_stride), src_stride, buf)
#define PERMUTE_32_16_08(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_16_04(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_16_04(p_dst + dst_stride, dst_stride,                    \
                   p_src + src_stride, src_stride, buf)
#define PERMUTE_32_16_16(p_dst, dst_stride, p_src, src_stride, buf)   \
  PERMUTE_32_16_08(p_dst, dst_stride, p_src, src_stride, buf);        \
  PERMUTE_32_16_08(p_dst + 2 * dst_stride, dst_stride,                \
                   p_src + 2 * src_stride, src_stride, buf)

static MUSTINLINE void Transpose32x32Bits(
    uint8_t       *p_dst,
    int            dst_stride,
    const uint8_t *p_src,
    int            src_stride) {
  uint32_t *p_dst_32       = reinterpret_cast<      uint32_t *>(p_dst);
  const uint32_t *p_src_32 = reinterpret_cast<const uint32_t *>(p_src);
  uint32_t buf;
  PERMUTE_32_01_16(p_dst_32, dst_stride, p_src_32, src_stride, buf);
  PERMUTE_32_02_16(p_dst_32, dst_stride, p_dst_32, dst_stride, buf);
  PERMUTE_32_04_16(p_dst_32, dst_stride, p_dst_32, dst_stride, buf);
  PERMUTE_32_08_16(p_dst_32, dst_stride, p_dst_32, dst_stride, buf);
  PERMUTE_32_16_16(p_dst_32, dst_stride, p_dst_32, dst_stride, buf);
}

#define PERMUTE_811(p_dst, dst_stride, p_src, src_stride, buf)        \
  buf = (*(p_src) << 1 & 0xAA) | (*(p_src + src_stride)      & 0x55); \
  *(p_dst) = (*(p_src) & 0xAA) | (*(p_src + src_stride) >> 1 & 0x55); \
  *(p_dst + dst_stride) = buf
#define PERMUTE_812(p_dst, dst_stride, p_src, src_stride, buf)        \
  PERMUTE_811(p_dst, dst_stride, p_src, src_stride, buf);             \
  PERMUTE_811(p_dst + 2 * dst_stride, dst_stride,                     \
              p_src + 2 * src_stride, src_stride, buf)
#define PERMUTE_814(p_dst, dst_stride, p_src, src_stride, buf)        \
  PERMUTE_812(p_dst, dst_stride, p_src, src_stride, buf);             \
  PERMUTE_812(p_dst + 4 * dst_stride, dst_stride,                     \
              p_src + 4 * src_stride, src_stride, buf)

#define PERMUTE_821(p_dst, dst_stride, p_src, src_stride, buf)            \
  buf = (*(p_src) << 2 & 0xCC) | (*(p_src + 2 * src_stride)      & 0x33); \
  *(p_dst) = (*(p_src) & 0xCC) | (*(p_src + 2 * src_stride) >> 2 & 0x33); \
  *(p_dst + 2 * dst_stride) = buf
#define PERMUTE_822(p_dst, dst_stride, p_src, src_stride, buf)        \
  PERMUTE_821(p_dst, dst_stride, p_src, src_stride, buf);             \
  PERMUTE_821(p_dst + dst_stride, dst_stride,                         \
              p_src + src_stride, src_stride, buf)
#define PERMUTE_824(p_dst, dst_stride, p_src, src_stride, buf)        \
  PERMUTE_822(p_dst, dst_stride, p_src, src_stride, buf);             \
  PERMUTE_822(p_dst + 4 * dst_stride, dst_stride,                     \
              p_src + 4 * src_stride, src_stride, buf)

#define PERMUTE_841(p_dst, dst_stride, p_src, src_stride, buf)            \
  buf = (*(p_src) << 4 & 0xF0) | (*(p_src + 4 * src_stride)  & 0x0F);     \
  *(p_dst) = (*(p_src) & 0xF0) | (*(p_src + 4 * src_stride) >> 4 & 0x0F); \
  *(p_dst + 4 * dst_stride) = buf
#define PERMUTE_842(p_dst, dst_stride, p_src, src_stride, buf)        \
  PERMUTE_841(p_dst, dst_stride, p_src, src_stride, buf);             \
  PERMUTE_841(p_dst + dst_stride, dst_stride,                         \
              p_src + src_stride, src_stride, buf)
#define PERMUTE_844(p_dst, dst_stride, p_src, src_stride, buf)        \
  PERMUTE_842(p_dst, dst_stride, p_src, src_stride, buf);             \
  PERMUTE_842(p_dst + 2 * dst_stride, dst_stride,                     \
              p_src + 2 * src_stride, src_stride, buf)

static MUSTINLINE void Transpose8x8Bits(
    uint8_t       *p_dst,
    int            dst_stride,
    const uint8_t *p_src,
    int            src_stride) {
  uint8_t buf;
  PERMUTE_814(p_dst, dst_stride, p_src, src_stride, buf);
  PERMUTE_824(p_dst, dst_stride, p_dst, dst_stride, buf);
  PERMUTE_844(p_dst, dst_stride, p_dst, dst_stride, buf);
}

static MUSTINLINE void Transpose8x8BitsInternal(
    uint64_t       *p_dst,
    const uint64_t *p_src) {
  *p_dst = (*p_src << 9  & 0xAA00AA00AA00AA00ll) |
           (*p_src >> 9  & 0x0055005500550055ll) |
           (*p_src       & 0x55AA55AA55AA55AAll);
  *p_dst = (*p_dst << 18 & 0xCCCC0000CCCC0000ll) |
           (*p_dst >> 18 & 0x0000333300003333ll) |
           (*p_dst       & 0x3333CCCC3333CCCCll);
  *p_dst = (*p_dst << 36 & 0xF0F0F0F000000000ll) |
           (*p_dst >> 36 & 0x000000000F0F0F0Fll) |
           (*p_dst       & 0x0F0F0F0FF0F0F0F0ll);
}

#endif // #ifndef MINIMGAPI_SRC_VECTOR_TRANSPOSE_INL_H_INCLUDED
