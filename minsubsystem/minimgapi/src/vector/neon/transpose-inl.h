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
#ifndef MINIMGAPI_SRC_VECTOR_NEON_TRANSPOSE_INL_H_INCLUDED
#define MINIMGAPI_SRC_VECTOR_NEON_TRANSPOSE_INL_H_INCLUDED

#include <arm_neon.h>
#include <minbase/crossplat.h>
#include <minutils/smartptr.h>

static MUSTINLINE
void Transpose16x16(
    uint8_t       *p_dst_buffer,
    int            dst_stride,
    const uint8_t *p_src_buffer,
    int            src_stride) {
  uint8x16x2_t t0 = vtrnq_u8(vld1q_u8(p_src_buffer +  0 * src_stride),
                             vld1q_u8(p_src_buffer +  1 * src_stride));
  uint8x16x2_t t1 = vtrnq_u8(vld1q_u8(p_src_buffer +  2 * src_stride),
                             vld1q_u8(p_src_buffer +  3 * src_stride));
  uint8x16x2_t t2 = vtrnq_u8(vld1q_u8(p_src_buffer +  4 * src_stride),
                             vld1q_u8(p_src_buffer +  5 * src_stride));
  uint8x16x2_t t3 = vtrnq_u8(vld1q_u8(p_src_buffer +  6 * src_stride),
                             vld1q_u8(p_src_buffer +  7 * src_stride));
  uint8x16x2_t t4 = vtrnq_u8(vld1q_u8(p_src_buffer +  8 * src_stride),
                             vld1q_u8(p_src_buffer +  9 * src_stride));
  uint8x16x2_t t5 = vtrnq_u8(vld1q_u8(p_src_buffer + 10 * src_stride),
                             vld1q_u8(p_src_buffer + 11 * src_stride));
  uint8x16x2_t t6 = vtrnq_u8(vld1q_u8(p_src_buffer + 12 * src_stride),
                             vld1q_u8(p_src_buffer + 13 * src_stride));
  uint8x16x2_t t7 = vtrnq_u8(vld1q_u8(p_src_buffer + 14 * src_stride),
                             vld1q_u8(p_src_buffer + 15 * src_stride));

  uint16x8x2_t x0 = vtrnq_u16(vreinterpretq_u16_u8(t0.val[0]),
                              vreinterpretq_u16_u8(t1.val[0]));
  uint16x8x2_t x1 = vtrnq_u16(vreinterpretq_u16_u8(t0.val[1]),
                              vreinterpretq_u16_u8(t1.val[1]));
  uint16x8x2_t x2 = vtrnq_u16(vreinterpretq_u16_u8(t2.val[0]),
                              vreinterpretq_u16_u8(t3.val[0]));
  uint16x8x2_t x3 = vtrnq_u16(vreinterpretq_u16_u8(t2.val[1]),
                              vreinterpretq_u16_u8(t3.val[1]));
  uint16x8x2_t x4 = vtrnq_u16(vreinterpretq_u16_u8(t4.val[0]),
                              vreinterpretq_u16_u8(t5.val[0]));
  uint16x8x2_t x5 = vtrnq_u16(vreinterpretq_u16_u8(t4.val[1]),
                              vreinterpretq_u16_u8(t5.val[1]));
  uint16x8x2_t x6 = vtrnq_u16(vreinterpretq_u16_u8(t6.val[0]),
                              vreinterpretq_u16_u8(t7.val[0]));
  uint16x8x2_t x7 = vtrnq_u16(vreinterpretq_u16_u8(t6.val[1]),
                              vreinterpretq_u16_u8(t7.val[1]));

  uint32x4x2_t z0 = vtrnq_u32(vreinterpretq_u32_u16(x0.val[0]),
                              vreinterpretq_u32_u16(x2.val[0]));
  uint32x4x2_t z1 = vtrnq_u32(vreinterpretq_u32_u16(x0.val[1]),
                              vreinterpretq_u32_u16(x2.val[1]));
  uint32x4x2_t z2 = vtrnq_u32(vreinterpretq_u32_u16(x1.val[0]),
                              vreinterpretq_u32_u16(x3.val[0]));
  uint32x4x2_t z3 = vtrnq_u32(vreinterpretq_u32_u16(x1.val[1]),
                              vreinterpretq_u32_u16(x3.val[1]));
  uint32x4x2_t z4 = vtrnq_u32(vreinterpretq_u32_u16(x4.val[0]),
                              vreinterpretq_u32_u16(x6.val[0]));
  uint32x4x2_t z5 = vtrnq_u32(vreinterpretq_u32_u16(x4.val[1]),
                              vreinterpretq_u32_u16(x6.val[1]));
  uint32x4x2_t z6 = vtrnq_u32(vreinterpretq_u32_u16(x5.val[0]),
                              vreinterpretq_u32_u16(x7.val[0]));
  uint32x4x2_t z7 = vtrnq_u32(vreinterpretq_u32_u16(x5.val[1]),
                              vreinterpretq_u32_u16(x7.val[1]));

  vst1q_u8(p_dst_buffer +  0 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z0.val[0]), vget_low_u32(z4.val[0]))));
  vst1q_u8(p_dst_buffer +  1 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z2.val[0]), vget_low_u32(z6.val[0]))));
  vst1q_u8(p_dst_buffer +  2 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z1.val[0]), vget_low_u32(z5.val[0]))));
  vst1q_u8(p_dst_buffer +  3 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z3.val[0]), vget_low_u32(z7.val[0]))));
  vst1q_u8(p_dst_buffer +  4 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z0.val[1]), vget_low_u32(z4.val[1]))));
  vst1q_u8(p_dst_buffer +  5 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z2.val[1]), vget_low_u32(z6.val[1]))));
  vst1q_u8(p_dst_buffer +  6 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z1.val[1]), vget_low_u32(z5.val[1]))));
  vst1q_u8(p_dst_buffer +  7 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_low_u32(z3.val[1]), vget_low_u32(z7.val[1]))));
  vst1q_u8(p_dst_buffer +  8 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z0.val[0]), vget_high_u32(z4.val[0]))));
  vst1q_u8(p_dst_buffer +  9 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z2.val[0]), vget_high_u32(z6.val[0]))));
  vst1q_u8(p_dst_buffer + 10 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z1.val[0]), vget_high_u32(z5.val[0]))));
  vst1q_u8(p_dst_buffer + 11 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z3.val[0]), vget_high_u32(z7.val[0]))));
  vst1q_u8(p_dst_buffer + 12 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z0.val[1]), vget_high_u32(z4.val[1]))));
  vst1q_u8(p_dst_buffer + 13 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z2.val[1]), vget_high_u32(z6.val[1]))));
  vst1q_u8(p_dst_buffer + 14 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z1.val[1]), vget_high_u32(z5.val[1]))));
  vst1q_u8(p_dst_buffer + 15 * dst_stride, vreinterpretq_u8_u32(
           vcombine_u32(vget_high_u32(z3.val[1]), vget_high_u32(z7.val[1]))));
}

