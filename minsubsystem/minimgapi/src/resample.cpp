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
#include <cmath>
#include <minbase/minresult.h>
#include <minutils/smartptr.h>
#include <minbase/crossplat.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/minimgapi-inl.h>
#include <minimgapi/imgguard.hpp>
#include "bitcpy.h"

#if defined(MINSTOPWATCH_ENABLED)
#  include <minstopwatch/stopwatch.hpp>
DECLARE_MINSTOPWATCH(gsw_ResampleMinImage, "ResampleMinImage");
#endif // defined(MINSTOPWATCH_ENABLED)

template<typename TChunk>
static int ChunkedResampleMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    double        x_phase,
    double        y_phase) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  NEED_NO_ERRORS(_CompareMinImagePixels(p_dst_image, p_src_image));

  x_phase -= floor(x_phase);
  y_phase -= floor(y_phase);

  int chunk_size = sizeof(TChunk) << 3;
  int pixel_size = _GetMinImageBitsPerPixel(p_dst_image);
  if (pixel_size % chunk_size)
    return BAD_ARGS;
  int chunks_per_pixel = pixel_size / chunk_size;
  int chunks_per_line = chunks_per_pixel * p_dst_image->width;

  if (p_dst_image->addressSpace != p_src_image->addressSpace)
    return BAD_ARGS;
  if (p_dst_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  double x_qoutient = p_src_image->width / (p_dst_image->width + 0.);
  scoped_cpp_array<int> src_indices_by_dst(new int[chunks_per_line]);
  for (int dst_x = 0; dst_x < p_dst_image->width; ++dst_x) {
    int *dst_pixel = &src_indices_by_dst[dst_x * chunks_per_pixel];
    int src_x = static_cast<int>((dst_x + x_phase) * x_qoutient);
    int src_chunk_x = src_x * chunks_per_pixel;
    for (int chunk = 0; chunk < chunks_per_pixel; ++chunk)
      dst_pixel[chunk] = src_chunk_x + chunk;
  }

  double y_quotient = p_src_image->height / (p_dst_image->height + 0.);
  int byte_line_width = _GetMinImageBytesPerLine(p_dst_image);
  int last_line_done = -1;
  TChunk *p_dst_line = reinterpret_cast<TChunk *>(
                                              _GetMinImageLine(p_dst_image, 0));
  if (!p_dst_line)
    return INTERNAL_ERROR;
  for (int dst_y = 0; dst_y < p_dst_image->height; ++dst_y) {
    int src_y = static_cast<int>((dst_y + y_phase) * y_quotient);
    if (src_y == last_line_done)
      ::memcpy(p_dst_line, ShiftPtr(p_dst_line, -p_dst_image->stride),
                                                               byte_line_width);
    else {
      const TChunk *p_src_line = reinterpret_cast<TChunk *>(
                                          _GetMinImageLine(p_src_image, src_y));
      if (!p_src_line)
        return INTERNAL_ERROR;
      for (int dst_chunk_x = 0; dst_chunk_x < chunks_per_line; ++dst_chunk_x)
        p_dst_line[dst_chunk_x] = p_src_line[src_indices_by_dst[dst_chunk_x]];
    }
    p_dst_line = ShiftPtr(p_dst_line, p_dst_image->stride);
  }

  return NO_ERRORS;
}

