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
#include <csetjmp>
#include <cstring>
#include <algorithm>

#include <minutils/smartptr.h>
#include <minbase/minresult.h>

#include "minimgiojpeg.h"
#include "utils.h"

#ifdef WITH_JPEG

#include <libjpeg/jpeglib.h>
#include <libjpeg/jerror.h>


static void _FClose(FILE *&pF)
{
  if (pF) {
    fclose(pF);
    pF = NULL;
  }
}

DEFINE_SCOPED_OBJECT(_scoped_file, _FClose)
typedef _scoped_file<FILE> scoped_file;

DEFINE_SCOPED_OBJECT(_scoped_ljobj, jpeg_destroy_decompress)
typedef _scoped_ljobj<jpeg_decompress_struct> scoped_ljobj;

DEFINE_SCOPED_OBJECT(_scoped_sjobj, jpeg_destroy_compress)
typedef _scoped_sjobj<jpeg_compress_struct> scoped_sjobj;

// Error handling structures.
typedef struct
{
  jpeg_error_mgr pub;
  jmp_buf buf;
} jem;

METHODDEF(void) jee(j_common_ptr cinfo)
{
  jem *pJem = (jem *)cinfo->err;
  longjmp(pJem->buf, 1);
}

METHODDEF(void) jom(j_common_ptr /* cinfo */)
{
}

int GetJpegPages(const char * /* pFileName */)
{
  return 1;
}

