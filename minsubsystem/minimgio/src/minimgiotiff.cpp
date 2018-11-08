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

#include <cstring>
#include <algorithm>

#include <minutils/smartptr.h>
#include <minutils/minhelpers.h>
#include <minbase/minresult.h>

#ifdef WITH_TIFF

#include <tiffio.h>

static void _TIFFClose(TIFF *pTIF)
{
  if (pTIF)
    TIFFClose(pTIF);
}

DEFINE_SCOPED_OBJECT(_scoped_tiff_handle, _TIFFClose)
typedef _scoped_tiff_handle<TIFF> scoped_tiff_handle;

DEFINE_SCOPED_OBJECT(_scoped_scanline, _TIFFfree)
typedef _scoped_scanline<void> scoped_scanline;

template <class T> static void _TIFFGetField(
    TIFF *pTIF,
    ttag_t tag,
    T *pVal,
    const T &defVal) {
  if (!TIFFGetField(pTIF, tag, pVal))
  {
    *pVal = defVal;
  }
}

#endif // WITH_TIFF

#include "minimgiotiff.h"
#include "pack.h"


int GetTiffPages(const char *pFileName)
{
#ifndef WITH_TIFF
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  return NOT_IMPLEMENTED;
#else
  if (!pFileName)
    return BAD_ARGS;

  TIFFSetErrorHandler(NULL);
  TIFFSetWarningHandler(NULL);

  scoped_tiff_handle pTIF(TIFFOpen(pFileName, "r"));
  if (!pTIF)
    return FILE_ERROR;
  const int nPages = TIFFNumberOfDirectories(pTIF);
  if (nPages < 0)
    return INTERNAL_ERROR;
  return nPages;
#endif // WITH_TIFF
}

int GetTiffPageName(char *pPageName, int pageNameSize,
                    const char *pFileName, int /*page*/)
{
#ifndef WITH_TIFF
  SUPPRESS_UNUSED_VARIABLE(pPageName);
  SUPPRESS_UNUSED_VARIABLE(pageNameSize);
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  return NOT_IMPLEMENTED;
#else
  if (!pPageName || pageNameSize <= 0)
    return BAD_ARGS;
  if (!pFileName || strlen(pFileName) <= 0)
    return BAD_ARGS;

  ::memset(pPageName, 0, pageNameSize);
  ::memcpy(pPageName, pFileName, std::min((int)strlen(pFileName), pageNameSize));

  return NO_ERRORS;
#endif // WITH_TIFF
}

int GetTiffPropsEx
(
  MinImg *pImg,
  ExtImgProps *pProps,
  const char *pFileName,
  int page
)
{
#ifndef WITH_TIFF
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pProps);
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  SUPPRESS_UNUSED_VARIABLE(page);
  return NOT_SUPPORTED;