static int ResampleNBytesImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    double        x_phase,
    double        y_phase,
    int           element_byte_size) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  NEED_NO_ERRORS(_CompareMinImagePixels(p_dst_image, p_src_image));
  if (element_byte_size < 1)
    return BAD_ARGS;

  x_phase -= floor(x_phase);
  y_phase -= floor(y_phase);

  if (p_src_image->addressSpace != p_dst_image->addressSpace)
    return BAD_ARGS;
  if (p_src_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  double x_quotient = p_src_image->width / (p_dst_image->width + 0.);
  scoped_cpp_array<int> src_indices_by_dst(new int[p_dst_image->width]);
  for (int dst_x = 0; dst_x < p_dst_image->width; ++dst_x) {
    int src_x = static_cast<int>((dst_x + x_phase) * x_quotient);
    src_indices_by_dst[dst_x] = src_x * element_byte_size;
  }

  double y_quotient = p_src_image->height / (p_dst_image->height + 0.);
  int byte_line_width = _GetMinImageBytesPerLine(p_dst_image);
  int last_line_done = -1;
  uint8_t *p_dst_line = _GetMinImageLine(p_dst_image, 0);
  if (!p_dst_line)
    return INTERNAL_ERROR;
  for (int dst_y = 0; dst_y < p_dst_image->height; ++dst_y) {
    int src_y = static_cast<int>((dst_y + y_phase) * y_quotient);
    if (dst_y == last_line_done)
      ::memcpy(p_dst_line, p_dst_line - p_dst_image->stride, byte_line_width);
    else {
      const uint8_t *p_src_line = _GetMinImageLine(p_src_image, src_y);
      if (!p_src_line)
        return INTERNAL_ERROR;
      for (int dst_x = 0; dst_x < p_dst_image->width; ++dst_x)
        ::memcpy(p_dst_line + dst_x * element_byte_size,
               p_src_line + src_indices_by_dst[dst_x] * element_byte_size,
               element_byte_size);
    }
    p_dst_line += p_dst_image->stride;
  }

  return NO_ERRORS;
}

static int ResampleNBitsImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    double        x_phase,
    double        y_phase,
    int           element_bit_size) {
  NEED_NO_ERRORS(_AssureMinImageFits(p_dst_image, TYP_UINT1));
  NEED_NO_ERRORS(_AssureMinImageFits(p_src_image, TYP_UINT1));
  NEED_NO_ERRORS(_CompareMinImagePixels(p_dst_image, p_src_image));
  if (element_bit_size < 1)
    return BAD_ARGS;

  x_phase -= floor(x_phase);
  y_phase -= floor(y_phase);

  if (p_src_image->addressSpace != p_dst_image->addressSpace)
    return BAD_ARGS;
  if (p_src_image->addressSpace != 0)
    return NOT_IMPLEMENTED;

  double x_quotient = p_src_image->width / (p_dst_image->width + 0.);
  scoped_cpp_array<int> src_indices_by_dst(
                           new int[p_dst_image->width * p_dst_image->channels]);
  for (int dst_x = 0; dst_x < p_dst_image->width; ++dst_x) {
    int *dst_pixel = &src_indices_by_dst[dst_x * p_dst_image->channels];
    int src_x = static_cast<int>((dst_x + x_phase) * x_quotient);
    int src_bit_x = src_x * p_src_image->channels;
    for (int channel = 0; channel < p_dst_image->channels; ++channel)
      dst_pixel[channel] = src_bit_x + channel;
  }

  double y_quotient = p_src_image->height / (p_dst_image->height + 0.);
  int bit_line_width = p_dst_image->width * element_bit_size;
  int last_line_done = -1;
  uint8_t *p_dst_line = _GetMinImageLine(p_dst_image, 0);
  if (!p_dst_line)
    return INTERNAL_ERROR;
  for (int dst_y = 0; dst_y < p_dst_image->height; ++dst_y) {
    int src_y = static_cast<int>((dst_y + y_phase) * y_quotient);
    if (src_y == last_line_done)
      bitcpy(p_dst_line, 0, p_dst_line - p_dst_image->stride, 0,
                                                                bit_line_width);
    else {
      const uint8_t *p_src_line = _GetMinImageLine(p_src_image, src_y);
      if (!p_src_line)
        return INTERNAL_ERROR;
      for (int dst_x = 0; dst_x < p_dst_image->width; ++dst_x)
        bitcpy(p_dst_line, dst_x                     * element_bit_size,
               p_src_line, src_indices_by_dst[dst_x] * element_bit_size,
               element_bit_size);
    }
    p_dst_line += p_dst_image->stride;
  }

  return NO_ERRORS;
}

