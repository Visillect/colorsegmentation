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

#include <cstdlib>
#include <fstream>
#include <vector>

#ifdef WITH_WEBP
#include <webp/decode.h>
#include <webp/encode.h>
#endif // WITHWEBP

#include <minbase/minresult.h>
#include <minutils/minhelpers.h>

#include "minimgiowebp.h"
#include "utils.h"

#include <cstring>
#include <algorithm>


int GetWebPPages(const char * /* pFileName */) {
  return 1;
}


int GetWebPPageName(char *pPageName, int pageNameSize,
                    const char *pFileName, int /*page*/) {
#ifndef WITH_WEBP
  SUPPRESS_UNUSED_VARIABLE(pPageName);
  SUPPRESS_UNUSED_VARIABLE(pageNameSize);
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  return NOT_IMPLEMENTED;
#else // WITH_WEBP
  if (!pPageName || pageNameSize <= 0)
    return BAD_ARGS;
  if (!pFileName || strlen(pFileName) <= 0)
    return BAD_ARGS;

  ::memset(pPageName, 0, pageNameSize);
  ::memcpy(pPageName, pFileName, std::min((int)strlen(pFileName), pageNameSize));

  return NO_ERRORS;
#endif // WITH_WEBP
}


static int ReadFileToMemory(const char           *pFileName,
                            std::vector<uint8_t> &file_data) {
  if (!pFileName)
    return BAD_ARGS;

  std::ifstream ifs(pFileName, std::ifstream::binary);
  if (!ifs)
    return FILE_ERROR;

  int size = 0;
  ifs.seekg(0, ifs.end);
  size = static_cast<int>(ifs.tellg());
  ifs.seekg(0, ifs.beg);
  file_data.resize(size);
  ifs.read(reinterpret_cast<char *>(&file_data[0]), size);
  ifs.close();

  return NO_ERRORS;
}

static int GetWebPPropsFromMemory(
    MinImg        *pImg,
    ExtImgProps   *pProps,
    const uint8_t *ptr,
    size_t         size) {
#ifndef WITH_WEBP
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pProps);
  SUPPRESS_UNUSED_VARIABLE(ptr);
  SUPPRESS_UNUSED_VARIABLE(size);
  return NOT_IMPLEMENTED;
#else // WITH_WEBP
  WebPBitstreamFeatures features;
  VP8StatusCode sc = WebPGetFeatures(ptr, size, &features);
  if (sc != VP8_STATUS_OK) {
    return INTERNAL_ERROR;
  }

  pImg->width = features.width;
  pImg->height = features.height;
  pImg->channelDepth = 1;
  pImg->format = FMT_UINT;
  pImg->channels = 3; // For RGB
  if (features.has_alpha) {
    ++pImg->channels; // to handle as RGBA.
  }

  if (pProps) {
    pProps->iff = IFF_WEBP;
  }

  return NO_ERRORS;
#endif // WITH_WEBP
}

static int GetWebPProps_Memory(
    MinImg       *pImg,
    ExtImgProps  *pProps,
    const char   *pFileName) {
  uint8_t *ptr;
  size_t size;
  PROPAGATE_ERROR(ExtractMemoryLocation(pFileName, &ptr, &size));
  PROPAGATE_ERROR(GetWebPPropsFromMemory(pImg, pProps, ptr, size));

  return NO_ERRORS;
}

static int GetWebPProps_FileSystem(
    MinImg       *pImg,
    ExtImgProps  *pProps,
    const char   *pFileName) {
  std::vector<uint8_t> file_data;
  PROPAGATE_ERROR(ReadFileToMemory(pFileName, file_data));
  int retCode = GetWebPPropsFromMemory(pImg, pProps, &file_data[0], file_data.size());

  return retCode;
}

