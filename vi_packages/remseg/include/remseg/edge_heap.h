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

#include <remseg/vertex.h>

namespace vi { namespace remseg {

typedef double EdgeValue;

struct Edge
{

  Vertex *a, *b;
  Joint jointA;	// линк в `a`, соединяющий с `b`
  Joint jointB;	// линк в `b`, соединяющий с `a`
  EdgeValue value;

  Edge()
    : a(0)
    , b(0)
    , value(0)
  { }

  Edge(Vertex *_a, Vertex *_b, EdgeValue _value, Joint _jointA, Joint _jointB)
    : a(_a)
    , b(_b)
    , jointA(_jointA)
    , jointB(_jointB)
    , value(_value)
  { }

  bool operator< (const Edge &edge) const { return value < edge.value; }
  bool operator> (const Edge &edge) const { return edge < (*this); }
  bool operator<= (const Edge &edge) const { return value <= edge.value; }
  bool operator>= (const Edge &edge) const { return edge <= (*this); }

  void update() { jointA->edge = jointB->edge = this; }
};

class EdgeHeap
{
public:
  static constexpr const int degree = 8;

  EdgeHeap(int init_max_size);
  ~EdgeHeap();

  void push(Edge edge);
  void remove(Edge *edge);
  void update(Edge *edge, EdgeValue newValue);

  Edge *top();

  bool isEmpty() const
  { return edges == 0 or size == 0; }

  int getSize() const
  { return size; }

  bool isConsistent() const;

private:
  Edge *edges;
  int size;
  int max_size;

  void swap(Edge *e1, Edge *e2);
  void siftDown(Edge *edge);
  void siftUp(Edge *edge);
  bool goodEdge(const Edge *edge) const
  { return edge != 0 and !isEmpty() and edge >= edges and edge < edges + size; }

  EdgeHeap(const EdgeHeap &)
  { }

  bool __isConsistent(int i) const;
};

}}	// ns vi::remseg