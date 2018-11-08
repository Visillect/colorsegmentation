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
#ifndef MINIMGAPI_SRC_VECTOR_SSE_TRANSPOSE_INL_H_INCLUDED
#define MINIMGAPI_SRC_VECTOR_SSE_TRANSPOSE_INL_H_INCLUDED

#include <emmintrin.h>
#include <xmmintrin.h>
#include <minbase/crossplat.h>
#include <minutils/smartptr.h>

static MUSTINLINE void Permute0401(
  __m128i       *p_out0,
  __m128i       *p_out1,
  __m128i       *p_out2,
  __m128i       *p_out3,
  const __m128i *p_in0,
  const __m128i *p_in1,
  const __m128i *p_in2,
  const __m128i *p_in3) {
  __m128i mmi0 = _mm_loadu_si128(p_in0);
  __m128i mmi1 = _mm_loadu_si128(p_in1);
  __m128i mmi2 = _mm_loadu_si128(p_in2);
  __m128i mmi3 = _mm_loadu_si128(p_in3);

  __m128i ans_a = _mm_unpacklo_epi8(mmi0, mmi1);
  __m128i ans_b = _mm_unpackhi_epi8(mmi0, mmi1);
  __m128i ans_c = _mm_unpacklo_epi8(mmi2, mmi3);
  __m128i ans_d = _mm_unpackhi_epi8(mmi2, mmi3);

  _mm_storeu_si128(p_out0, _mm_unpacklo_epi16(ans_a, ans_c));
  _mm_storeu_si128(p_out1, _mm_unpackhi_epi16(ans_a, ans_c));
  _mm_storeu_si128(p_out2, _mm_unpacklo_epi16(ans_b, ans_d));
  _mm_storeu_si128(p_out3, _mm_unpackhi_epi16(ans_b, ans_d));
}

static MUSTINLINE void Permute0402(
  __m128i       *p_out0,
  __m128i       *p_out1,
  const __m128i *p_in0,
  const __m128i *p_in1) {
  __m128i mmi0 = _mm_loadu_si128(p_in0);
  __m128i mmi1 = _mm_loadu_si128(p_in1);

  _mm_storeu_si128(p_out0, _mm_unpacklo_epi16(mmi0, mmi1));
  _mm_storeu_si128(p_out1, _mm_unpackhi_epi16(mmi0, mmi1));
}

static MUSTINLINE void Permute1604(
  __m128i       *p_out0,
  __m128i       *p_out1,
  __m128i       *p_out2,
  __m128i       *p_out3,
  const __m128i *p_in0,
  const __m128i *p_in1,
  const __m128i *p_in2,
  const __m128i *p_in3) {
  __m128i mmi0 = _mm_loadu_si128(p_in0);
  __m128i mmi1 = _mm_loadu_si128(p_in1);
  __m128i mmi2 = _mm_loadu_si128(p_in2);
  __m128i mmi3 = _mm_loadu_si128(p_in3);

  __m128i ans_a = _mm_unpacklo_epi32(mmi0, mmi1);
  __m128i ans_b = _mm_unpackhi_epi32(mmi0, mmi1);
  __m128i ans_c = _mm_unpacklo_epi32(mmi2, mmi3);
  __m128i ans_d = _mm_unpackhi_epi32(mmi2, mmi3);

  _mm_storeu_si128(p_out0, _mm_unpacklo_epi64(ans_a, ans_c));
  _mm_storeu_si128(p_out1, _mm_unpackhi_epi64(ans_a, ans_c));
  _mm_storeu_si128(p_out2, _mm_unpacklo_epi64(ans_b, ans_d));
  _mm_storeu_si128(p_out3, _mm_unpackhi_epi64(ans_b, ans_d));
}

static MUSTINLINE void Transpose16x16(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride) {
  const __m128i *p_src = reinterpret_cast<const __m128i *>(p_src_buffer);
  __m128i *p_dst = reinterpret_cast<__m128i *>(p_dst_buffer);
  for (int i = 0; i < 4; ++i) {
    Permute0401(ShiftPtr(p_dst,  0 * dst_stride),
                ShiftPtr(p_dst,  4 * dst_stride),
                ShiftPtr(p_dst,  8 * dst_stride),
                ShiftPtr(p_dst, 12 * dst_stride),
                ShiftPtr(p_src,  0 * src_stride),
                ShiftPtr(p_src,  1 * src_stride),
                ShiftPtr(p_src,  2 * src_stride),
                ShiftPtr(p_src,  3 * src_stride));
    p_src = ShiftPtr(p_src, 4 * src_stride);
    p_dst = ShiftPtr(p_dst, 1 * dst_stride);
  }

  p_dst = reinterpret_cast<__m128i *>(p_dst_buffer);
  for (int i = 0; i < 4; ++i) {
    Permute1604(ShiftPtr(p_dst, 0 * dst_stride),
                ShiftPtr(p_dst, 1 * dst_stride),
                ShiftPtr(p_dst, 2 * dst_stride),
                ShiftPtr(p_dst, 3 * dst_stride),
                ShiftPtr(p_dst, 0 * dst_stride),
                ShiftPtr(p_dst, 1 * dst_stride),
                ShiftPtr(p_dst, 2 * dst_stride),
                ShiftPtr(p_dst, 3 * dst_stride));
    p_dst = ShiftPtr(p_dst, 4 * dst_stride);
  }
}

static MUSTINLINE void Transpose8x8(
    uint16_t       *p_dst_buffer,
    int             dst_stride,
    const uint16_t *p_src_buffer,
    int             src_stride) {
  const __m128i *p_src = reinterpret_cast<const __m128i *>(p_src_buffer);
  __m128i *p_dst = reinterpret_cast<__m128i *>(p_dst_buffer);
  for (int i = 0; i < 4; ++i) {
    Permute0402(p_dst, ShiftPtr(p_dst, 4 * dst_stride),
                p_src, ShiftPtr(p_src, src_stride));
    p_src = ShiftPtr(p_src, 2 * src_stride);
    p_dst = ShiftPtr(p_dst, dst_stride);
  }

  p_dst = reinterpret_cast<__m128i *>(p_dst_buffer);
  for (int i = 0; i < 2; ++i) {
    Permute1604(p_dst,
                ShiftPtr(p_dst, dst_stride),
                ShiftPtr(p_dst, 2 * dst_stride),
                ShiftPtr(p_dst, 3 * dst_stride),
                p_dst,
                ShiftPtr(p_dst, dst_stride),
                ShiftPtr(p_dst, 2 * dst_stride),
                ShiftPtr(p_dst, 3 * dst_stride));
    p_dst = ShiftPtr(p_dst, 4 * dst_stride);
  }
}

static MUSTINLINE void Transpose4x4(
    uint32_t       *p_dst_buffer,
    int             dst_stride,
    const uint32_t *p_src_buffer,
    int             src_stride) {
  const __m128i *p_src = reinterpret_cast<const __m128i *>(p_src_buffer);
  __m128i *p_dst = reinterpret_cast<__m128i *>(p_dst_buffer);
  Permute1604(
    p_dst,
    ShiftPtr(p_dst, dst_stride),
    ShiftPtr(p_dst, 2 * dst_stride),
    ShiftPtr(p_dst, 3 * dst_stride),
    p_src,
    ShiftPtr(p_src, src_stride),
    ShiftPtr(p_src, 2 * src_stride),
    ShiftPtr(p_src, 3 * src_stride));
}

#endif // #ifndef MINIMGAPI_SRC_VECTOR_SSE_TRANSPOSE_INL_H_INCLUDED
