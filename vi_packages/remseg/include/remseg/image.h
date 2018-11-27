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


#pragma once

#include <ostream>
#include <vector>

#include <minbase/minimg.h>

#include <cassert>

namespace vi { namespace remseg {

struct Point
{
  int x, y;
  Point(int _x = 0, int _y = 0)
    : x(_x)
    , y(_y)
  { }
};

typedef std::vector<Point> Points;

class Image
{
public:
  Image();
  Image(int _width, int _height, int _channels = 3, int _channelDepth = 1, MinFmt _format = FMT_UINT, bool free_minimg_in_destructor = true);
  Image(const MinImg *_minimg, bool free_minimg_in_destructor);
  Image(const char *fileName, bool free_minimg_in_destructor = true);
  ~Image();

  bool WriteToFile(const char *TIFF_FileName);

  uint8_t *operator() (int x, int y);
  uint8_t *operator() (const Point &pt)
  { return (*this)(pt.x, pt.y); }

  const uint8_t *getPixel(int x, int y) const;
  const uint8_t *getPixel(const Point &pt) const
  { return getPixel(pt.x, pt.y); }

  double getChannel(int x, int y, int channel) const
  {
    assert(channel >= 0 && channel < minimg.channels);
    return channelToDouble(getPixel(x, y), channel);
  }

  double getChannel(const uint8_t *pix, int channel) const
  {
    assert(channel >= 0 && channel < minimg.channels);
    return channelToDouble(pix, channel);
  }

  bool ConvertToUINT8(Image &image_uint8, bool image_uint8_needsFree) const;

  bool Normalize(Image &result);

  bool isEmpty() const
  { return width <= 0 || height <= 0 || pixSize == 0 || minimg.pScan0 == 0; }

  bool operator !() const
  { return isEmpty(); }

  void SetNeedFree(bool _needsFree)
  { needsFree = _needsFree; }

  int getWidth() const
  { return width; }

  int getHeight() const
  { return height; }

  int getChannelsNum() const
  { return minimg.channels; }

  int getChannelDepth() const
  { return minimg.channelDepth; }

  MinFmt getFormat() const
  { return minimg.format; }

  const MinImg *getMinImg() const
  { return &minimg; }

private:
  bool needsFree;		// need to free MinImg bitmap in destructor
  int width;
  int height;
  int pixSize;		// size of 1 pixel in bytes (minimg.channelDepth * minimg.channels)
  MinImg minimg;

  double (*channelToDouble)(const uint8_t *, int);	// (pixel_start, channel)
  void InitChannelToDouble();
  void Assign(const MinImg *_minimg, bool free_minimg_in_destructor);
};

inline uint8_t *Image::operator() (int x, int y)
{
  assert(!isEmpty() and x >= 0 and x < width and y >= 0 and y < height);
  return minimg.pScan0 + y*minimg.stride + x*pixSize;
}

inline const uint8_t *Image::getPixel(int x, int y) const
{
  assert(!isEmpty() and x >= 0 and x < width and y >= 0 and y < height);
  return minimg.pScan0 + y*minimg.stride + x*pixSize;
}

}}	// ns vi::remseg

std::ostream &operator<< (std::ostream &s, vi::remseg::Point pt);
