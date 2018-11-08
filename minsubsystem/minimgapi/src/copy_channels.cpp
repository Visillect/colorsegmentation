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

#include <minbase/minresult.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/minimgapi-inl.h>
#include <minimgapi/imgguard.hpp>
#include <minbase/crossplat.h>
#include <minutils/smartptr.h>
#include "vector/copy_channels-inl.h"

template <typename TChannel>
static int DeinterleaveMinImage4To3(
    const MinImg *p_dst_image,
    const MinImg *p_src_image) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  MinImg tmp_image = {};
  PROPAGATE_ERROR(_CloneDimensionedMinImagePrototype(
                     &tmp_image, p_dst_image, p_src_image->channels, AO_EMPTY));
  if (_CompareMinImagePrototypes(p_src_image, &tmp_image))
    return BAD_ARGS;
  if (p_dst_image->channels != 3 || p_src_image->channels != 4)
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_dst_image) == NO_ERRORS)
    return NO_ERRORS;
  if (p_dst_image->addressSpace)
    return NOT_IMPLEMENTED;

  const MinImg *p_work_dst_image = p_dst_image;
  const MinImg *p_work_src_image = p_src_image;

  MinImg unrolled_dst_image = {};
  MinImg unrolled_src_image = {};
  if (_AssureMinImageIsSolid(p_dst_image) == NO_ERRORS &&
      _AssureMinImageIsSolid(p_src_image) == NO_ERRORS) {
    SHOULD_WORK(_UnrollSolidMinImage(&unrolled_dst_image, p_dst_image));
    SHOULD_WORK(_UnrollSolidMinImage(&unrolled_src_image, p_src_image));
    p_work_dst_image = &unrolled_dst_image;
    p_work_src_image = &unrolled_src_image;
  }

  TChannel *p_dst_line = reinterpret_cast<TChannel *>(
                                         _GetMinImageLine(p_work_dst_image, 0));
  const TChannel *p_src_line = reinterpret_cast<TChannel *>(
                                         _GetMinImageLine(p_work_src_image, 0));
  if (!p_dst_line || !p_src_line)
    return INTERNAL_ERROR;
  for (int y = 0; y < p_work_dst_image->height; ++y) {
    vector_deinterleave_4to3(p_dst_line, p_src_line, p_work_dst_image->width);
    p_dst_line = ShiftPtr(p_dst_line, p_work_dst_image->stride);
    p_src_line = ShiftPtr(p_src_line, p_work_src_image->stride);
  }
  return NO_ERRORS;
}

MINIMGAPI_API int CopyMinImageChannels(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    const int    *p_dst_channels,
    const int    *p_src_channels,
    int           num_channels) {
  if (!p_dst_channels || !p_src_channels || num_channels < 0)
    return BAD_ARGS;
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  MinImg tmp_image = {};
  PROPAGATE_ERROR(_CloneDimensionedMinImagePrototype(
                     &tmp_image, p_dst_image, p_src_image->channels, AO_EMPTY));
  if (_CompareMinImagePrototypes(p_src_image, &tmp_image))
    return BAD_ARGS;
  if (_AssureMinImageIsEmpty(p_dst_image) == NO_ERRORS || !num_channels)
    return NO_ERRORS;

  if (num_channels == 3 &&
      p_dst_image->channels == 3 && p_src_image->channels == 4 &&
      p_dst_channels[0] == 0 && p_src_channels[0] == 0 &&
      p_dst_channels[1] == 1 && p_src_channels[1] == 1 &&
      p_dst_channels[2] == 2 && p_src_channels[2] == 2) {
    int res = NOT_IMPLEMENTED;
    switch (p_dst_image->channelDepth) {
      case 1:
        res = DeinterleaveMinImage4To3<uint8_t>(p_dst_image, p_src_image);
        break;
      case 2:
        res = DeinterleaveMinImage4To3<uint16_t>(p_dst_image, p_src_image);
        break;
      case 4:
        res = DeinterleaveMinImage4To3<uint32_t>(p_dst_image, p_src_image);
        break;
      case 8:
        res = DeinterleaveMinImage4To3<uint64_t>(p_dst_image, p_src_image);
        break;
      default:
        res = NOT_IMPLEMENTED;
    }
    if (res == NO_ERRORS)
      return NO_ERRORS;
  }

  MinImg unfolded_dst_image = {};
  PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_dst_image, p_dst_image));
  DECLARE_GUARDED_MINIMG(transfolded_dst_image);
  PROPAGATE_ERROR(_CloneTransposedMinImagePrototype(&transfolded_dst_image,
                                                   &unfolded_dst_image));

  MinImg unfolded_src_image = {};
  PROPAGATE_ERROR(_UnfoldMinImageChannels(&unfolded_src_image, p_src_image));
  DECLARE_GUARDED_MINIMG(transfolded_src_image);
  PROPAGATE_ERROR(_CloneTransposedMinImagePrototype(&transfolded_src_image,
                                                    &unfolded_src_image));
  PROPAGATE_ERROR(TransposeMinImage(&transfolded_src_image,
                                    &unfolded_src_image));

  int used_dst_channels = 0;
  for (int i = 0; i < num_channels; ++i) {
    int dst_channel = p_dst_channels[i];
    int src_channel = p_src_channels[i];

    if (dst_channel < 0 || dst_channel > p_dst_image->channels)
      return BAD_ARGS;
    if (src_channel < 0 || src_channel > p_src_image->channels)
      return BAD_ARGS;

    ++used_dst_channels;
    for (int j = 0; j < i; ++j)
      if (p_dst_channels[j] == p_dst_channels[i]) {
        --used_dst_channels;
        break;
      }
  }

  if (used_dst_channels < p_dst_image->channels)
    PROPAGATE_ERROR(TransposeMinImage(&transfolded_dst_image,
                                      &unfolded_dst_image));

  for (int i = 0; i < num_channels; ++i) {
    int dst_channel = p_dst_channels[i];
    int src_channel = p_src_channels[i];

    MinImg dst_channel_image = {};
    SHOULD_WORK(_SliceMinImageVertically(&dst_channel_image,
                   &transfolded_dst_image, dst_channel, p_dst_image->channels));
    MinImg src_channel_image = {};
    SHOULD_WORK(_SliceMinImageVertically(&src_channel_image,
                   &transfolded_src_image, src_channel, p_src_image->channels));

    PROPAGATE_ERROR(CopyMinImage(&dst_channel_image, &src_channel_image));
  }

  return TransposeMinImage(&unfolded_dst_image, &transfolded_dst_image);
}
