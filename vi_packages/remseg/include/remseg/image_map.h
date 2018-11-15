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

#include <mingeo/mingeo.h>

#include <map>
#include <unordered_map>
#include <unordered_set>

#include <cassert>

namespace vi { namespace remseg {

typedef uint8_t RGB[3];
typedef int SegmentID;

struct Point
{
  int x, y;
  Point(int _x = 0, int _y = 0)
    : x(_x)
    , y(_y)
  { }
};

struct SegmentStat
{
  MinRect rect;
  int area;
  std::pair<int, int> leftTopPoint;
  std::unordered_set<SegmentID> neighbours;
};

class ImageMap
{
public:
  ImageMap(int _width, int _height);
  ImageMap(const char *fileName);
  ImageMap(const ImageMap &);
  ~ImageMap();

  SegmentID &operator() (int x, int y) const;
  SegmentID &operator() (const Point &pt) const { return (*this)(pt.x, pt.y); }

  SegmentID getSegment(int x, int y) const;
  SegmentID getSegment(const Point &pt) const { return getSegment(pt.x, pt.y); }

  bool isBorder(int x, int y) const;
  bool isEmpty() const { return pixels == 0; }

  int getWidth() const  { return width; }
  int getHeight() const { return height; }

  int numberOfSegments() const;

  std::unordered_map<SegmentID, SegmentStat> getSegmentStats() const;
  std::unordered_map<SegmentID, RGB> getColorMap(bool check_neighbours=false) const;

private:
  void Init(int init_width, int init_height);

  SegmentID *pixels;

  int width;
  int height;

  mutable std::unordered_map<SegmentID, RGB> colorMap;
};

inline SegmentID &ImageMap::operator() (int x, int y) const
{
  assert(!isEmpty() and x >= 0 and x < width and y >= 0 and y < height);
  return *(pixels + y*width + x);
}

inline SegmentID ImageMap::getSegment(int x, int y) const
{
  assert(!isEmpty() and x >= 0 and x < width and y >= 0 and y < height);
  return *(pixels + y*width + x);
}

int sumNeighboursNum(std::unordered_map<SegmentID, SegmentStat> const & stats);

}}	// ns vi::remseg