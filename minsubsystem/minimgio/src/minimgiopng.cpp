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

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cstddef>
#include <algorithm>

#include <minbase/minresult.h>
#include <minutils/smartptr.h>

#include "minimgiojpeg.h"
#include "utils.h"

#ifdef WITH_PNG

#include <png.h>

static void _FClose(FILE*& pF)
{
  if (pF) {
    fclose(pF);
    pF = NULL;
  }
}

DEFINE_SCOPED_OBJECT(_scoped_file, _FClose)
typedef _scoped_file<FILE> scoped_file;

struct FileReaderInterface {
  virtual void ReadBytes(uint8_t *bytes, size_t count) = 0;
  virtual bool IsGood() const = 0;
  virtual ~FileReaderInterface() { }
};

DEFINE_SCOPED_OBJECT(_scoped_file_reader, delete)
typedef _scoped_file_reader<FileReaderInterface> scoped_file_reader;

class FileInMemory : public FileReaderInterface {
public:
  FileInMemory(const char *filename) {
    ExtractMemoryLocation(filename, &data_ptr_, &size_);
    bytes_read_ = 0;
  }

  void ReadBytes(uint8_t *bytes, size_t count) {
    assert(bytes);
    assert(count <= size_ - bytes_read_);

    ::memcpy(bytes, data_ptr_ + bytes_read_, count);
    bytes_read_ += count;
  }

  bool IsGood() const { return data_ptr_ != NULL; }

private:
  uint8_t *data_ptr_;
  size_t size_;
  size_t bytes_read_;
};


class FileInFileSystem : public FileReaderInterface {
public:
  FileInFileSystem(const char *filename) : fp_(::fopen(filename, "rb")) { }
  ~FileInFileSystem() { if (fp_) ::fclose(fp_); }

  void ReadBytes(uint8_t *bytes, size_t count) {
    assert(bytes);
    ::fread(bytes, count, 1, fp_);
  }

  bool IsGood() const { return fp_ != NULL; }

private:
  FILE *fp_;
};

static void PngReadDataFromFile(png_structp png_ptr,
                                png_bytep   bytes,
                                png_size_t  count) {
  /// FIXME: Add error handling here.

  FileReaderInterface *reader =
      static_cast<FileReaderInterface *>(png_get_io_ptr(png_ptr));
  reader->ReadBytes(bytes, count);
}

int GetPngPages(const char * /* pFileName */)
{
  return 1;
}

int GetPngPageName(char *pPageName, int pageNameSize,
                   const char *pFileName, int /*page*/)
{
  if (!pPageName || pageNameSize <= 0)
    return BAD_ARGS;
  if (!pFileName || strlen(pFileName) <= 0)
    return BAD_ARGS;

  ::memset(pPageName, 0, pageNameSize);
  ::memcpy(pPageName, pFileName, std::min((int)strlen(pFileName), pageNameSize));

  return NO_ERRORS;
}

static FileReaderInterface* CreateFileReader(const char *pFileName)
{
  const int fileLocation = DeduceFileLocation(pFileName);
  switch (fileLocation) {
  case inFileSystem:
    return new FileInFileSystem(pFileName);
    break;
  case inMemory:
    return new FileInMemory(pFileName);
    break;
  default:
    return NULL;
  }
}

