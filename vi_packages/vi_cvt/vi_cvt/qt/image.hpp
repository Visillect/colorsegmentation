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
#include <QImage>
#include <minbase/minimg.h>
#include <vi_cvt/std/exceptions.hpp>
#include <vi_cvt/std/exception_macros.hpp>
#include <minbase/minresult.h>
#include <minimgapi/imgguard.hpp>


namespace vi { namespace cvt { namespace qt {

inline QImage as_qimage(MinImg const* src, bool is_const = false) {
  if (!src) {
    return QImage();
  }

  QImage::Format format = QImage::Format_Invalid;
  if (src->format != FMT_UINT) {
    throw MinResultException(BAD_ARGS,
                             "MinImg format " + std::to_string(src->format)
                             + " is not supported for QImage (only FMT_UINT is)",
                             "as_qimage", __FILE__, __LINE__);
  }
  if (src->channelDepth != 1) {
    throw MinResultException(BAD_ARGS,
                             "MinImg depth " + std::to_string(src->channelDepth)
                             + " is not supported for QImage (only 1 is)",
                             "as_qimage", __FILE__, __LINE__);
  }
  if (src->channels == 1) {
    format = QImage::Format_Grayscale8;
  } else if (src->channels == 3) {
    format = QImage::Format_RGB888;
  } else {
    throw MinResultException(BAD_ARGS,
                             "MinImg channel number " + std::to_string(src->channels)
                             + " is not supported for QImage (only 1 and 3 are)",
                             "as_qimage", __FILE__, __LINE__);
  }

  if (src->stride <= 0) {
    throw MinResultException(BAD_ARGS,
                             "MinImg stride " + std::to_string(src->stride)
                             + " is not supported for QImage",
                             "as_qimage", __FILE__, __LINE__);
  }

  if (is_const) {
    return QImage(static_cast<uint8_t const*>(src->pScan0), src->width, src->height, src->stride, format);
  } else {
    return QImage(static_cast<uint8_t *>(src->pScan0), src->width, src->height, src->stride, format);
  }
}

inline MinImg as_minimg(QImage & src) {
  MinImg dst = {0};
  if (src.isNull()) {
    return dst;
  }

  int channels = 0;
  switch(src.format())
  {
    case QImage::Format_Grayscale8:
      channels = 1;
      break;
    case QImage::Format_RGB888:
      channels = 3;
      break;
    default:
      throw MinResultException(BAD_ARGS, "Unsupported format", "to_minimg", __FILE__, __LINE__);
  }
  THROW_ON_MINERR(NewMinImagePrototype(&dst, src.width(), src.height(), channels, TYP_UINT8, 0, AO_EMPTY));
  dst.stride = src.bytesPerLine();
  dst.pScan0 = src.scanLine(0);
  return dst;
}

}}} // ns vi::cvt::qt
