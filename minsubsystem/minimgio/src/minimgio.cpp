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

#ifdef _MSC_VER
#pragma warning(disable : 4996)
#endif

#include <cstdio>
#include <cmath>
#include <cstring>

#include <minbase/minresult.h>
#include "minimgiodevice.h"
#include "minimgiotiff.h"
#include "minimgiojpeg.h"
#include "minimgiopng.h"
#include "minimgiowebp.h"
#include "minimgiolst.h"
#include "utils.h"
#include "pack.h"

#ifdef _WIN32
#define stricmp _stricmp
#else
#include <strings.h>  // POSIX 
#define stricmp strcasecmp
#endif

static inline
int ExtractBytes_FileSystem(const char *fileName, int count, uint8_t *bytes)
{
  if (count <= 0)
    return BAD_ARGS;
  if (!bytes)
    return BAD_ARGS;

  FILE *pf = fopen(fileName, "rb");
  if (NULL == pf)
    return FILE_ERROR;

  const size_t ret = fread(bytes, count, 1, pf);
  fclose(pf);

  if (ret != 1)
    return FILE_ERROR;

  return NO_ERRORS;
}

static inline
int ExtractBytes_Memory(const char *fileName, int count, uint8_t *bytes)
{
  if (count <= 0)
    return BAD_ARGS;
  if (!bytes)
    return BAD_ARGS;

  uint8_t *ptr = NULL;
  size_t size = 0;
  PROPAGATE_ERROR(ExtractMemoryLocation(fileName, &ptr, &size));
  if (static_cast<int>(size) < count)
    return BAD_ARGS;

  ::memcpy(bytes, ptr, count);
  return NO_ERRORS;
}

static int GuessImageFileFormatByTag
(
  const char *fileName
)
{
  if (!fileName)
    return BAD_ARGS;

  int fileLocation = DeduceFileLocation(fileName);
  uint8_t tag[12] = {0};
  int resTagRead = 0;
  switch (fileLocation)
  {
    case inFileSystem:
      resTagRead = ExtractBytes_FileSystem(fileName, sizeof(tag), tag);
      break;
    case inMemory:
      resTagRead = ExtractBytes_Memory(fileName, sizeof(tag), tag);
      break;
    default:
      return IFF_UNKNOWN;
  }

  if (0 <= resTagRead)
  {
    const uint8_t TIFF_TAG_II[] = {0x49, 0x49};
    const uint8_t TIFF_TAG_MM[] = {0x4d, 0x4d};
    const uint8_t JPEG_TAG[] = {0xFF, 0xD8};
    const uint8_t PNG_TAG[]  = {0x89, 0x50};
    const uint8_t WEBP_TAG_1[] = {'R', 'I', 'F', 'F'};
    const uint8_t WEBP_TAG_2[] = {'W', 'E', 'B', 'P'};

    if (!::memcmp(tag, TIFF_TAG_II, sizeof(TIFF_TAG_II)) ||
        !::memcmp(tag, TIFF_TAG_MM, sizeof(TIFF_TAG_MM)))
      return IFF_TIFF;
    else if (!::memcmp(tag, JPEG_TAG, sizeof(JPEG_TAG)))
      return IFF_JPEG;
    else if (!::memcmp(tag, PNG_TAG, sizeof(PNG_TAG)))
      return IFF_PNG;
    else if (!::memcmp(tag,     WEBP_TAG_1, sizeof(WEBP_TAG_1)) &&
             !::memcmp(tag + 8, WEBP_TAG_2, sizeof(WEBP_TAG_2)))
      return IFF_WEBP;
  }

  return IFF_UNKNOWN;
}

static int GuessImageFileFormatByExtension
(
  const char *fileName
)
{
  if (!fileName)
    return BAD_ARGS;

  const char *pExt = strrchr(fileName, '.');
  if (!pExt)
    return IFF_UNKNOWN;
  pExt++;
  if (!stricmp(pExt, "jpg") || !stricmp(pExt, "jpeg"))
    return IFF_JPEG;
  if (!stricmp(pExt, "tif") || !stricmp(pExt, "tiff"))
    return IFF_TIFF;
  if (!stricmp(pExt, "png"))
    return IFF_PNG;
  if (!stricmp(pExt, "webp"))
    return IFF_WEBP;
  if (!stricmp(pExt, "lst") || !stricmp(pExt, "txt"))
    return IFF_LST;

  return IFF_UNKNOWN;
}