static int PngInitAndReadPrototype(
  png_structp *ppPng,
  png_infop *ppInfo,
  MinImg *pImg,
  FileReaderInterface *file)
{
  if (!file->IsGood())
    return FILE_ERROR;

  png_byte header[8] = { 0 };
  file->ReadBytes(header, 8);
  if (png_sig_cmp(header, 0, 8) != 0)
    return INTERNAL_ERROR;

  png_structp pPng = *ppPng = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  png_infop pInfo = *ppInfo = png_create_info_struct(pPng);
  if (!pInfo)
    return INTERNAL_ERROR;

  png_set_read_fn(pPng, file, PngReadDataFromFile);

  if (setjmp(png_jmpbuf(pPng)))
    return INTERNAL_ERROR;

  png_set_sig_bytes(pPng, 8);
  png_read_info(pPng, pInfo);

  // Begin
  png_uint_32 width = 0, height = 0;
  int depth = 0, color_type = 0, interlace = 0;
  png_get_IHDR(pPng, pInfo, &width, &height, &depth, &color_type, &interlace, 0, 0);

  if (color_type == PNG_COLOR_TYPE_PALETTE)
    png_set_palette_to_rgb(pPng);

  if (color_type == PNG_COLOR_TYPE_GRAY && depth < 8
      && (depth != 1 || pImg->channelDepth == 1 ))
    png_set_expand_gray_1_2_4_to_8(pPng);

  if (color_type & PNG_COLOR_MASK_ALPHA &&
      png_get_channels(pPng, pInfo) == pImg->channels + 1)
    png_set_strip_alpha(pPng);

  if (png_get_valid(pPng, pInfo, PNG_INFO_tRNS))
    png_set_tRNS_to_alpha(pPng);

  if (depth == 16)
  {
    if (pImg->channelDepth == 1)
      png_set_strip_16(pPng);
#ifndef __BIG_ENDIAN__
    else
      png_set_swap(pPng);
#endif
  }

  if (depth > 1 && depth < 8 && pImg->channelDepth == 1)
    png_set_packing(pPng);

  if (interlace != PNG_INTERLACE_NONE)
    png_set_interlace_handling(pPng);

  /* recheck header after setting EXPAND options */
  png_read_update_info(pPng, pInfo);
  png_get_IHDR(pPng, pInfo, &width, &height, &depth, &color_type, &interlace, 0, 0);

  if (depth % 8 != 0 && depth != 1)
    return NOT_SUPPORTED;
  pImg->channelDepth = depth / 8;
  pImg->channels = png_get_channels(pPng, pInfo);
  pImg->format = FMT_UINT;  // the only format supported by png
  pImg->height = height;
  pImg->width = width;
  return NO_ERRORS;
}

int GetPngPropsEx(MinImg *pImg, ExtImgProps *pProps, const char *pFileName)
{
  if (!pImg || pImg->pScan0 || !pFileName)
    return BAD_ARGS;
  FileReaderInterface *file = CreateFileReader(pFileName);
  if (!file)
    return BAD_ARGS;
  scoped_file_reader FileGuardian(file);

  png_structp pPng = NULL;
  png_infop pInfo = NULL;
  BACKED_PROPAGATE_ERROR(
    PngInitAndReadPrototype(&pPng, &pInfo, pImg, file),
    png_destroy_read_struct(&pPng, &pInfo, NULL)
  );

  if (pProps)
  {
    int unitType = PNG_RESOLUTION_UNKNOWN;
    png_uint_32 resX = 0, resY = 0;
    png_get_pHYs(pPng, pInfo, &resX, &resY, &unitType);
    pProps->iff = IFF_PNG;
    pProps->comp = IFC_NONE;
    pProps->qty = 0;  // zlib compression quality is unknown
    if (unitType == PNG_RESOLUTION_METER)
    {
      const double dpiFactor = 100.0 / 2.54;
      pProps->xDPI = static_cast<float>(resX / dpiFactor);
      pProps->yDPI = static_cast<float>(resY / dpiFactor);
    }
    else
    {
      pProps->xDPI = static_cast<float>(resX);
      pProps->yDPI = static_cast<float>(resY);
    }
  }
  png_destroy_read_struct(&pPng, &pInfo, 0);

  return NO_ERRORS;
}

int LoadPng(const MinImg *pImg, const char *pFileName)
{
  if (!pImg || !pImg->pScan0 || !pFileName || pImg->channels < 1)
    return BAD_ARGS;
  if (pImg->format != FMT_UINT || pImg->channels > 4)
    return NOT_SUPPORTED;
  if (pImg->channelDepth < 0 || pImg->channelDepth > 2)
    return NOT_SUPPORTED;

  FileReaderInterface *file = CreateFileReader(pFileName);
  if (!file)
    return BAD_ARGS;
  scoped_file_reader FileGuardian(file);

  png_structp pPng = NULL;
  png_infop pInfo = NULL;
  MinImg imgRead = { 0 };
  imgRead.channels = pImg->channels;
  imgRead.channelDepth = pImg->channelDepth;
  BACKED_PROPAGATE_ERROR(
    PngInitAndReadPrototype(&pPng, &pInfo, &imgRead, file),
    png_destroy_read_struct(&pPng, &pInfo, NULL)
  );

  if (
    imgRead.width != pImg->width ||
    imgRead.height != pImg->height ||
    imgRead.channels != pImg->channels ||
    imgRead.channelDepth != pImg->channelDepth ||
    imgRead.format != pImg->format ||
    imgRead.addressSpace != pImg->addressSpace
    )
  {
    png_destroy_read_struct(&pPng, &pInfo, 0);
    return BAD_ARGS;
  }

  // Read the file
  if (setjmp(png_jmpbuf(pPng)))
  {
    png_destroy_read_struct(&pPng, &pInfo, 0);
    return INTERNAL_ERROR;
  }
  scoped_cpp_array<png_bytep> ppRows(new png_bytep[pImg->height]);
  for(int32_t y = 0; y < pImg->height; ++y)
    ppRows[y] = pImg->pScan0 + y * pImg->stride;

  png_read_image(pPng, ppRows);

  png_destroy_read_struct(&pPng, &pInfo, (png_infopp)NULL);
  return NO_ERRORS;

}

