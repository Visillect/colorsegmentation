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


#include <remseg/utils.h>

#include <minimgapi/minimgapi-helpers.hpp>
#include <vi_cvt/std/exception_macros.hpp>
#include <validate_json/validate_json.h>

namespace vi { namespace remseg {

void visualize(MinImg * imgres, ImageMap const & imageMap,
               std::set<std::pair<int, int> > const & blocklist)
{
  THROW_ON_MINERR(NewMinImagePrototype(imgres, imageMap.getWidth(),
                                       imageMap.getHeight(), 3, TYP_UINT8));

  auto colors = imageMap.getColorMap(true);
  std::unordered_map<SegmentID, SegmentStat> stats;
  if (!blocklist.empty())
    stats = imageMap.getSegmentStats();

  uint8_t black[3] = {0,0,0};
  for (int y = 0; y < imageMap.getHeight(); y++)
  {
    RGB* line = reinterpret_cast<RGB*>(GetMinImageLine(imgres, y));
    for (int x = 0; x < imageMap.getWidth(); x++)
    {
      const int idx = imageMap(x, y);
      bool paintItBlack = false;
      if (!blocklist.empty())
      {
        if (blocklist.find(stats[idx].leftTopPoint) != blocklist.end())
          paintItBlack = true;
      }
      if (paintItBlack)
        memcpy(line+x, &black, sizeof(RGB));
      else
        memcpy(line+x, &colors[idx], sizeof(RGB));
    }
  }
}

void readLockList(std::set<std::pair<int, int> > & lockList,
                   bool & lockingPolicy,
                   std::string const & filename)
{
  Json::Value root = vi::json_from_file(filename);
  lockList.clear();
  for (auto& point : root["vertices"])
    lockList.insert({point["x"].asInt(), point["y"].asInt()});
  lockingPolicy = root["blocking_policy"].asBool();
}

void saveLockList(std::string const & filename,
                   std::set<std::pair<int, int> > const & lockList,
                   bool lockingPolicy)
{
  Json::Value root;
  root["vertices"] = Json::arrayValue;
  for (auto const& point : lockList)
  {
    Json::Value v;
    v["x"] = point.first;
    v["y"] = point.second;
    root["vertices"].append(v);
  }
  root["blocking_policy"] = lockingPolicy;
  vi::json_to_file(filename, root);
}

}}	// ns vi::remseg