#else
  if (!pFileName || page < 0)
    return BAD_ARGS;
  if (pImg && pImg->pScan0)
    return BAD_ARGS;

  TIFFSetErrorHandler(NULL);
  TIFFSetWarningHandler(NULL);

  scoped_tiff_handle pTIF(TIFFOpen(pFileName, "r"));
  if (!pTIF)
    return FILE_ERROR;
  const int nPages = TIFFNumberOfDirectories(pTIF);
  if (page >= nPages)
    return BAD_ARGS;
  TIFFSetDirectory(pTIF, page);

  if (pImg)
  {
    int nc = 0, typ = PLANARCONFIG_CONTIG;
    _TIFFGetField(pTIF, TIFFTAG_SAMPLESPERPIXEL, &nc, 1);
    if (nc > 1)
      _TIFFGetField(pTIF, TIFFTAG_PLANARCONFIG, &typ, PLANARCONFIG_CONTIG);
    if (typ != PLANARCONFIG_CONTIG)
      return NOT_IMPLEMENTED;

    int wd = 0, ht = 0, bpc = 0;
    _TIFFGetField(pTIF, TIFFTAG_IMAGEWIDTH, &wd, 0);
    _TIFFGetField(pTIF, TIFFTAG_IMAGELENGTH, &ht, 0);
    _TIFFGetField(pTIF, TIFFTAG_BITSPERSAMPLE, &bpc, 0);
    if (bpc != 1 && bpc & 7)
      return NOT_IMPLEMENTED;
    bpc = bpc >> 3;

    int dtyp = 0;
    _TIFFGetField(pTIF, TIFFTAG_SAMPLEFORMAT, &dtyp, 0);
    switch (dtyp)
    {
    case 0:
    case SAMPLEFORMAT_UINT:
      dtyp = FMT_UINT;
      break;
    case SAMPLEFORMAT_INT:
      dtyp = FMT_INT;
      break;
    case SAMPLEFORMAT_IEEEFP:
      dtyp = FMT_REAL;
      break;
    default:
      return NOT_IMPLEMENTED;
    }

    pImg->width = wd;
    pImg->height = ht;
    pImg->channels = nc;
    pImg->channelDepth = bpc;
    pImg->format = (MinFmt)dtyp;
    pImg->stride = 0;
  }

  if (pProps)
  {
    pProps->iff = IFF_TIFF;
    pProps->comp = IFC_NONE;
    pProps->qty = 0;

    int compression = 0;
    _TIFFGetField(pTIF, TIFFTAG_COMPRESSION, &compression, COMPRESSION_NONE);
    switch(compression)
    {
      case COMPRESSION_LZW: pProps->comp = IFC_LZW; break;
      case COMPRESSION_DEFLATE: pProps->comp = IFC_DEFLATE; break;
      case COMPRESSION_PACKBITS: pProps->comp = IFC_PACKBITS; break;
      case COMPRESSION_JPEG: pProps->comp = IFC_JPEG; break;
      case COMPRESSION_CCITTRLE: pProps->comp = IFC_RLE; break;
      case COMPRESSION_CCITTFAX3: pProps->comp = IFC_GROUP3; break;
      case COMPRESSION_CCITTFAX4: pProps->comp = IFC_GROUP4; break;
      default: pProps->comp = IFC_NONE;
    }

    int unit = 0;
    _TIFFGetField(pTIF, TIFFTAG_XRESOLUTION, &pProps->xDPI, 0.f);
    _TIFFGetField(pTIF, TIFFTAG_YRESOLUTION, &pProps->yDPI, 0.f);
    _TIFFGetField(pTIF, TIFFTAG_RESOLUTIONUNIT, &unit, 0);
    switch (unit)
    {
    case RESUNIT_INCH:
      break;
    case RESUNIT_CENTIMETER:
      pProps->xDPI *= 2.54f;
      pProps->yDPI *= 2.54f;
      break;
    default:
      pProps->xDPI = 0.f;
      pProps->yDPI = 0.f;
    }
  }

  return NO_ERRORS;
#endif // WITH_TIFF
}

