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


#include <remseg/image_map.h>
#include <remseg/image.h>
#include <vi_cvt/std/color.hpp>

#include <cstring>
#include <stack>
#include <unordered_map>
#include <cassert>

namespace vi { namespace remseg {

int ImageMap::numberOfSegments() const
{
  if (isEmpty())
    return -1;
  std::unordered_set<SegmentID> s;
  for (SegmentID *pix = pixels; pix < pixels + width * height; pix++)
    s.insert(*pix);
  return s.size();
}

ImageMap::ImageMap(const ImageMap &imageMap)
  : pixels(0)
  , width(0)
  , height(0)
{
  if (imageMap.isEmpty())
    return;
  Init(imageMap.getWidth(), imageMap.getHeight());
  memcpy(pixels, imageMap.pixels, width * height * sizeof(SegmentID));
  colorMap = imageMap.colorMap;
}

void DFS(Image const & image, SegmentID * pixels,
         uint32_t color, int i, int j, SegmentID id)
{
  std::stack<std::pair<int, int> > buffer;
  buffer.push({i, j});

  while (!buffer.empty())
  {
    auto pos2d = buffer.top();
    buffer.pop();

    int pos1d = pos2d.first * image.getWidth() + pos2d.second;
    if (pixels[pos1d] != -1)
      continue;

    pixels[pos1d] = id;
    for (int k = std::max(pos2d.first - 1, 0); k <= std::min(pos2d.first + 1, image.getHeight() -1); ++k)
    {
      for (int l = std::max(pos2d.second - 1, 0); l <= std::min(pos2d.second + 1, image.getWidth() - 1); ++l)
      {
        RGB curr_color;
        for (int m = 0; m < 3; ++m)
          curr_color[m] = image.getPixel(l, k)[m];
        if (vi::cvt::as_intcolor(curr_color, 3) == color)
          buffer.push({k, l});
      }
    }
  }
}

static void generate_color(uint8_t * color)
{
  color[0] = rand() % 256;
  color[1] = rand() % 256;
  color[2] = rand() % 256;
}

ImageMap::ImageMap(const char *fileName)
  : pixels(0)
  , width(0)
  , height(0)
{
  Image image(fileName, true);
  Init(image.getWidth(), image.getHeight());
  for (int i = 0; i < image.getWidth() * image.getHeight(); ++i)
    pixels[i] = -1;

  SegmentID max_id = 0;

  for (int i = 0; i < image.getHeight(); ++i)
  {
    for (int j = 0; j < image.getWidth(); ++j)
    {
      if (pixels[i * width + j] > -1)
        continue;

      RGB color;
      for (int k = 0; k < 3; ++k)
        color[k] = image.getPixel(j, i)[k];
      uint32_t int_color = vi::cvt::as_intcolor(color, 3);
      DFS(image, pixels, int_color, i, j, max_id);

      colorMap[max_id][0] = color[0];
      colorMap[max_id][1] = color[1];
      colorMap[max_id][2] = color[2];
      max_id++;
    }
  }
}

ImageMap::ImageMap(int w, int h)
  : pixels(0)
  , width(0)
  , height(0)
{
  Init(w, h);
  for (int i = 0; i < w * h; ++i)
    pixels[i] = i;
}

void ImageMap::Init(int init_width, int init_height)
{
  if (init_width < 1 or init_height < 1)
    throw std::invalid_argument("bad image size");

  if (!isEmpty())
    throw std::runtime_error("image map not empty");

  if (!(pixels = new SegmentID [init_width * init_height]))
    throw std::runtime_error("cannot allocate pixels");

  width = init_width;
  height = init_height;
}

bool ImageMap::isBorder(int x, int y) const
{
  assert(!isEmpty() and x >= 0 and x < width and y >= 0 and y < height);
  bool answer = false;
  SegmentID self = getSegment(x, y);
  answer |= x > 0		and	getSegment(x-1, y) != self;
  answer |= x < width-1	and 	getSegment(x+1, y) != self;
  answer |= y > 0		and 	getSegment(x, y-1) != self;
  answer |= y < height-1	and 	getSegment(x, y+1) != self;
  return answer;
}

ImageMap::~ImageMap()
{
  if (pixels)
    delete[] pixels;
  pixels = 0;
  width = height = 0;
}

std::unordered_map<SegmentID, SegmentStat> ImageMap::getSegmentStats() const
{
  std::unordered_map<SegmentID, SegmentStat> stats;

  for (int i = 0; i < height; ++i)
  {
    for (int j = 0; j < width; ++j)
    {
      const SegmentID id = getSegment(j, i);
      if (stats.find(id) == stats.end())
      {
        SegmentStat s;
        s.rect = MinRect(j, i, 1, 1);
        s.area = 0;
        s.leftTopPoint = {j, i};
        stats.insert({id, s});
      }

      auto & s = stats[id];
      auto dump_rect = s.rect;
      s.rect.x = std::min(j, dump_rect.x);
      s.rect.y = std::min(i, dump_rect.y);
      s.rect.width = std::max(j + 1, dump_rect.x + dump_rect.width) - s.rect.x;
      s.rect.height = std::max(i + 1, dump_rect.y + dump_rect.height) - s.rect.y;
      s.area += 1;

      if (j <= s.leftTopPoint.first)
      {
        if (i <= s.leftTopPoint.second)
          s.leftTopPoint = {j, i};
      }

      for (int k = std::max(0, i-1); k <= i; ++k)
      {
        for (int l = std::max(0, j-(k -i + 1)); l <= j; ++l)
        {
          const SegmentID id_adj = getSegment(l, k);
          if (id_adj != id) {
            stats[id].neighbours.insert(id_adj);
            stats[id_adj].neighbours.insert(id);
          }
        }
      }
    }
  }

  return stats;
}

std::unordered_map<SegmentID, RGB> ImageMap::getColorMap(bool check_neighbours) const
{
  if (!check_neighbours)
    return colorMap;

  std::unordered_set<uint32_t> colors;

  auto stats = getSegmentStats();
  for (auto const & stat : stats)
  {
    SegmentID id = stat.first;
    uint8_t color[3];
    if (colorMap.find(id) == colorMap.end())
      generate_color(color);
    else
    {
      color[0] = colorMap[id][0];
      color[1] = colorMap[id][1];
      color[2] = colorMap[id][2];

      //colors.insert(vi::cvt::as_intcolor(color, 3));
    }

    for (auto const & neighb: stat.second.neighbours)
    {
      if (colorMap.find(neighb) != colorMap.end())
        colors.insert(vi::cvt::as_intcolor(colorMap[neighb], 3));
    }

    while (colors.find(vi::cvt::as_intcolor(color, 3)) != colors.end())
      generate_color(color);

    //colors.insert(vi::cvt::as_intcolor(color, 3));

    colorMap[id][0] = color[0];
    colorMap[id][1] = color[1];
    colorMap[id][2] = color[2];

    colors.clear();
  };

  return colorMap;
}

int sumNeighboursNum(std::unordered_map<SegmentID, SegmentStat> const & stats)
{
  int sum = 0;
  for (auto const & stat : stats)
    sum += stat.second.neighbours.size(); // TODO: actually, here are duplicates
  return sum;
}

}}	// ns vi::remseg