int GetWebPPropsEx(MinImg *pImg, ExtImgProps *pProps, const char  *pFileName) {
#ifndef WITH_WEBP
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pProps);
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  return NOT_SUPPORTED;
#else // WITH_WEBP
  if (!pImg || pImg->pScan0 || !pFileName)
    return BAD_ARGS;

  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inFileSystem)
    return GetWebPProps_FileSystem(pImg, pProps, pFileName);
  else if (fileLocation == inMemory)
    return GetWebPProps_Memory(pImg, pProps, pFileName);
  else
    return NOT_IMPLEMENTED;
#endif
}

static int DecodeWebPFromMemory(
    const MinImg *pImg,
    uint8_t      *ptr,
    size_t        size) {
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(ptr);
  SUPPRESS_UNUSED_VARIABLE(size);
#ifndef WITH_WEBP
  return NOT_IMPLEMENTED;
#else // WITH_WEBP
  if (!ptr || size == 0) {
    return BAD_ARGS;
  }

  if (pImg->channelDepth != 1 ||
      pImg->format != FMT_UINT) {
    return BAD_ARGS;
  }

  int stride = pImg->stride;
  int ht = pImg->height;
  if (pImg->channels == 3) {
    if (WebPDecodeRGBInto(ptr, size, pImg->pScan0, stride * ht, stride) == NULL)
      return INTERNAL_ERROR;
  } else if (pImg->channels == 4) {
    if (WebPDecodeRGBAInto(ptr, size, pImg->pScan0, stride * ht, stride) == NULL)
      return INTERNAL_ERROR;
  } else {
    return BAD_ARGS;
  }

  return NO_ERRORS;
#endif // WITH_WEBP
}

static int LoadWebP_Memory(const MinImg *pImg, const char *pFileName) {
  uint8_t *ptr;
  size_t size;
  PROPAGATE_ERROR(ExtractMemoryLocation(pFileName, &ptr, &size));
  PROPAGATE_ERROR(DecodeWebPFromMemory(pImg, ptr, size));

  return NO_ERRORS;
}

static int LoadWebP_FileSystem(const MinImg *pImg, const char *pFileName) {
  std::vector<uint8_t> file_data;
  PROPAGATE_ERROR(ReadFileToMemory(pFileName, file_data));
  int retCode = DecodeWebPFromMemory(pImg, &file_data[0], file_data.size());

  return retCode;
}

int LoadWebP(const MinImg *pImg, const char *pFileName) {
#ifndef WITH_WEBP
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  return NOT_SUPPORTED;
#else // WITH_WEBP
  if (!pImg || !pImg->pScan0 || !pFileName)
    return BAD_ARGS;
  if (pImg->channelDepth != 1 || pImg->format != FMT_UINT)
    return NOT_IMPLEMENTED;
  if (pImg->channels != 3)
    return NOT_IMPLEMENTED;

  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inFileSystem)
    return LoadWebP_FileSystem(pImg, pFileName);
  else if (fileLocation == inMemory)
    return LoadWebP_Memory(pImg, pFileName);
  else
    return NOT_IMPLEMENTED;
#endif
}

int SaveWebPEx(
    const char        *pFileName,
    const MinImg      *pImg,
    const ExtImgProps *pProps) {
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pProps);
#ifndef WITH_WEBP
  return NOT_SUPPORTED;
#else // WITH_WEBP
  if (!pImg || !pImg->pScan0 || !pFileName)
    return BAD_ARGS;
  if (pImg->channelDepth != 1 || pImg->format != FMT_UINT)
    return NOT_IMPLEMENTED;
  if (pImg->channels != 3)
    return NOT_IMPLEMENTED;

  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation != inFileSystem)
    return NOT_IMPLEMENTED;

  std::ofstream ofs(pFileName, std::ofstream::binary);
  if (!ofs)
    return FILE_ERROR;

  uint8_t *buffer = NULL;
  size_t size = WebPEncodeLosslessRGB(pImg->pScan0, pImg->width, pImg->height,
                                      pImg->stride, &buffer);

  if (!size)
    return INTERNAL_ERROR;

  ofs.write(reinterpret_cast<const char *>(buffer), size);
  ofs.close();
  free(buffer);

  return NO_ERRORS;
#endif // WITH_WEBP
}