int LoadTiff
(
  const MinImg *pImg,
  const char *pFileName,
  int page
)
{
#ifndef WITH_TIFF
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  SUPPRESS_UNUSED_VARIABLE(page);
  return NOT_SUPPORTED;
#else
  if (!pImg || !pFileName || page < 0)
    return BAD_ARGS;
  if (!pImg->pScan0)
    return BAD_ARGS;

  TIFFSetErrorHandler(NULL);
  TIFFSetWarningHandler(NULL);

  scoped_tiff_handle pTIF(TIFFOpen(pFileName, "r"));
  if (!pTIF)
    return FILE_ERROR;
  const int nPages = TIFFNumberOfDirectories(pTIF);
  if (page >= nPages)
    return BAD_ARGS;
  TIFFSetDirectory(pTIF, page);

  int nc = 0, typ = PLANARCONFIG_CONTIG;
  _TIFFGetField(pTIF, TIFFTAG_SAMPLESPERPIXEL, &nc, 1);
  if (nc > 1)
    _TIFFGetField(pTIF, TIFFTAG_PLANARCONFIG, &typ, PLANARCONFIG_CONTIG);
  if (typ != PLANARCONFIG_CONTIG)
    return NOT_IMPLEMENTED;

  int wd = 0, ht = 0, bpc = 0;
  _TIFFGetField(pTIF, TIFFTAG_IMAGEWIDTH, &wd, 0);
  _TIFFGetField(pTIF, TIFFTAG_IMAGELENGTH, &ht, 0);
  _TIFFGetField(pTIF, TIFFTAG_BITSPERSAMPLE, &bpc, 0);

  if (bpc != 1 && bpc & 7)
    return NOT_IMPLEMENTED;
  bpc = bpc >> 3;

  if (pImg->channels != nc)
    return BAD_ARGS;
  if (bpc == 0 && pImg->channelDepth > 1)
    return BAD_ARGS;
  if (bpc > 0 && pImg->channelDepth != bpc)
    return BAD_ARGS;
  if (pImg->height < ht || pImg->width < wd)
    return BAD_ARGS;

  int metr = 0;
  _TIFFGetField(pTIF, TIFFTAG_PHOTOMETRIC, &metr, PHOTOMETRIC_MINISWHITE);
  if (metr == PHOTOMETRIC_MINISWHITE && bpc > 0)
    return NOT_IMPLEMENTED;

  int dtyp = SAMPLEFORMAT_UINT;
  _TIFFGetField(pTIF, TIFFTAG_SAMPLEFORMAT, &dtyp, SAMPLEFORMAT_UINT);
  switch (dtyp)
  {
  case SAMPLEFORMAT_UINT:
    if (pImg->format != FMT_UINT)
      return BAD_ARGS;
    break;
  case SAMPLEFORMAT_INT:
    if (pImg->format != FMT_INT)
      return BAD_ARGS;
    break;
  case SAMPLEFORMAT_IEEEFP:
    if (pImg->format != FMT_REAL)
      return BAD_ARGS;
    break;
  default:
    return NOT_IMPLEMENTED;
  }

  const tsize_t scanLen = TIFFScanlineSize(pTIF);
  scoped_scanline pScanLine(_TIFFmalloc(scanLen));
  if (!pScanLine)
    return NO_MEMORY;
  const int byteWidth = pImg->channelDepth > 0 ?
                        pImg->width * pImg->channels * pImg->channelDepth :
                        (pImg->width * pImg->channels + 7) >> 3;
  if (byteWidth < scanLen)
    return INTERNAL_ERROR;

  const bool invert = (metr == PHOTOMETRIC_MINISWHITE);
  const size_t len = wd * nc;
  uint8_t *pScanLineUint8 = (uint8_t *)((void *)(pScanLine));
  uint8_t *p_dst_line = pImg->pScan0;
  for (int y = 0; y < ht; y++)
  {
    SHOULD_WORK(TIFFReadScanline(pTIF, pScanLineUint8, y));

    if (bpc == 0 && bpc == pImg->channelDepth)
      CopyBits(p_dst_line, pScanLineUint8, len, invert);
    else if (bpc == 0 && bpc < pImg->channelDepth)
      UnpackLine(p_dst_line, pScanLineUint8, len, invert);
    else
      ::memcpy(p_dst_line, pScanLineUint8, scanLen);
    p_dst_line += pImg->stride;
  }

  return NO_ERRORS;
#endif // WITH_TIFF
}

#ifdef WITH_TIFF
struct TiffData
{
  int samples_per_pixel;
  int width;
  int height;
  int bytes_per_sample;
  int photometric;
  int sample_format;
  uint8_t *scan_lines;
  float xresolution;
  float yresolution;
  int resolution_unit;
  int compression_type;
  int jpeg_quality;
  unsigned scanLen;

