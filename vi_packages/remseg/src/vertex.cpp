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


#include <remseg/vertex.h>
#include <remseg/edge_heap.h>

#include <minbase/crossplat.h>
THIRDPARTY_INCLUDES_BEGIN
#include <Eigen/Dense>
THIRDPARTY_INCLUDES_END

#include <cassert>

namespace vi { namespace remseg {

Vertex::Vertex(const Vertex * v)
: channelsNum(v->channelsNum)
, channelsSum(0)
, area(v->area)
, needsSort(v->needsSort)
, existence_flag(v->existence_flag)
, absorbent(v->absorbent)
{
  Initialize(channelsNum);
  for (int i = 0; i < channelsNum; i++)
    channelsSum[i] = v->channelsSum[i];
}

void Vertex::Initialize(int _channelsNum)
{
  assert(_channelsNum > 0);
  channelsNum = _channelsNum;
  channelsSum = new long double[channelsNum];
  for (int i = 0; i < channelsNum; i++)
    channelsSum[i] = 0;
}

Vertex::~Vertex()
{
  if (channelsSum != NULL)
    delete[] channelsSum;
}

bool Vertex::isConnectedTo(const Vertex *v) const
{
  assert(v);
  for (ConstJoint it = begin(); it != end(); it++)
    if (it->vertex == v)
      return true;
  return false;
}

Vertex *Vertex::nearestNeighbour() const
{
  if (empty())
    return 0;
  EdgeValue minDist = begin()->edge->value;
  Vertex *nn = begin()->vertex;
  for (ConstJoint it = begin(); it != end(); it++)
    if (it->edge->value < minDist)
    {
      minDist = it->edge->value;
      nn = it->vertex;
    }
  return nn;
}

bool Vertex::isSorted() const
{
  if (empty())
    return true;
  ConstJoint next = begin();
  next++;
  for (ConstJoint current = begin(); next != end(); current++, next++)
    if (*next < *current)
      return false;
  return true;
}

void Vertex::update(const double * pix)
{
  const int n = channelsNum;
  for (int i = 0; i < n; ++i)
    channelsSum[i] += pix[i];
  area += 1;
}

void Vertex::update(const uint8_t * pix)
{
  const int n = channelsNum;
  for (int i = 0; i < n; ++i)
    channelsSum[i] += (long double)pix[i];
  area += 1;
}

void Vertex::absorb(Vertex *v)
{
  assert(v and v->exists());
  v->existence_flag = false;
  v->absorbent = this;

  for (int i = 0; i < channelsNum; i++)
    channelsSum[i] += v->channelsSum[i];

  area += v->area;
}

Vertex *Vertex::getFinalAbsorbent() const
{
  Vertex *v;
  for (v = absorbent; v and v->absorbent; v = v->absorbent)
    ;
  return v;
}

Joint Vertex::orderedInsert(Vertex *v)
{
  assert(v  and  v != this);

  Joint it = begin();
  while (it != end() and it->vertex < v)
    it++;
  assert(it == end()  or  it->vertex != v);	// попытка повтыорного добавления вершины

  return insert(it, Link(v));
}

Json::Value Vertex::jsonLog() const
{
  Json::Value root;

  root["area"] = (int)area;
  root["mean"] = Json::arrayValue;
  for (int i = 0; i < channelsNum; ++i)
    root["mean"].append(double(channelsSum[i]) / area);
  return root;
}

}}	// ns vi::remseg