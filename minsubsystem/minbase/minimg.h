/*

Copyright (c) 2016, Smart Engines Limited. All rights reserved.

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

/**
 * @file   minimgbase.h
 * @brief  Definition of a low-level representation of a bitmap image.
 */

#pragma once
#ifndef MINSBASE_MINIMG_H_INCLUDED
#define MINSBASE_MINIMG_H_INCLUDED

#include <minbase/mintyp.h>

/**
 * @defgroup MinBase_MinImg Image Representation
 * @brief    The module specifies the image representation format. Every
 *           function in the library expects an input image in the form of @c
 *           MinImg object. @c MinImg is a cross-platform open-source container.
 *           The advantages of this container are the using minimal number of
 *           fields needed to represent the bitmap image and the easy way to
 *           cast it to other standard and popular view (for instance, Windows
 *           DIB, GDI+ BitmapData, Intel/OpenCV IplImage).
 */

/**
 * @brief   A low-level universal representation of a bitmap image.
 * @details The struct @c MinImg represents a 2D dense numerical with additional
 *          fields needed for image representations (format and channel number).
 *          The struct @c MinImg allows to describe single-channel and multi-
 *          channel images in a wide range of different image types. Herewith,
 *          the format of the image is specified by two values: depth of the
 *          channel (see @c MinImg::channelDepth) and channel element format
 *          (see @c MinImg::format). To represent a binary image you should set
 *          @c MinImg::format to @c #FMT_UINT and @c MinImg::channelDepth to 0.
 * @ingroup MinBase_MinImg
 */
#pragma pack(push, 1)
typedef struct
{
  int32_t  width;        ///< The image width in pixels. Must be positive.
  int32_t  height;       ///< The image height in pixels. Must be positive.
  int32_t  stride;       ///< Difference in bytes between addresses of adjacent rows.
  int32_t  channels;     ///< The number of channels per pixel. Must be positive.
  int32_t  channelDepth; ///< The channel depth in bytes. Must be nonnegative.
  MinFmt   format;       ///< The channel element format (see @c #MinFmt).
  uint8_t *pScan0;       ///< The pointer to the first pixel of the first row.
  int32_t  addressSpace; ///< Address space enumerator. 0 means that pScan0 is accessible.
} MinImg;
#pragma pack(pop)

#endif // #ifndef MINSBASE_MINIMG_H_INCLUDED