  TiffData()
  {
    samples_per_pixel = 0;
    sample_format = SAMPLEFORMAT_UINT;
    bytes_per_sample = 0;
    xresolution = 0;
    yresolution = 0;
    resolution_unit = RESUNIT_INCH;
    compression_type = COMPRESSION_NONE;
    jpeg_quality = 90;
    scan_lines = 0;
    scanLen = 0;
    photometric = 0;
  }

  ~TiffData()
  {
    if (scan_lines)
      delete [] scan_lines;
    scan_lines = 0;
  }
};

static int AddPageToTiffExImpl
  (
  TIFF * pTIF,
  const MinImg *pImg,
  const ExtImgProps *pProps
  )
{
  int bpp = std::max(1, (int)pImg->channelDepth * 8);
  if (pProps != NULL)
  {
    switch (pProps->comp)
    {
      case IFC_RLE:
      case IFC_GROUP3:
      case IFC_GROUP4:
        bpp = 1;
        break;
      case IFC_JPEG:
        bpp = std::max(8, bpp);
        break;
      default:
        break;
    }
  }

  TIFFSetField(pTIF, TIFFTAG_IMAGELENGTH, pImg->height);
  TIFFSetField(pTIF, TIFFTAG_IMAGEWIDTH, pImg->width);
  TIFFSetField(pTIF, TIFFTAG_BITSPERSAMPLE, bpp);
  TIFFSetField(pTIF, TIFFTAG_SAMPLESPERPIXEL, pImg->channels);
  TIFFSetField(pTIF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);

  // FIXME: should be clarified for non-standard as soon as possible!
  if (pImg->channels == 3)
    TIFFSetField(pTIF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
  else
    TIFFSetField(pTIF, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);

  switch (pImg->format)
  {
    case FMT_UINT:
      TIFFSetField(pTIF, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_UINT);
      break;
    case FMT_INT:
      TIFFSetField(pTIF, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_INT);
      break;
    case FMT_REAL:
      TIFFSetField(pTIF, TIFFTAG_SAMPLEFORMAT, SAMPLEFORMAT_IEEEFP);
      break;
    default:
      return BAD_ARGS;
  }

  if (pProps)
  {
    TIFFSetField(pTIF, TIFFTAG_XRESOLUTION, pProps->xDPI);
    TIFFSetField(pTIF, TIFFTAG_YRESOLUTION, pProps->yDPI);
    TIFFSetField(pTIF, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);

    switch(pProps->comp)
    {
      case IFC_LZW: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_LZW); break;
      case IFC_DEFLATE: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE); break;
      case IFC_PACKBITS: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_PACKBITS); break;
      case IFC_JPEG: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_JPEG); break;
      case IFC_RLE: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_CCITTRLE); break;
      case IFC_GROUP3: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX3); break;
      case IFC_GROUP4: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4); break;
      default: TIFFSetField(pTIF, TIFFTAG_COMPRESSION, COMPRESSION_NONE); break;
    }

    if (pProps->comp == IFC_JPEG && pProps->qty >= 0 && pProps->qty <= 100)
      TIFFSetField(pTIF, TIFFTAG_JPEGQUALITY, pProps->qty);
  }

  if (bpp == 1 && pImg->channelDepth > 0)
  {
    if (pImg->channelDepth > 1 || pImg->channels > 1 || pImg->format != FMT_UINT)
      return NOT_IMPLEMENTED;

    const uint8_t level = 128;
    size_t size = (pImg->width + 7) / 8;
    scoped_cpp_array<uint8_t> pBuf(new uint8_t[size]);

    for (int y = 0; y < pImg->height; y++)
    {
      PackLine(pBuf, pImg->pScan0 + pImg->stride * y, level, pImg->width, false);
      SHOULD_WORK(TIFFWriteScanline(pTIF, pBuf, y, 0));
    }
  }
  else if (bpp == 8 && pImg->channelDepth == 0)
  {
    if (pImg->channels > 1 || pImg->format != FMT_UINT)
      return NOT_IMPLEMENTED;

    const int size = pImg->width;
    scoped_cpp_array<uint8_t> pBuf(new uint8_t[size]);

    for (int y = 0; y < pImg->height; y++)
    {
      UnpackLine(pBuf, pImg->pScan0 + pImg->stride * y, pImg->width, false);
      SHOULD_WORK(TIFFWriteScanline(pTIF, pBuf, y, 0));
    }
  }
  else
  {
    for (int y = 0; y < pImg->height; y++)
    {
      SHOULD_WORK(TIFFWriteScanline(pTIF, pImg->pScan0 + pImg->stride * y, y, 0));
    }
  }

  return NO_ERRORS;
}