MINIMGIO_API int GuessImageFileFormat
(
  const char *fileName
)
{
  // Guess by tag.
  int iff = GuessImageFileFormatByTag(fileName);
  if (iff != IFF_UNKNOWN)
    return iff;

  // Guess by content.
  if (DeduceFileLocation(fileName) == inFileSystem && IsTextualFile(fileName))
    return IFF_LST;

  // Guess by extension.
  return GuessImageFileFormatByExtension(fileName);
}

MINIMGIO_API int GetMinImageFilePages
(
  const char *pFileName
)
{
  if (!pFileName)
    return BAD_ARGS;
  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inDevice)
    return GetDevicePages(pFileName);

  int iff = GuessImageFileFormat(pFileName);
  if (iff < 0)
    return iff;

  switch (iff)
  {
    case IFF_TIFF:
      return GetTiffPages(pFileName);
    case IFF_JPEG:
      return GetJpegPages(pFileName);
    case IFF_PNG:
      return GetPngPages(pFileName);
    case IFF_WEBP:
      return GetWebPPages(pFileName);
    case IFF_LST:
      return GetLstPages(pFileName);
    default:
      return 0;
  }

  return INTERNAL_ERROR;
}

MINIMGIO_API int GetMinImagePageName(char *pPageName, int pageNameSize, const char *pFileName, int page)
{
  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inDevice)
    return GetDevicePageName(pPageName, pageNameSize, pFileName, page);

  int iff = GuessImageFileFormat(pFileName);
  if (iff < 0)
    return iff;

  switch (iff)
  {
    case IFF_TIFF:
      return GetTiffPageName(pPageName, pageNameSize, pFileName, page);
    case IFF_JPEG:
      return GetJpegPageName(pPageName, pageNameSize, pFileName, page);
    case IFF_PNG:
      return GetPngPageName(pPageName, pageNameSize, pFileName, page);
    case IFF_WEBP:
      return GetPngPageName(pPageName, pageNameSize, pFileName, page);
    case IFF_LST:
      return GetLstPageName(pPageName, pageNameSize, pFileName, page);
    default:
      return 0;
  }

  return INTERNAL_ERROR;
}

MINIMGIO_API int GetMinImageFileProps
(
  MinImg     *pImg,
  const char *pFileName,
  int         page
)
{
  return GetMinImageFilePropsEx(pImg, NULL, pFileName, page);
}

MINIMGIO_API int GetMinImageFilePropsEx
(
  MinImg      *pImg,
  ExtImgProps *pProps,
  const char  *pFileName,
  int          page
)
{
  if (!pFileName || (!pProps && !pImg))
    return BAD_ARGS;
  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inDevice)
    return GetDevicePropsEx(pImg, pProps, pFileName);

  int iff = GuessImageFileFormat(pFileName);
  if (iff < 0)
    return iff;

  switch (iff)
  {
  case IFF_TIFF:
    return GetTiffPropsEx(pImg, pProps, pFileName, page);
  case IFF_JPEG:
    return GetJpegPropsEx(pImg, pProps, pFileName);
  case  IFF_PNG:
    return GetPngPropsEx(pImg, pProps, pFileName);
  case IFF_WEBP:
    return GetWebPPropsEx(pImg, pProps, pFileName);
  case IFF_LST:
    return GetLstPropsEx(pImg, pProps, pFileName, page);
  default:
    return FILE_ERROR;
  }
  return INTERNAL_ERROR;
}

MINIMGIO_API int LoadMinImage
(
  const MinImg *pImg,
  const char   *pFileName,
  int          page
)
{
  if (!pFileName)
    return BAD_ARGS;
  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inDevice)
    return LoadDevice(pImg, pFileName);

  int iff = GuessImageFileFormat(pFileName);
  if (iff < 0)
    return iff;

  switch (iff)
  {
  case IFF_TIFF:
    return LoadTiff(pImg, pFileName, page);
  case IFF_JPEG:
    return LoadJpeg(pImg, pFileName);
  case IFF_PNG:
    return LoadPng(pImg, pFileName);
  case IFF_WEBP:
    return LoadWebP(pImg, pFileName);
  case IFF_LST:
    return LoadLst(pImg, pFileName, page);
  default:
    return FILE_ERROR;
  }
  return INTERNAL_ERROR;
}