int SavePngEx(const char *pFileName, const MinImg *pImg, const ExtImgProps *pProps)
{
  if (!pImg || !pImg->pScan0 || !pFileName || pImg->channels < 1)
    return BAD_ARGS;
  if (pImg->format != FMT_UINT || pImg->channels > 4)
    return NOT_SUPPORTED;
  if (pImg->channelDepth < 0 || pImg->channelDepth > 2)
    return NOT_SUPPORTED;
  if (pImg->channelDepth == 0 && pImg->channels != 1)
    return NOT_SUPPORTED;

  scoped_file pFile(fopen(pFileName, "wb"));
  if (pFile == 0)
    return BAD_ARGS;

  png_structp pPng = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (pPng == 0)
    return INTERNAL_ERROR;

  png_infop pInfo = png_create_info_struct(pPng);
  if (pInfo == 0)
  {
    png_destroy_write_struct(&pPng, 0);
    return INTERNAL_ERROR;
  }

  if (setjmp(png_jmpbuf(pPng)))
  {
    png_destroy_write_struct(&pPng, &pInfo);
    return INTERNAL_ERROR;
  }

  const int depth = std::max(1, pImg->channelDepth * 8);
  int colorType = PNG_COLOR_TYPE_GRAY;
  if (pImg->channels == 2)
    colorType = PNG_COLOR_TYPE_GRAY_ALPHA;
  else if (pImg->channels == 3)
    colorType = PNG_COLOR_TYPE_RGB;
  else if (pImg->channels == 4)
    colorType = PNG_COLOR_TYPE_RGBA;
  png_set_IHDR(pPng, pInfo, pImg->width, pImg->height, depth, colorType, PNG_INTERLACE_NONE, 0, 0);

  if (pProps != 0)
  {
    if (pProps->xDPI || pProps->yDPI)
    {
      const double dpiFactor = 100.0 / 2.54;
      uint32_t xDPM = static_cast<uint32_t>(pProps->xDPI * dpiFactor + 0.5);
      uint32_t yDPM = static_cast<uint32_t>(pProps->yDPI * dpiFactor + 0.5);
      png_set_pHYs(pPng, pInfo, xDPM , yDPM, PNG_RESOLUTION_METER);
    }
    if (pProps->qty)
    {
      // to comply with jpeg range 0-100, where 0 is the default value
      const int level = std::max(-1, std::min(pProps->qty / 10, 9));
      png_set_compression_level(pPng, level);
    }
  }

  png_init_io(pPng, pFile);
  png_write_info(pPng, pInfo);
#ifndef __BIG_ENDIAN__
  if (depth == 16)
    png_set_swap(pPng);
#endif

  scoped_cpp_array<png_bytep> ppRows(new png_bytep[pImg->height]);
  for(int y = 0; y < pImg->height; ++y)
    ppRows[y] = pImg->pScan0 + y * pImg->stride;
  png_write_image(pPng, ppRows);

  png_write_end(pPng, pInfo);
  png_destroy_write_struct(&pPng, &pInfo);

  return NO_ERRORS;
}

#else // WITH_PNG

int GetPngPages(const char * /*pFileName*/)
{
  return NOT_SUPPORTED;
}

int GetPngPageName(char * /*pPageName*/, int /*pageNameSize*/,
                   const char * /*pFileName*/, int /*page*/)
{
  return NOT_IMPLEMENTED;
}

int GetPngPropsEx(MinImg * /*pImg*/, ExtImgProps * /*pProps*/,
                  const char * /*pFileName*/)
{
  return NOT_SUPPORTED;
}

int LoadPng(const MinImg * /*pImg*/, const char * /*pFileName*/)
{
  return NOT_SUPPORTED;
}

int SavePngEx(const char * /*pFileName*/, const MinImg * /*pImg*/,
              const ExtImgProps * /*pProps*/)
{
  return NOT_SUPPORTED;
}

#endif  // WITH_PNG