int GetJpegPageName(char *pPageName, int pageNameSize,
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

// There are a lot of sources where one can receive JPEG encoded
// image. To handle it we use the following scheme.


// 1. For every source we define structure to present the source in
// the single entity. Such a structure should have method isGood() to
// check if it has some data which might be good.

// C file.
struct FileFolder
{
  FileFolder(FILE *_file = NULL): file(_file) {};
  bool isGood() const { return file != NULL; }
  FILE *file;
};

// Array of bytes.
struct ArrayFolder
{
  ArrayFolder(): ptr(NULL), size(0) {}
  ArrayFolder(const uint8_t *_ptr, size_t _size): ptr(_ptr), size(_size) {}
  bool isGood() const { return ptr != NULL && size > 0; }
  const uint8_t *ptr;
  size_t        size;
};

// 2. Then we define a function which binds structure representing
// source to jpeg_decompress_structure.

// File source -> jpeg_decompress_structure.
inline static
void bindJpegDecompStruct(jpeg_decompress_struct *jsd, FileFolder &src)
{
  jpeg_stdio_src(jsd, src.file);
}

// Array source -> jpeg_decompress_structure.
inline static
void bindJpegDecompStruct(jpeg_decompress_struct *jsd, ArrayFolder &src)
{
  jpeg_mem_src(jsd, const_cast<uint8_t *>(src.ptr),
    static_cast<unsigned long>(src.size));
}

// 3. The main function to perform decompression of needed data.
// This function is parametrized by Image type, Source type and types
// of processing routines.
// Image parameter is needed only to work with both MinImg and const
// MinImg.
// SourceType is the class representing the source (defined in
// (1) section)
// Processing routines are:
// - decode-to-image which is run after jpeg decoder is prepared to
//   decode source;
// - decode-to-image which is run after decode-to-image has been run;
// - decode-finalize which is run in the end.

template<
  typename Image,
  typename SourceType,
  typename JpegDecodeToImage,
  typename JpegDecodeToProps,
  typename JpegDecodeFinalize>
static int DecodeJpegInfo
(
  Image                 *pImage,
  ExtImgProps           *pProps,
  SourceType            source,
  JpegDecodeToImage     decodeToImage,
  JpegDecodeToProps     decodeToProps,
  JpegDecodeFinalize    decodeFinalize
)
{
  // Check parameters.
  if (!source.isGood())
    return BAD_ARGS;

  // Instantiate decoding environment.
  jpeg_decompress_struct cinfo = {0};
  jem jerr;
  ::memset(&jerr, 0, sizeof(jerr));
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jee;
  jerr.pub.output_message = jom;

  if (setjmp(jerr.buf))
    return FILE_ERROR;

  jpeg_create_decompress(&cinfo);
  scoped_ljobj guard(&cinfo);

  bindJpegDecompStruct(&cinfo, source);

  jpeg_read_header(&cinfo, true);
  jpeg_start_decompress(&cinfo);

  // Run handlers
  if (decodeToImage != NULL)
    PROPAGATE_ERROR(decodeToImage(pImage, &cinfo));

  if (decodeToProps != NULL)
    PROPAGATE_ERROR(decodeToProps(pProps, &cinfo));

  decodeFinalize(&cinfo);

  return 0;
}

// Implementation of GetJpegPropsEx() GetJpegPropsExMem()

static
int decodeExtProps(ExtImgProps *pProps, jpeg_decompress_struct *jds)
{
  if (pProps)
  {
    pProps->iff = IFF_JPEG;
    pProps->comp = IFC_NONE;
    pProps->qty = 0;
    switch (jds->density_unit)
    {
    case 1:
      pProps->xDPI = jds->X_density;
      pProps->yDPI = jds->Y_density;
      break;
    case 2:
      pProps->xDPI = jds->X_density * 2.54f;
      pProps->yDPI = jds->Y_density * 2.54f;
      break;
    default:
      pProps->xDPI = 0.f;
      pProps->yDPI = 0.f;
    }
  }
  return NO_ERRORS;
}

static
int decodeImgProps(MinImg *pImage, jpeg_decompress_struct *jds)
{
  if (pImage && pImage->pScan0)
    return BAD_ARGS;

  if (pImage)
  {
    pImage->width = jds->output_width;
    pImage->height = jds->output_height;
    pImage->channels = jds->output_components;
    pImage->channelDepth = 1;
    pImage->format = FMT_UINT;
    pImage->stride = 0;
  }

  return NO_ERRORS;
}

int GetJpegPropsEx_FileSystem
(
  MinImg      *pImg,
  ExtImgProps *pProps,
  const char  *pFileName
)
{
  // Open file source.
  scoped_file pF(fopen(pFileName, "rb"));
  if (!pF)
    return FILE_ERROR;

  // Process file source source.
  PROPAGATE_ERROR(DecodeJpegInfo(pImg, pProps, FileFolder(pF),
      decodeImgProps, decodeExtProps, jpeg_abort_decompress));

  return NO_ERRORS;
}

int GetJpegPropsEx_Memory
(
  MinImg        *pImg,
  ExtImgProps   *pProps,
  const char    *pFileName
)
{
  uint8_t *ptr;
  size_t size;

  PROPAGATE_ERROR(ExtractMemoryLocation(pFileName, &ptr, &size));
  if (!ptr)
    return BAD_ARGS;

  PROPAGATE_ERROR(DecodeJpegInfo(pImg, pProps, ArrayFolder(ptr, size),
      decodeImgProps, decodeExtProps, jpeg_abort_decompress));

  return NO_ERRORS;
}

int GetJpegPropsEx
(
  MinImg      *pImg,
  ExtImgProps *pProps,
  const char  *pFileName
)
{
  int fileLocation = DeduceFileLocation(pFileName);
  switch (fileLocation)
  {
    case inFileSystem:
      return GetJpegPropsEx_FileSystem(pImg, pProps, pFileName);
    case inMemory:
      return GetJpegPropsEx_Memory(pImg, pProps, pFileName);
  }

  return NOT_IMPLEMENTED;
}

// Implementation of LoadJpeg() and LoadJpegMem()
static
int decodeImgLoad(const MinImg *pImg, jpeg_decompress_struct *jds)
{
  if (pImg->channelDepth != 1 || pImg->channels != jds->output_components)
    return BAD_ARGS;
  if (pImg->width < (int)jds->output_width || pImg->height < (int)jds->output_height)
    return BAD_ARGS;
  if (pImg->format != FMT_UINT)
    return BAD_ARGS;

  JSAMPROW ppBuf[1] = {NULL};
  for (int y = 0; y < (int)jds->output_height; y++)
  {
    ppBuf[0] = (JSAMPROW)(pImg->pScan0 + pImg->stride * y);
    jpeg_read_scanlines(jds, ppBuf, 1);
  }

  return NO_ERRORS;
}

inline static
int doNothingWithProps(const ExtImgProps *, const jpeg_decompress_struct *)
{
  return NO_ERRORS;
};

int LoadJpeg_FileSystem
(
  const MinImg *pImg,
  const char *pFileName
)
{
  // Open file source.
  if (!pImg || !pFileName)
    return BAD_ARGS;
  if (!pImg->pScan0)
    return BAD_ARGS;

  scoped_file pF(fopen(pFileName, "rb"));
  if (!pF)
    return FILE_ERROR;

  // Decode source.
  PROPAGATE_ERROR(DecodeJpegInfo(pImg, NULL, FileFolder(pF),
      decodeImgLoad, doNothingWithProps, jpeg_finish_decompress));

  return NO_ERRORS;
}

int LoadJpeg_Memory
(
  const MinImg  *pImg,
  const char    *pFileName
)
{
  uint8_t *ptr;
  size_t size;
  PROPAGATE_ERROR(ExtractMemoryLocation(pFileName, &ptr, &size));
  if (!ptr || size == 0)
    return BAD_ARGS;

  if (!pImg->pScan0)
    return BAD_ARGS;

  PROPAGATE_ERROR(DecodeJpegInfo(pImg, NULL, ArrayFolder(ptr, size),
      decodeImgLoad, doNothingWithProps, jpeg_finish_decompress));

  return NO_ERRORS;
}

int LoadJpeg
(
  const MinImg  *pImg,
  const char    *pFileName
)
{
  int fileLocation = DeduceFileLocation(pFileName);
  switch (fileLocation)
  {
    case inFileSystem:
      return LoadJpeg_FileSystem(pImg, pFileName);
    case inMemory:
      return LoadJpeg_Memory(pImg, pFileName);
  }

  return NOT_IMPLEMENTED;
}

int SaveJpegEx
(
  const char *pFileName,
  const MinImg *pImg,
  const ExtImgProps *pProps
)
{
  int fileLocation = DeduceFileLocation(pFileName);
  if (fileLocation != inFileSystem)
    return NOT_IMPLEMENTED;

  if (!pImg || !pFileName)
    return BAD_ARGS;
  if (!pImg->pScan0)
    return BAD_ARGS;
  if (pImg->channelDepth != 1 || pImg->format != FMT_UINT)
    return NOT_IMPLEMENTED;

  jpeg_compress_struct cinfo = {0};
  jem jerr = {{0}};
  cinfo.err = jpeg_std_error(&jerr.pub);
  jerr.pub.error_exit = jee;
  jerr.pub.output_message = jom;
  scoped_file pF(fopen(pFileName, "wb"));
  if (!pF)
    return FILE_ERROR;

  if (setjmp(jerr.buf))
    return FILE_ERROR;

  jpeg_create_compress(&cinfo);
  scoped_sjobj guard(&cinfo);
  jpeg_stdio_dest(&cinfo, pF);

  cinfo.image_width = pImg->width;
  cinfo.image_height = pImg->height;
  cinfo.input_components = pImg->channels;
  switch (cinfo.input_components)
  {
  case 1:
    cinfo.in_color_space = JCS_GRAYSCALE;
    break;
  case 3:
    cinfo.in_color_space = JCS_RGB;
    break;
  default:
    cinfo.in_color_space = JCS_UNKNOWN;
  }
  jpeg_set_defaults(&cinfo);
  if (pProps)
  {
    cinfo.density_unit = 1;
    cinfo.X_density = (short)(pProps->xDPI + .5);
    cinfo.Y_density = (short)(pProps->yDPI + .5);
  }
  int quality = 90;
  if (pProps && pProps->qty)
    quality = pProps->qty;
  jpeg_set_quality(&cinfo, quality, true);
  jpeg_start_compress(&cinfo, true);

  JSAMPROW ppBuf[1] = {NULL};
  for (int y = 0; y < pImg->height; y++)
  {
    ppBuf[0] = (JSAMPROW)(pImg->pScan0 + pImg->stride * y);
    jpeg_write_scanlines(&cinfo, ppBuf, 1);
  }

  jpeg_finish_compress(&cinfo);
  jpeg_destroy_compress(&cinfo);
  return NO_ERRORS;
}

#else // WITH_JPEG

int GetJpegPages(const char * /*pFileName*/)
{
  return NOT_SUPPORTED;
}

int GetJpegPageName(char * /*pPageName*/, int /*pageNameSize*/,
                    const char * /*pFileName*/, int /*page*/)
{
  return NOT_SUPPORTED;
}

int GetJpegPropsEx(MinImg * /*pImg*/, ExtImgProps * /*pProps*/,
                   const char * /*pFileName*/)
{
  return NOT_SUPPORTED;
}

int LoadJpeg(const MinImg * /*pImg*/, const char * /*pFileName*/)
{
  return NOT_SUPPORTED;
}

int SaveJpegEx(const char * /*pFileName*/, const MinImg * /*pImg*/,
               const ExtImgProps * /*pProps*/)
{
  return NOT_SUPPORTED;
}

#endif // WITH_JPEG