MINIMGIO_API int SaveMinImage
(
  const char   *pFileName,
  const MinImg *pImg,
  int           page
)
{
  return SaveMinImageEx(pFileName, pImg, NULL, page);
}

MINIMGIO_API int SaveMinImageEx
(
  const char        *pFileName,
  const MinImg      *pImg,
  const ExtImgProps *pProps,
  int                page
)
{
  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation == inDevice)
    return SaveDeviceEx(pFileName, pImg, pProps);

  int iff = IFF_UNKNOWN;
  if (pProps)
    iff = pProps->iff;

  if (iff == IFF_UNKNOWN)
    iff = GuessImageFileFormatByExtension(pFileName);
  if (iff < 0)
    return iff;

  switch (iff)
  {
  case IFF_TIFF:
    return SaveTiffEx(pFileName, pImg, pProps, page);
  case IFF_JPEG:
    if (page != 0)
      return BAD_ARGS; // JPEG has multiple frames extensions, ignore them
    return SaveJpegEx(pFileName, pImg, pProps);
  case IFF_PNG:
    if (page != 0)
      return NOT_IMPLEMENTED; // PNG kind of supports multiple frames
    return SavePngEx(pFileName, pImg, pProps);
  case IFF_WEBP:
    if (page != 0)
      return NOT_IMPLEMENTED; // WebP supports multiple frames
    return SaveWebPEx(pFileName, pImg, pProps);
  case IFF_LST:
    return SaveLstEx(pFileName, pImg, pProps, page);
  default:
    return FILE_ERROR;
  }
  return INTERNAL_ERROR;
}

MINIMGIO_API int PackMinImage
(
  const MinImg *pDst,
  const MinImg *pSrc,
  uint8_t       level
)
{
  if (pSrc == NULL || pDst == NULL)
    return BAD_ARGS;
  if (pSrc->pScan0 == NULL || pDst->pScan0 == NULL)
    return BAD_ARGS;
  if (pSrc->width > pDst->width || pSrc->height > pDst->height)
    return BAD_ARGS;
  if (pSrc->format != FMT_UINT || pDst->format != FMT_UINT)
    return BAD_ARGS;
  if (pSrc->channelDepth != 1 || pDst->channelDepth != 0)
    return BAD_ARGS;
  if (pSrc->channels != pDst->channels || pDst->channels < 1)
    return BAD_ARGS;

  const size_t count = pDst->width * pDst->channels;
  for (int y = 0; y < pDst->height; y++)
  {
    const uint8_t *pSrcLine = pSrc->pScan0 + y * pSrc->stride;
    uint8_t *pDstLine = pDst->pScan0 + y * pDst->stride;
    PROPAGATE_ERROR(PackLine(pDstLine, pSrcLine, level, count, false));
  }

  return NO_ERRORS;
}

MINIMGIO_API int UnpackMinImage
(
  const MinImg *pDst,
  const MinImg *pSrc
)
{
  if (pSrc == NULL || pDst == NULL)
    return BAD_ARGS;
  if (pSrc->pScan0 == NULL || pDst->pScan0 == NULL)
    return BAD_ARGS;
  if (pSrc->width > pDst->width || pSrc->height > pDst->height)
    return BAD_ARGS;
  if (pSrc->format != FMT_UINT || pDst->format != FMT_UINT)
    return BAD_ARGS;
  if (pSrc->channelDepth != 0 || pDst->channelDepth != 1)
    return BAD_ARGS;
  if (pSrc->channels != pDst->channels || pDst->channels < 1)
    return BAD_ARGS;

  const size_t count = pDst->width * pDst->channels;
  for (int y = 0; y < pDst->height; y++)
  {
    const uint8_t *pSrcLine = pSrc->pScan0 + y * pSrc->stride;
    uint8_t *pDstLine = pDst->pScan0 + y * pDst->stride;
    PROPAGATE_ERROR(UnpackLine(pDstLine, pSrcLine, count, false));
  }

  return NO_ERRORS;
}