static MUSTINLINE void Transpose8x8(
    uint16_t       *p_dst_buffer,
    int             dst_stride,
    const uint16_t *p_src_buffer,
    int             src_stride) {
  uint16x8x2_t t0 = vtrnq_u16(
    vld1q_u16(ShiftPtr(p_src_buffer, 0 * src_stride)),
    vld1q_u16(ShiftPtr(p_src_buffer, 1 * src_stride)));
  uint16x8x2_t t1 = vtrnq_u16(
    vld1q_u16(ShiftPtr(p_src_buffer, 2 * src_stride)),
    vld1q_u16(ShiftPtr(p_src_buffer, 3 * src_stride)));
  uint16x8x2_t t2 = vtrnq_u16(
    vld1q_u16(ShiftPtr(p_src_buffer, 4 * src_stride)),
    vld1q_u16(ShiftPtr(p_src_buffer, 5 * src_stride)));
  uint16x8x2_t t3 = vtrnq_u16(
    vld1q_u16(ShiftPtr(p_src_buffer, 6 * src_stride)),
    vld1q_u16(ShiftPtr(p_src_buffer, 7 * src_stride)));
  uint32x4x2_t x0 = vtrnq_u32(vreinterpretq_u32_u16(t0.val[0]),
                              vreinterpretq_u32_u16(t1.val[0]));
  uint32x4x2_t x1 = vtrnq_u32(vreinterpretq_u32_u16(t2.val[0]),
                              vreinterpretq_u32_u16(t3.val[0]));
  uint32x4x2_t x2 = vtrnq_u32(vreinterpretq_u32_u16(t0.val[1]),
                              vreinterpretq_u32_u16(t1.val[1]));
  uint32x4x2_t x3 = vtrnq_u32(vreinterpretq_u32_u16(t2.val[1]),
                              vreinterpretq_u32_u16(t3.val[1]));

  vst1q_u16(ShiftPtr(p_dst_buffer, 0 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_low_u32(x0.val[0]), vget_low_u32(x1.val[0]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 1 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_low_u32(x2.val[0]), vget_low_u32(x3.val[0]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 2 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_low_u32(x0.val[1]), vget_low_u32(x1.val[1]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 3 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_low_u32(x2.val[1]), vget_low_u32(x3.val[1]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 4 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_high_u32(x0.val[0]), vget_high_u32(x1.val[0]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 5 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_high_u32(x2.val[0]), vget_high_u32(x3.val[0]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 6 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_high_u32(x0.val[1]), vget_high_u32(x1.val[1]))));
  vst1q_u16(ShiftPtr(p_dst_buffer, 7 * dst_stride), vreinterpretq_u16_u32(
    vcombine_u32(vget_high_u32(x2.val[1]), vget_high_u32(x3.val[1]))));
}

static MUSTINLINE void Transpose4x4(
    uint32_t       *p_dst_buffer,
    int             dst_stride,
    const uint32_t *p_src_buffer,
    int             src_stride) {
  const int src_block_stride = 16;
  const int dst_block_stride = dst_stride * 4;
  const uint32_t *ps0 = p_src_buffer;
  uint32x4_t s0 = vld1q_u32(ps0);
  __builtin_prefetch(ShiftPtr(ps0, src_block_stride), 0);
  const uint32_t *ps1 = ShiftPtr(ps0, src_stride);
  uint32x4_t s1 = vld1q_u32(ps1);
  __builtin_prefetch(ShiftPtr(ps1, src_block_stride), 0);
  uint32x4x2_t t0 = vtrnq_u32(s0, s1);
  const uint32_t *ps2 = ShiftPtr(ps1, src_stride);
  uint32x4_t s2 = vld1q_u32(ps2);
  __builtin_prefetch(ShiftPtr(ps2, src_block_stride), 0);
  const uint32_t *ps3 = ShiftPtr(ps2, src_stride);
  uint32x4_t s3 = vld1q_u32(ps3);
  __builtin_prefetch(ShiftPtr(ps3, src_block_stride), 0);
  uint32x4x2_t t1 = vtrnq_u32(s2, s3);

  uint32x4_t d0 = vcombine_u32(vget_low_u32(t0.val[0]),
                               vget_low_u32(t1.val[0]));
  uint32_t *pd0 = p_dst_buffer;
  vst1q_u32(pd0, d0);
  __builtin_prefetch(ShiftPtr(pd0, dst_block_stride), 1);
  uint32x4_t d1 = vcombine_u32(vget_low_u32(t0.val[1]),
                               vget_low_u32(t1.val[1]));
  uint32_t *pd1 = ShiftPtr(pd0, dst_stride);
  vst1q_u32(pd1, d1);
  __builtin_prefetch(ShiftPtr(pd1, dst_block_stride), 1);
  uint32x4_t d2 = vcombine_u32(vget_high_u32(t0.val[0]),
                               vget_high_u32(t1.val[0]));
  uint32_t *pd2 = ShiftPtr(pd1, dst_stride);
  vst1q_u32(pd2, d2);
  __builtin_prefetch(ShiftPtr(pd2, dst_block_stride), 1);
  uint32x4_t d3 = vcombine_u32(vget_high_u32(t0.val[1]),
                               vget_high_u32(t1.val[1]));
  uint32_t *pd3 = ShiftPtr(pd2, dst_stride);
  vst1q_u32(pd3, d3);
  __builtin_prefetch(ShiftPtr(pd3, dst_block_stride), 1);
}

#endif // #ifndef MINIMGAPI_SRC_VECTOR_NEON_TRANSPOSE_INL_H_INCLUDED
