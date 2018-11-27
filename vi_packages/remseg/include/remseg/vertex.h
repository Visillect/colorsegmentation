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

#include <list>
#include <array>
#include <vector>
#include <validate_json/validate_json.h>

namespace vi { namespace remseg {

struct Edge;
struct Link;

class Vertex: public std::list<Link>	// поддерживается упорядочивание по Link.vertex
{
public:
	typedef iterator Joint;
	typedef const_iterator ConstJoint;

	int channelsNum;

	long double *channelsSum;
	long area;

	bool needsSort;

	bool isBlocked;

	Vertex()
		: channelsNum(0)
		, channelsSum(0)
		, area(0)
		, needsSort(false)
		, isBlocked(false)
		, existence_flag(true)
		, absorbent(0)
		{ }

    Vertex(const Vertex* v);

	virtual ~Vertex();

	virtual void Initialize(int _channelsNum);
	virtual void update(const double * pix);
	virtual void update(const uint8_t * pix);
	virtual void absorb(Vertex *to_be_absorbed);

	void clearAbsorbent() { absorbent = 0; }

	// возвращает последний ненулевой поглотитель в цепочке (absorbent) - (absorbent->absorbent) - (absorbent->absorbent->absorbent) - ...
	// или 0, если absorbent == 0
	Vertex *getFinalAbsorbent() const;

	bool exists() const
		{ return existence_flag; }

	// Упорядоченная вставка ("соединение" с v). Внимание! При этом поля edge и joint в соответствующем линке не устанавливаются!
	// Возвращает позицию v
	Joint orderedInsert(Vertex *v);

	Vertex *nearestNeighbour() const;

	bool isSorted() const;

	bool isConnectedTo(const Vertex *v) const;

    virtual Json::Value jsonLog() const;

private:
	bool existence_flag;	// поглощена ли вершина? (true - нет, false - да)
	// поглотитель (!= 0, если вершина была поглощена, а привязка сегментов к изображению не обновилась, если же вершина не поглощена или привязка
	// актуальна, = 0)
	Vertex *absorbent;
};

typedef Vertex::iterator Joint;
typedef Vertex::const_iterator ConstJoint;

struct Link
{
	Vertex *vertex;		// вершина, соединенная с текущей (текущая вершина - это та, которая содержит Link)
	Edge *edge;		// соединяющее с vertex ребро. Должно совпадать с joint->edge
	Joint joint;		// линк в vertex, указывающий на текущую вершину

	Link(Vertex *v)
		: vertex(v)
		{ }

	Link(Vertex *v, Edge *e, Joint _joint)
		: vertex(v)
		, edge(e)
		, joint(_joint)
		{ }

	bool operator== (const Link &link) const
		{ return vertex == link.vertex; }

	bool operator> 	(const Link &link) const
		{ return vertex > link.vertex; }

	bool operator< 	(const Link &link) const
		{ return vertex < link.vertex; }
};

}}	// ns vi::remseg