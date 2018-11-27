/*
Copyright (c) 2010-2018 Timur M. Khanipov <khanipov@gmail.com>.
Copyright (c) 2018, Visillect Service LLC. All rights reserved.
Developed for Kharkevich Institute for Information Transmission Problems of the
              Russian Academy of Sciences (IITP RAS).

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
#include <cstdlib>
#include <fstream>

#include <remseg/image.h>
#include <minimgio/minimgio.h>
#include <minimgapi/minimgapi.h>
#include <minbase/minresult.h>

std::ostream &operator<< (std::ostream &s, vi::remseg::Point pt)
{
  return s << '(' << pt.x << ';' << pt.y << ')';
}

namespace vi { namespace remseg {

bool Image::WriteToFile(const char *fileName)
{
  if (isEmpty())
    throw std::runtime_error("Image is empty");

  return SaveMinImage(fileName, &minimg) == NO_ERRORS;
}

Image::Image(int _width, int _height, int _channels, int _channelDepth, MinFmt _format, bool _needsFree)
  : needsFree(false)
  , pixSize(0)
  , channelToDouble(0)
{
  memset((char *) &minimg, 0, sizeof(minimg));

  MinImg __minimg;
  memset((char *) &__minimg, 0, sizeof(__minimg));

  __minimg.width = _width;
  __minimg.height = _height;
  __minimg.channels = _channels;
  __minimg.channelDepth = _channelDepth;
  __minimg.format = _format;

  int res = AllocMinImage(&__minimg);
  if (res != NO_ERRORS)
    throw std::runtime_error("AllocMinImage() returned " + std::to_string(res));

  Assign(&__minimg, _needsFree);
}

Image::Image()
  : needsFree(false)
  , width(0)
  , height(0)
  , pixSize(0)
  , channelToDouble(0)

{
  memset((char *) &minimg, 0, sizeof(minimg));
}

void Image::Assign(const MinImg *_minimg, bool _needsFree)
{
  needsFree = _needsFree;

  if (_minimg == 0)
    throw std::runtime_error("NULL MinImg pointer");

  if (_minimg->format != FMT_UINT && _minimg->format != FMT_INT && _minimg->format != FMT_REAL)
    throw std::runtime_error("MinImg format " + std::to_string(_minimg->format) + " is not supported");

  minimg = *_minimg;

  // if (minimg.pScan0 == 0)
  //   LOG_WARNING("MinImg.pScan0 is NULL");

  width = minimg.width;
  height = minimg.height;
  pixSize = minimg.channelDepth * minimg.channels;

  InitChannelToDouble();

  // LOG_DEBUG("Assign() call successful: Image width = " << width << "; height = " << height << "; pixSize = " << pixSize << "; stride = " << minimg.stride);
}

Image::Image(const MinImg *_minimg, bool _needsFree)
{
  Assign(_minimg, _needsFree);
}

Image::Image(const char *fileName, bool __needsFree)
  : needsFree(false)
{
  int res = GetMinImageFilePages(fileName);

  // if (res > 1)
  // {
  //   LOG_WARNING("More than one page in TIFF file. Only the first page will be used.");
  // }
  /*else*/ if (res <= 0)
    throw std::runtime_error("GetImageFilePages() returned " + std::to_string(res));

  MinImg __minimg;
  memset((char *) &__minimg, 0, sizeof(__minimg));
  if ((res = GetMinImageFileProps(&__minimg, fileName)) != NO_ERRORS)
    throw std::runtime_error("GetMinImageFileProps() returned " + std::to_string(res));

  if (__minimg.format != FMT_UINT && __minimg.format != FMT_INT && __minimg.format != FMT_REAL)
    throw std::runtime_error("Unknown MinImg.format " + std::to_string(__minimg.format));

  if ((res = AllocMinImage(&__minimg)) != NO_ERRORS)
    throw std::runtime_error("AllocMinImage() returned " + std::to_string(res));

  if ((res = LoadMinImage(&__minimg, fileName)) != NO_ERRORS)
    throw std::runtime_error("LoadMinImage() returned " + std::to_string(res));

  Assign(&__minimg, __needsFree);
}

Image::~Image()
{
  if (needsFree)
  {
    if (minimg.pScan0)
    {
      // LOG_DEBUG("Deallocating bitmap");
      FreeMinImage(&minimg);
      // LOG_DEBUG("FreeMinImage() finished");
    }
    // else
      // LOG_WARNING("needsFree is set but MinImg.pScan0 is NULL");
  }
  memset((char *) &minimg, 0, sizeof(minimg));
}

