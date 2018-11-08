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
#ifndef MINIMGAPI_SRC_VECTOR_NEON_COPY_CHANNELS_INL_H_INCLUDED
#define MINIMGAPI_SRC_VECTOR_NEON_COPY_CHANNELS_INL_H_INCLUDED

#include <arm_neon.h>
#include <minbase/crossplat.h>
#include <minbase/mintyp.h>

template<> STATIC_SPECIAL MUSTINLINE void vector_deinterleave_4to3(
    uint8_t       *p_dst,
    const uint8_t *p_src,
    int            len) {
  const uint8_t *ps = p_src;
  __builtin_prefetch(ps, 0);
  const int effective_len = len & (~0x0FU);
  int i = 0;
  uint8_t *pd = p_dst;
  __builtin_prefetch(pd, 1);
  for (; i < effective_len; i += 16, ps += 64, pd += 48) {
    const uint8x16x4_t pix4 = vld4q_u8(ps);
    __builtin_prefetch(ps + 64, 0);
    uint8x16x3_t pix3;
    pix3.val[0] = pix4.val[0];
    pix3.val[1] = pix4.val[1];
    pix3.val[2] = pix4.val[2];
    vst3q_u8(pd, pix3);
    __builtin_prefetch(pd + 48, 1);
  }
  for (; i < len; ++i, ps += 4, pd += 3) {
    pd[0] = ps[0];
    pd[1] = ps[1];
    pd[2] = ps[2];
  }
}

#endif // #ifndef MINIMGAPI_SRC_VECTOR_NEON_COPY_CHANNELS_INL_H_INCLUDED