#endif // WITH_TIFF

int SaveTiffEx
(
  const char *pFileName,
  const MinImg *pImg,
  const ExtImgProps *pProps,
  int page
)
{
#ifndef WITH_TIFF
  SUPPRESS_UNUSED_VARIABLE(pFileName);
  SUPPRESS_UNUSED_VARIABLE(pImg);
  SUPPRESS_UNUSED_VARIABLE(pProps);
  SUPPRESS_UNUSED_VARIABLE(page);
  return NOT_SUPPORTED;
#else
  if (!pImg || !pFileName || page < 0)
    return BAD_ARGS;
  if (!pImg->pScan0)
    return BAD_ARGS;

  TIFFSetErrorHandler(NULL);
  TIFFSetWarningHandler(NULL);

  int nPages = GetTiffPages(pFileName);
  if (nPages == FILE_ERROR)
    nPages = 0;
  if (page > nPages || page < 0)
    return BAD_ARGS;
  scoped_cpp_array<TiffData> tiff_pages(new TiffData[nPages]);
  if (page < nPages)
  {
    scoped_tiff_handle pTIF(TIFFOpen(pFileName, "r"));
    if (!pTIF)
      return FILE_ERROR;

    for (int k = 0; k < nPages; k++)
    {
      TiffData &data = tiff_pages[k];
      TIFFSetDirectory(pTIF, k);
      int &nc = data.samples_per_pixel;
      int typ = PLANARCONFIG_CONTIG;
      _TIFFGetField(pTIF, TIFFTAG_SAMPLESPERPIXEL, &nc, 1);
      if (nc > 1)
        _TIFFGetField(pTIF, TIFFTAG_PLANARCONFIG, &typ, PLANARCONFIG_CONTIG);
      if (typ != PLANARCONFIG_CONTIG)
        return NOT_IMPLEMENTED;

      int &bpc = data.bytes_per_sample;
      _TIFFGetField(pTIF, TIFFTAG_IMAGEWIDTH, &data.width, 0);
      _TIFFGetField(pTIF, TIFFTAG_IMAGELENGTH, &data.height, 0);
      _TIFFGetField(pTIF, TIFFTAG_BITSPERSAMPLE, &bpc, 0);

      if (bpc != 1 && bpc & 7)
        return NOT_IMPLEMENTED;
      bpc = bpc >> 3;

      int &metr = data.photometric;
      _TIFFGetField(pTIF, TIFFTAG_PHOTOMETRIC, &metr, PHOTOMETRIC_MINISWHITE);
      if (metr == PHOTOMETRIC_MINISWHITE && bpc > 0)
        return NOT_IMPLEMENTED;

      _TIFFGetField(pTIF, TIFFTAG_SAMPLEFORMAT, &data.sample_format, SAMPLEFORMAT_UINT);
      _TIFFGetField(pTIF, TIFFTAG_XRESOLUTION, &data.xresolution, 0.0f);
      _TIFFGetField(pTIF, TIFFTAG_YRESOLUTION, &data.yresolution, 0.0f);
      _TIFFGetField(pTIF, TIFFTAG_RESOLUTIONUNIT, &data.resolution_unit, RESUNIT_INCH);
      _TIFFGetField(pTIF, TIFFTAG_COMPRESSION, &data.compression_type, COMPRESSION_NONE);
      _TIFFGetField(pTIF, TIFFTAG_JPEGQUALITY, &data.jpeg_quality, 100);
      const tsize_t scanLen = TIFFScanlineSize(pTIF);
      data.scan_lines = new uint8_t[data.height * scanLen];
      data.scanLen = scanLen;
      for (int y = 0; y < data.height; y++)
      {
        void * pScanLine = data.scan_lines + y * scanLen;
        SHOULD_WORK(TIFFReadScanline(pTIF, pScanLine, y));
      }
    }
   }

  char mode[] = "a";
  if (page < nPages)
    mode[0] = 'w';
  scoped_tiff_handle pTIF(TIFFOpen( pFileName, mode ));
  if (!pTIF)
    return FILE_ERROR;

  if (page == nPages)
  {
    TIFFSetField(pTIF, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
    TIFFSetField(pTIF, TIFFTAG_PAGENUMBER, page, nPages + 1 );
    PROPAGATE_ERROR(AddPageToTiffExImpl(pTIF, pImg, pProps) );
    if (!TIFFWriteDirectory (pTIF))
      return INTERNAL_ERROR;
    return NO_ERRORS;
  }

  for (int k = 0; k < nPages; k++)
  {
    TIFFSetField(pTIF, TIFFTAG_SUBFILETYPE, FILETYPE_PAGE);
    if (k == page)
    {
      TIFFSetField(pTIF, TIFFTAG_PAGENUMBER, page, nPages );
      PROPAGATE_ERROR(AddPageToTiffExImpl(pTIF, pImg, pProps) );
      if (!TIFFWriteDirectory(pTIF))
        return INTERNAL_ERROR;
      continue;
    }

    TIFFSetField(pTIF, TIFFTAG_PAGENUMBER, k, nPages );
    TiffData const& data = tiff_pages[k];
    int bpp = std::max(1, data.bytes_per_sample * 8);
    switch (data.compression_type)
    {
      case COMPRESSION_CCITTRLE:
      case COMPRESSION_CCITTFAX3:
      case COMPRESSION_CCITTFAX4:
        bpp = 1;
        break;
      case COMPRESSION_JPEG:
        bpp = std::max(8, bpp);
        break;
      default:
        break;
    }

    TIFFSetField(pTIF, TIFFTAG_IMAGELENGTH, data.height);
    TIFFSetField(pTIF, TIFFTAG_IMAGEWIDTH, data.width);
    TIFFSetField(pTIF, TIFFTAG_BITSPERSAMPLE, bpp);
    TIFFSetField(pTIF, TIFFTAG_SAMPLESPERPIXEL, data.samples_per_pixel );
    TIFFSetField(pTIF, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(pTIF, TIFFTAG_PHOTOMETRIC, data.photometric);
    TIFFSetField(pTIF, TIFFTAG_SAMPLEFORMAT, data.sample_format);
    TIFFSetField(pTIF, TIFFTAG_XRESOLUTION, data.xresolution);
    TIFFSetField(pTIF, TIFFTAG_YRESOLUTION, data.yresolution);
    TIFFSetField(pTIF, TIFFTAG_RESOLUTIONUNIT, data.resolution_unit);
    TIFFSetField(pTIF, TIFFTAG_COMPRESSION, data.compression_type);

    if (data.compression_type == COMPRESSION_JPEG && data.jpeg_quality >= 0 && data.jpeg_quality <= 100)
      TIFFSetField(pTIF, TIFFTAG_JPEGQUALITY, data.jpeg_quality);

    for (int y = 0; y < data.height; ++y)
    {
      uint8_t * pBuf = data.scan_lines + y * data.scanLen;
      SHOULD_WORK(TIFFWriteScanline(pTIF, pBuf, y, 0));
    }
    if (!TIFFWriteDirectory(pTIF))
      return INTERNAL_ERROR;
  }
  return NO_ERRORS;
#endif // WITH_TIFF
}
