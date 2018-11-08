/*

Copyright (c) 2011, Smart Engines Limited. All rights reserved.

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
 * @file   device.h
 * @brief  MinImgIO library device interface.
 */

#pragma once
#ifndef MINIMGIO_DEVICE_H_INCLUDED
#define MINIMGIO_DEVICE_H_INCLUDED

#include <minimgio/define.h>

#ifdef __cplusplus
extern "C" {
#endif

static const char * DSS_CAMERA = "camera";

/**
 * Specifies frame size on a video capture device in the following format:
 * <width>x<height>. If the device doesn't support the specified frame size,
 * than the nearest value will be used.
 */
static const char * SP_FRAMESIZE      = "frameSize";
/**
 * Specifies frame rate on a video capture device. If the device doesn't
 * support the specified frame rate, than the nearest value will be used.
 */
static const char * SP_FRAMERATE      = "frameRate";
/**
 * Specifies the gain adjustment on a video capture device. Zero is normal.
 * Positive values are brighter and negative values are darker. The value is
 * integer and ranges [-100; 100].
 */
static const char * SP_GAIN           = "gain";
/**
 * Specifies whether the gain is controlled manually or automatically.
 * The value can be either @c true or @c false.
 */
static const char * SP_GAINAUTO       = "gainAuto";
/**
 * Specifies the exposure setting on a video capture device in seconds.
 * The value is real and must be positive.
 */
static const char * SP_EXPOSURE       = "exposure";
/**
 * Specifies whether the exposure is controlled manually or automatically.
 * The value can be either @c true or @c false.
 */
static const char * SP_EXPOSUREAUTO   = "exposureAuto";
/**
 * Specifies the focus setting on a video capture device in millimeters.
 * The value is real and must be positive.
 */
static const char * SP_FOCUS          = "focus";
/**
 * Specifies whether the focus is controlled manually or automatically.
 * The value can be either @c true or @c false.
 */
static const char * SP_FOCUSAUTO   = "focusAuto";
/**
 * Specifies the video file path for captured video. If the option is not
 * defined no video will be saved.
 */
static const char * SP_VIDEOFILEPATH  = "videoFilePath";
/**
 * Specifies the video preview (platform-dependent) for video capture session.
 */
static const char * SP_VIDEOPREVIEW   = "videoPreview";

MINIMGIO_API int GetDeviceList
(
  const char *pSubSystemName,
  char       *pDeviceNames,
  int         size
);

MINIMGIO_API int OpenStream
(
  const char *pSubSystemName,
  const char *pDeviceName,
  char       *pURI,
  int         size
);

MINIMGIO_API int CloseStream
(
  const char *pURI
);

MINIMGIO_API int GetStreamProperty
(
  const char *pURI,
  const char *pPropertyKey,
  char       *pPropertyValue,
  int         size
);

MINIMGIO_API int SetStreamProperty
(
  const char *pURI,
  const char *pPropertyKey,
  const char *pPropertyVal
);

#ifdef __cplusplus
}
#endif

#endif // #infdef MINIMGIO_DEVICE_H_INCLUDED