MINIMGAPI_API int ResampleMinImage(
    const MinImg *p_dst_image,
    const MinImg *p_src_image,
    double        x_phase,
    double        y_phase) {
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_src_image));
  PROPAGATE_ERROR(_AssureMinImageIsValid(p_dst_image));
  NEED_NO_ERRORS(_CompareMinImagePixels(p_dst_image, p_src_image));

  uint32_t tangling = 0;
  PROPAGATE_ERROR(CheckMinImagesTangle(&tangling, p_dst_image, p_src_image));

  bool x_upsample = p_dst_image->width > p_src_image->width;
  bool y_upsample = p_dst_image->height > p_src_image->height;
  bool upsample = x_upsample || y_upsample;

  typedef enum {
    DTM_DO_NOTHING = 0,
    DTM_FLIP_VERTICALLY = 1,
    DTM_COPY_SOURCE = 2
  } DetangleMethod;
  DetangleMethod detangle = DTM_DO_NOTHING;

  if ((~tangling & TCR_FORWARD_PASS_POSSIBLE) ||
      (x_upsample && (~tangling & TCR_INDEPENDENT_LINES)) ||
      (y_upsample && (~tangling & TCR_BACKWARD_PASS_POSSIBLE)))
    detangle = DTM_COPY_SOURCE;

  if (!y_upsample && (~tangling & TCR_FORWARD_PASS_POSSIBLE) &&
      (tangling & TCR_BACKWARD_PASS_POSSIBLE) &&
      (!x_upsample && (tangling & TCR_INDEPENDENT_LINES)))
    detangle = DTM_FLIP_VERTICALLY;

  const MinImg *p_work_dst_image = p_dst_image;
  const MinImg *p_work_src_image = p_src_image;

  MinImg dst_image = {0};
  MinImg src_image = {0};
  DECLARE_GUARDED_MINIMG(tmp_image);
  if (detangle == DTM_FLIP_VERTICALLY) {
    SHOULD_WORK(_FlipMinImageVertically(&dst_image, p_dst_image));
    SHOULD_WORK(_FlipMinImageVertically(&src_image, p_src_image));
    p_work_src_image = &src_image;
    p_work_dst_image = &dst_image;
    x_phase = 1.0 - x_phase;
    y_phase = 1.0 - y_phase;
  }
  if (detangle == DTM_COPY_SOURCE) {
    PROPAGATE_ERROR(_CloneMinImagePrototype(&tmp_image, p_src_image));
    SHOULD_WORK(CopyMinImage(&tmp_image, p_src_image));
    p_work_src_image = &tmp_image;
  }

  int bits_per_pixel = _GetMinImageBitsPerPixel(p_work_dst_image);
  if (bits_per_pixel & 0x07U)
    return ResampleNBitsImage(p_work_dst_image, p_work_src_image,
                              x_phase, y_phase, bits_per_pixel);

  int bytes_per_pixel = bits_per_pixel >> 3;
  if (p_work_dst_image->channels == 3)
    bytes_per_pixel /= 3;

  switch (bytes_per_pixel) {
  case 1:
    return ChunkedResampleMinImage<uint8_t>(p_work_dst_image, p_work_src_image,
                                                              x_phase, y_phase);
  case 2:
    return ChunkedResampleMinImage<uint16_t>(p_work_dst_image, p_work_src_image,
                                                              x_phase, y_phase);
  case 4:
    return ChunkedResampleMinImage<uint32_t>(p_work_dst_image, p_work_src_image,
                                                              x_phase, y_phase);
  case 8:
    return ChunkedResampleMinImage<uint64_t>(p_work_dst_image, p_work_src_image,
                                                              x_phase, y_phase);
  default:
    return ResampleNBytesImage(p_work_dst_image, p_work_src_image,
                               x_phase, y_phase, bytes_per_pixel);
  }

  return INTERNAL_ERROR;
}