bool Image::Normalize(Image &n_img)
{
  if (!n_img.isEmpty())
    throw std::runtime_error("Destination image is not empty!");

  if (isEmpty())
    throw std::runtime_error("Cannot normalize empty image");

  int ch = getChannelsNum();
  Image res(width, height, getChannelsNum(), 8, FMT_REAL, false);

  for (int i = 0; i < width; i++)
    for (int j = 0; j < height; j++)
    {
      uint8_t *pix = (*this)(i,j);
      double *dst = (double *) res(i,j);
      double sum = 0.0;
      for (int k = 0; k < ch; k++)
        sum += getChannel(pix, k);
      for (int k = 0; k < ch; k++)
        dst[k] = getChannel(pix, k) / sum;
    }

  n_img.Assign(res.getMinImg(), true);

  if (!n_img)
    throw std::runtime_error("Failed to assign, memory will leak :(");

  return true;
}

bool Image::ConvertToUINT8(Image &img_uint8, bool img_uint8_needsFree) const
{
  if (!img_uint8.isEmpty())
    throw std::runtime_error("Destination image is not empty!");

  if (isEmpty())
    throw std::runtime_error("Cannot convert empty image");

  MinImg minimg_uint8;
  minimg_uint8.width = width;
  minimg_uint8.height = height;
  minimg_uint8.stride = 0;
  minimg_uint8.channels = minimg.channels;
  minimg_uint8.channelDepth = 1;
  minimg_uint8.format = FMT_UINT;
  minimg_uint8.pScan0 = 0;

  int res;
  if ((res = AllocMinImage(&minimg_uint8)) != NO_ERRORS)
    throw std::runtime_error("AllocMinImage() returned " + std::to_string(res));

  img_uint8.Assign(&minimg_uint8, img_uint8_needsFree);

  for (int k = 0; k < minimg.channels; k++)
  {
    double channelMin, channelMax;
    channelMin = channelMax = getChannel(0, 0, k);
    for (int i = 0; i < width; i++)
      for (int j = 0; j < height; j++)
      {
        double channelValue = getChannel(i, j, k);
        if (channelValue < channelMin)
          channelMin = channelValue;
        if (channelValue > channelMax)
          channelMax = channelValue;
      }
    bool const_channel = channelMax - channelMin < 1.0e-8;
    for (int i = 0; i < width; i++)
      for (int j = 0; j < height; j++)
        img_uint8(i,j)[k] = const_channel? 0 : uint8_t( (getChannel(i, j, k) - channelMin) * 255.0 / (channelMax - channelMin) );
  }
  return true;
}

template<typename T>
double __channelToDouble(const uint8_t *pix, int channel)
{
  return *((T*)(pix + channel*sizeof(T)));
}

void Image::InitChannelToDouble()
{
  channelToDouble = 0;
  if (isEmpty())
  {
    // LOG_WARNING("Image is empty. channelToDouble will be set to NULL");
    return;
  }
  if (minimg.format == FMT_UINT)
    switch (minimg.channelDepth)
    {
    case 1:
      channelToDouble = __channelToDouble<uint8_t>;
      break;
    case 2:
      channelToDouble = __channelToDouble<uint16_t>;
      break;
    case 4:
      channelToDouble = __channelToDouble<uint32_t>;
      break;
    }
  else if (minimg.format == FMT_INT)
    switch (minimg.channelDepth)
    {
    case 1:
      channelToDouble = __channelToDouble<int8_t>;
      break;
    case 2:
      channelToDouble = __channelToDouble<int16_t>;
      break;
    case 4:
      channelToDouble = __channelToDouble<int32_t>;
      break;
    }
  else if (minimg.format == FMT_REAL)
    switch (minimg.channelDepth)
    {
    case 4:
      channelToDouble = __channelToDouble<float>;
      break;
    case 8:
      channelToDouble = __channelToDouble<double>;
      break;
    }
  else
    throw std::runtime_error("Unsupported image format " + std::to_string(minimg.format));

  if (channelToDouble == 0)
    throw std::runtime_error("Could not find a matching channelToDouble function for format " + std::to_string(minimg.format) +
                             " and channel depth " + std::to_string(minimg.channelDepth));
}

}}	// ns vi::remseg