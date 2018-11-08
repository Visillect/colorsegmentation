/*
Copyright (c) 2012-2018, Visillect Service LLC. All rights reserved.

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

#include <minbase/crossplat.h>
#include <minbase/minimg.h>
#include <minbase/minresult.h>

THIRDPARTY_INCLUDES_BEGIN
#include <opencv2/core/core.hpp>
THIRDPARTY_INCLUDES_END

#include <vi_cvt/std/exceptions.hpp>


namespace vi { namespace cvt { namespace ocv {


inline MinImg as_minimg(cv::Mat const& src)
{
  MinImg dst = {0};

  dst.width = src.cols;
  dst.height = src.rows;
  dst.channels = static_cast<int32_t>(src.elemSize() / src.elemSize1());
  dst.channelDepth = static_cast<int32_t>(src.elemSize1());

  switch (src.depth())
  {
  case CV_8U:
  case CV_16U:
    dst.format = FMT_UINT;
    break;

  case CV_8S:
  case CV_16S:
  case CV_32S:
    dst.format = FMT_INT;
    break;

  case CV_32F:
  case CV_64F:
    dst.format = FMT_REAL;
    break;

  default:
    throw MinResultException(NOT_IMPLEMENTED,
                             "Unknown cv::Mat depth " + std::to_string(src.depth()),
                             "as_minimg", __FILE__, __LINE__);
  }

  dst.stride = static_cast<int32_t>(src.step);
  dst.pScan0 = src.data;

  return dst;
}


inline cv::Mat as_cvmat(MinImg const* src)
{
  if (!src) {
    return cv::Mat();
  }

  int cvMatType = 0;
  switch (src->format)
  {
  case FMT_UINT:
    if (src->channelDepth == 1)
      cvMatType = CV_8U;
    else if (src->channelDepth == 2)
      cvMatType = CV_16U;
    else
      throw MinResultException(NOT_IMPLEMENTED,
                               "MinImg depth UINT" + std::to_string(src->channelDepth)
                               + " is not supported for cv::Mat",
                               "as_cvmat", __FILE__, __LINE__);
    break;

  case FMT_INT:
    if (src->channelDepth == 1)
      cvMatType = CV_8S;
    else if (src->channelDepth == 2)
      cvMatType = CV_16S;
    else if (src->channelDepth == 4)
      cvMatType = CV_32S;
    else
      throw MinResultException(NOT_IMPLEMENTED,
                               "MinImg depth INT" + std::to_string(src->channelDepth)
                               + " is not supported for cv::Mat",
                               "as_cvmat", __FILE__, __LINE__);
    break;

  case FMT_REAL:
    if (src->channelDepth == 4)
      cvMatType = CV_32F;
    else if (src->channelDepth == 8)
      cvMatType = CV_64F;
    else
      throw MinResultException(NOT_IMPLEMENTED,
                               "MinImg depth REAL" + std::to_string(src->channelDepth)
                               + " is not supported for cv::Mat",
                               "as_cvmat", __FILE__, __LINE__);
    break;

  default:
    throw MinResultException(BAD_ARGS,
                             "MinImg format " + std::to_string(src->format)
                             + " is not supported for cv::Mat",
                             "as_cvmat", __FILE__, __LINE__);
  }

  if (src->channels < 1 || src->channels > 4) {
    throw MinResultException(BAD_ARGS,
                             "MinImg channels " + std::to_string(src->channels)
                             + " is not supported for cv::Mat",
                             "as_cvmat", __FILE__, __LINE__);
  }

  if (src->stride <= 0) {
    throw MinResultException(BAD_ARGS,
                             "MinImg stride " + std::to_string(src->stride)
                             + " is not supported for cv::Mat",
                             "as_cvmat", __FILE__, __LINE__);
  }

  return cv::Mat(src->height, src->width, CV_MAKETYPE(cvMatType, src->channels), src->pScan0, src->stride);
}

}}} // ns vi::cvt::ocv
