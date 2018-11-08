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


#include <remseg/edge_heap.h>

#include <cassert>

namespace vi { namespace remseg {

bool EdgeHeap::isConsistent() const
{
  if (isEmpty())
    return true;
  return __isConsistent(0);
}

bool EdgeHeap::__isConsistent(int root) const
{
  assert(root >= 0);
  bool result = true;
  for (int i = root * degree + 1; i <= root * degree + degree && i < size; i++)
    result &= edges[root] <= edges[i] && __isConsistent(i);
  return result;
}

EdgeHeap::EdgeHeap(int init_max_size)
  : edges(0)
  , size(0)
  , max_size(init_max_size)
{
  if (!(edges = new Edge[max_size]))
    throw std::runtime_error("Failed to allocate edges");
}

EdgeHeap::~EdgeHeap()
{
  // assert(isConsistent());
  if (edges)
    delete[] edges;
}

void EdgeHeap::update(Edge *edge, EdgeValue newValue)
{
  assert(goodEdge(edge));

  if (newValue < edge->value)
  {
    edge->value = newValue;
    siftUp(edge);
  }
  else
  {
    edge->value = newValue;
    siftDown(edge);
  }
  // assert(isConsistent());
}

Edge *EdgeHeap::top()
{
  // assert(isConsistent());
  return isEmpty() ? 0 : &edges[0];
}

void EdgeHeap::remove(Edge *edge)
{
  assert(goodEdge(edge));

  if (edge == &edges[size - 1])	// удаляется последнее ребро
  {
    size--;
    return;
  }

  EdgeValue oldValue = edge->value;
  *edge = edges[size - 1];
  edge->update();
  size--;

  if (edge->value < oldValue)
    siftUp(edge);
  else
    siftDown(edge);

  // assert(isConsistent());
}

void EdgeHeap::push(Edge edge)
{
  assert(edges  and   size != max_size);

  edges[size] = edge;
  edges[size].update();
  size++;
  siftUp(&edges[size-1]);
  // assert(isConsistent());
}

void EdgeHeap::swap(Edge *e1, Edge *e2)
{
  Edge tmp = *e2;
  *e2 = *e1;
  *e1 = tmp;
  e1->update();
  e2->update();
}

void EdgeHeap::siftUp(Edge *edge)
{
  int node = edge - edges;
  int parent = (node - 1) / degree;

  while (node > 0 and edges[parent] > edges[node])
  {
    swap(&edges[node], &edges[parent]);
    node = parent;
    parent = (node - 1) / degree;
  }
}

void EdgeHeap::siftDown(Edge *edge)
{
  int node = edge - edges;

  int left 	= node * degree + 1;
  int right = std::min(node * degree + degree, size - 1);

  while (left < size)
  {
    int min = left;
    for (int i = left + 1; i <= right; i++)
      if (edges[i] <= edges[min])
        min = i;

    if (edges[node] > edges[min])
    {
      swap(&edges[min], &edges[node]);
      node = min;
    }
    else
      return;

    left 	= node * degree + 1;
    right = std::min(node * degree + degree, size - 1);
  }
}

}}	// ns vi::remseg