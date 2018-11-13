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

#include <fstream>
#include <iostream>

#include <remseg/image.h>
#include <remseg/image_map.h>
#include <remseg/distance_func.h>

#include <i8r/i8r.h>

#include <remseg/utils.h>

#include <minimgapi/minimgapi-helpers.hpp>
#include <minimgapi/imgguard.hpp>
#include <validate_json/validate_json.h>

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <numeric>
#include <vector>
#include <set>

#include <cassert>

std::string dist_to_string(const double a_value)
{
  char buff[100];
  snprintf(buff, sizeof(buff), "%09.4f", a_value);
  std::string str = buff;
  str[4] = '_';
  return str;
}

namespace vi { namespace remseg {

enum {BLOCK_SEGMENTS, BLOCK_EDGES};
enum {MERGE_OK, MERGE_BREAK};

template<typename T>
class Segmentator
{
  static_assert(std::is_base_of<Vertex, T>::value, "T should be derived from Vertex");

public:

  typedef EdgeValue (*ErrorFunction)(const T *v);
  typedef EdgeValue (*DistanceFunction)(const T *v1, const T *v2);

  Segmentator(const Image &image,
              EdgeValue (*ef)(const T *v) = error_function_replaceme,
              EdgeValue (*df)(const T *v1, const T *v2) = student_distance,
              bool _normalize = false);

  Segmentator(const Image &image,
              const ImageMap &_imageMap,
              EdgeValue (*ef)(const T *v) = error_function_replaceme,
              EdgeValue (*df)(const T *v1, const T *v2) = student_distance,
              std::set<std::pair<int, int> > const & _blockList = {},
              bool _blocking_policy = BLOCK_SEGMENTS,
              bool _normalize = false);

  ~Segmentator();

  EdgeValue calcError(const T* v) const;

  int numberOfSegments() const { return vertexNum; }
  int numberOfEdges() const { return edgeHeap? edgeHeap->getSize() : 0; }

  T *pointToVertex(const Point &pt)
  {
    updateMapping();
    return vertices + (*imageMap)(pt.x, pt.y);
  }

  void connect(T *a, T *b, bool dummy = false);
  void merge(T *absorbent, T *v);
  bool areConnected(const T *v1, const T *v2) const;

  void setBreakpoint(T *v) { breakpoint = v; }
  void setBreakpoint(SegmentID id) { setBreakpoint(vertices + id); }
  void unsetBreakpoint() { breakpoint = 0; }

  bool isBorder(int x, int y);

  void mergeNext(bool do_update_mapping = true);

  // Два mergeToLimitCycle, между которыми происходит соединение заблокированных регионов с незаблокированными
  int mergeToLimit(EdgeValue distanceLimit, EdgeValue errorLimit, int segmentsLimit,
                   i8r::PLogger dbg = nullptr, int debug_iter = 1, int maxSegments=-1);

  // выполняет mergeNext(), пока вес ребра меньше distanceLimit, а общее число сегментов больше segmentsLimit. В случае прерывания
  // по breakpoint вернет MERGE_BREAK, иначе MERGE_OK.
  // После завершения вызывается updateMapping()
  // Если в качестве какого-то параметра установить отрицательное значение, то он учитываться не будет.
  int mergeToLimitCycle(EdgeValue distanceLimit, EdgeValue errorLimit, int segmentsLimit,
                        i8r::PLogger dbg = nullptr, int debug_iter = 1, int maxSegments=-1);

  T *mergeBackground(int areaLimit);

  void removeHoles(T *background);

  void updateMapping(bool check_neighbours=false);
  void enumerateSegments(std::map<T *, int> &enumerationMap);

  const Edge *top() const { return edgeHeap->top(); }

  bool dumpSegment(T *v, Points &points);
  bool dumpAllSegments(const char *fileName, const Image &image);

  const ImageMap &getImageMap() const { return *imageMap; }
  SegmentID getId(T *v) const { return v - vertices; }

  bool isEmpty() const { return vertices == 0 or edgeHeap == 0; }
  bool isConsistent() const { return edgeHeap->isConsistent(); }
  int getStepNumber() const { return stepNumber; }

  T *vertexById(SegmentID id) const { return vertices + id; }

  std::ofstream *mergeLogStream = nullptr;

  void saveLog(std::string const & filename);

  DistanceFunction getDistanceFunction() const { return distance_function; }

protected:
  ErrorFunction error_function;
  DistanceFunction distance_function;

  ImageMap *imageMap = nullptr;
  EdgeHeap *edgeHeap = nullptr;
  T *vertices = nullptr;
  T *breakpoint = nullptr;

  bool needUpdateMapping = false; // ?

  int channelsNum;

  int vertexNum = 0;
  int sizeOfVertices = 0;

  unsigned int max_neighbours = 0;

  int *mergeAuxArray = nullptr;
  int stepNumber = 0;

  std::set<std::pair<int, int> > blockList;
  bool blocking_policy;

  EdgeValue errorAccumulator = 0;
  bool normalize;

  void initialize(int maxNumberOfVertices, int maxNumberOfEdges);
  bool goodVertex(const T *v) const
  { return v != 0 and !isEmpty() and v >= vertices and v < vertices + sizeOfVertices and v->exists(); }

  void createAdjacencyGraph(const Image &image);
  void createAdjacencyGraph(const Image &image,
                            const ImageMap &imageMap);

};

template<typename T>
EdgeValue Segmentator<T>::calcError(const T *v) const
{
  EdgeValue error = error_function(v);
  return error;
}

template<typename T>
void Segmentator<T>::enumerateSegments(std::map<T *, int> &emap)
{
  emap.clear();
  int n = 0;
  for (int i = 0; i < sizeOfVertices; i++)
    if (vertices[i].exists())
      emap.insert(std::make_pair<T *, int>(&vertices[i], n++));
}

template<typename T>
bool Segmentator<T>::dumpAllSegments(const char *fileName, const Image &image)
{
  std::ofstream f(fileName);
  if (!f)
    return false;
  updateMapping();
  for (int i = 0; i < image.getWidth(); i++)
    for (int j = 0; j < image.getHeight(); j++)
    {
      const uint8_t *pixel = image.getPixel(i, j);
      for (int k = 0; k < image.getChannelsNum(); k++, pixel++)
        f << *pixel << ' ';
      f << imageMap->getSegment(i,j) << std::endl;
      if (!f)
        return false;
    }
  return true;
}

template<typename T>
bool Segmentator<T>::isBorder(int x, int y)
{
  updateMapping();
  return imageMap->isBorder(x, y);
}

template<typename T>
void Segmentator<T>::updateMapping(bool check_neighbours)
{
  if (!needUpdateMapping or isEmpty())
    return;

  if (imageMap->getColorMap().empty() || check_neighbours)
    imageMap->getColorMap(true);

  int n = 0;
  T *finalAbsorbent, *v;

  for (int i = 0; i < imageMap->getWidth(); i++)
    for (int j = 0; j < imageMap->getHeight(); j++)
    {
      int id = imageMap->getSegment(i,j);
      v = vertices + id;

      assert(v);
      if ( (finalAbsorbent = dynamic_cast<T*>(v->getFinalAbsorbent())) )
      {
        (*imageMap)(i,j) = getId(finalAbsorbent);
        n++;
      }
    }

  for (int i = 0; i < imageMap->getWidth(); i++)
    for (int j = 0; j < imageMap->getHeight(); j++)
      vertices[imageMap->getSegment(i,j)].clearAbsorbent();

  needUpdateMapping = false;

  assert(n > 0); 	// если needUpdateMapping был true, то кто-то точно должен был обновиться
  // LOG_DEBUG("Updated mapping of " << n << " pixels");
}

template<typename T>
bool Segmentator<T>::areConnected(const T *v1, const T *v2) const
{
  bool a = v1->isConnectedTo(v2);
  bool b = v2->isConnectedTo(v1);

  assert(a == b);
  return a;
}

template<typename T>
bool Segmentator<T>::dumpSegment(T *v, Points &points)
{
  assert(v and !isEmpty());

  // LOG_INFO("Dumping segment " << v - vertices);
  // LOG_INFO("R = " << v->channelsSum[0] / v->area << "  G = " << v->channelsSum[1] / v->area << "  B = " << v->channelsSum[2] / v->area << "  Area = " << v->area);

  if (!goodVertex(v))
    throw std::runtime_error("goodVertex() failed");

  SegmentID vID = getId(v);

  for (int j = 0; j < imageMap->getHeight(); j++)
    for (int i = 0; i < imageMap->getWidth(); i++)
      if (imageMap->getSegment(i,j) == vID)
        points.push_back(Point(i,j));
  return true;
}

template<typename T>
Segmentator<T>::Segmentator(const Image &image,
                            EdgeValue (*ef)(const T *v),
                            EdgeValue (*df)(const T *v1, const T *v2),
                            bool _normalize)
  : error_function(ef)
  , distance_function(df)
  , channelsNum(image.getChannelsNum())
  , normalize(_normalize)
{
  imageMap = new ImageMap(image.getWidth(), image.getHeight());

  createAdjacencyGraph(image);
}

template<typename T>
Segmentator<T>::Segmentator(const Image &image,
                            const ImageMap& _imageMap,
                            EdgeValue (*ef)(const T *v),
                            EdgeValue (*df)(const T *v1, const T *v2),
                            std::set<std::pair<int, int> > const & _blockList,
                            bool _blocking_policy,
                            bool _normalize)
  : error_function(ef)
  , distance_function(df)
  , channelsNum(image.getChannelsNum())
  , blockList(_blockList)
  , blocking_policy(_blocking_policy)
  , normalize(_normalize)
{
  imageMap = new ImageMap(image.getWidth(), image.getHeight());

  if (_imageMap.getWidth() != image.getWidth() || _imageMap.getHeight() != image.getHeight())
    throw std::runtime_error("ImageMap size is inconsistent with image");

  createAdjacencyGraph(image, _imageMap);
}

template<typename T>
void Segmentator<T>::createAdjacencyGraph(const Image &image)
{
  // LOG_INFO("Creating adjacency graph...");

  const int width = image.getWidth();
  const int height = image.getHeight();

  initialize(width*height, 2*width*height - width - height);

  T *row, *v;
  int i, j;

  for (j = 0, v = vertices; j < height; j++)
    for (i = 0; i < width; i++, v++)
    {
      const uint8_t *pix = image.getPixel(i, j);
      SegmentID id = getId(v);
      (*imageMap)(i,j) = id;
      v->update(pix);
    }

  for (j = 0, row = vertices; j < height; j++, row += width)
    for (i = 0, v = row; i < width-1; i++, v++)
      connect(v, v+1);

  T *column;
  for (i = 0, column = vertices; i < width; i++, column++)
    for (j = 0, v = column; j < height-1; j++, v += width)
      connect(v, v+width);

  // LOG_INFO("Adjacency graph created (" << width*height << " vertices, " << 2*width*height - width - height << " edges)");
}

template<typename T>
void Segmentator<T>::createAdjacencyGraph(const Image &image,
                                          const ImageMap & _imageMap)
{
  auto const stats = _imageMap.getSegmentStats();
  std::map<SegmentID, SegmentID> id_to_idx;
  SegmentID idx = 0;
  for (auto const & stat : stats)
  {
    id_to_idx[stat.first] = idx;
    idx++;
  }

  int edgesNum = sumNeighboursNum(stats);
  vertexNum = stats.size();

  initialize(vertexNum, edgesNum);
  for (auto const & stat : stats)
  {
    idx = id_to_idx[stat.first];
    auto rect = stat.second.rect;

    T *v = vertices + idx;

    for (int i = rect.y; i < rect.y + rect.height; ++i)
    {
      for (int j = rect.x; j < rect.x + rect.width; ++j)
      {
        if (_imageMap.getSegment(j, i) != stat.first)
          continue;
        v->update(image.getPixel(j, i));
        (*imageMap)(j,i) = idx;
      }
    }

    if (std::find(blockList.begin(), blockList.end(), stat.second.leftTopPoint) != blockList.end())
        v->isBlocked = true;

    errorAccumulator += calcError(v);
  }

  for (auto const & stat : stats)
  {
    idx = id_to_idx[stat.first];
    T *v = vertices + idx;

    if (v->isBlocked)
      continue;

    for (auto const & n : stat.second.neighbours)
    {
      if (idx >= id_to_idx[n])
        continue; // call connect() only once for each pair
      if ((vertices + id_to_idx[n])->isBlocked)
        continue;
      connect(v, vertices + id_to_idx[n], true);
    }
  }

  for (auto const & stat : stats)
  {
    idx = id_to_idx[stat.first];
    T *v = vertices + idx;

    if (v->isBlocked)
      continue;

    for (auto const & n : stat.second.neighbours)
    {
      if (idx >=id_to_idx[n])
        continue; // call connect() only once for each pair
      if ((vertices + id_to_idx[n])->isBlocked)
        continue;
      for (Joint it = v->begin(); it != v->end(); it++)
        if (it->vertex == (vertices + id_to_idx[n]))
        {
          edgeHeap->update(it->edge, distance_function(v, vertices + id_to_idx[n]));
          break;
        }
    }
  }
//  LOG_INFO("Adjacency graph created (" << imageMap.getWidth() * imageMap.getHeight()
//                                       << " vertices, " << edgesNum << " edges)");
}

template<typename T>
void Segmentator<T>::connect(T *a, T *b, bool dummy)
{
  assert(goodVertex(a) and goodVertex(b));
  assert(a != b);

  Joint jointA = a->orderedInsert(b);
  Joint jointB = b->orderedInsert(a);

  jointA->joint = jointB;
  jointB->joint = jointA;

  double dist = 0;
  if (!dummy)
    dist = distance_function(a, b);
  edgeHeap->push(Edge(a, b, dist, jointA, jointB));
}

template<typename T>
void Segmentator<T>::mergeNext(bool do_update_mapping)
{
  assert(!isEmpty());
  if (edgeHeap->getSize() == 0)
    // LOG_WARNING("Nothing to merge");
    return;

  Edge *topEdge = edgeHeap->top();

  if (mergeLogStream)
  {
    int a = getId(dynamic_cast<T*>(topEdge->a));
    int b = getId(dynamic_cast<T*>(topEdge->b));
    mergeLogStream->write(reinterpret_cast<const char*>(&a), sizeof(a));
    mergeLogStream->write(reinterpret_cast<const char*>(&b), sizeof(b));
  }

  T* v1 = dynamic_cast<T*>(topEdge->a);
  T* v2 = dynamic_cast<T*>(topEdge->b);
  if (v1->area > v2->area)
    merge(v1, v2);
  else
    merge(v2, v1);

  if (do_update_mapping)
    updateMapping();
}

template<typename T>
int Segmentator<T>::mergeToLimitCycle(EdgeValue distanceLimit, EdgeValue errorLimit, int segmentsLimit,
                                      i8r::PLogger dbg, int debug_iter, int maxSegments)
{
  assert(!isEmpty());

  Edge *topEdge;

  double EPS = 1e-5;

  bool noDistanceLimit = distanceLimit < EPS;
  bool noErrorLimit = errorLimit < EPS;
  bool noSegmentsLimit = segmentsLimit < 0;
  maxSegments = maxSegments < 0 ? imageMap->getWidth() * imageMap->getHeight() : maxSegments;

  int i = 0;
  int N = normalize ? imageMap->getWidth() * imageMap->getHeight() : 1;

  while ((topEdge = edgeHeap->top()) and
         (noDistanceLimit or topEdge->value < distanceLimit) and
         (noErrorLimit or (errorAccumulator + std::pow(topEdge->value, 2)) / N
                          < std::pow(errorLimit, 2)) and
         (noSegmentsLimit or numberOfSegments() > segmentsLimit))
  {
    if (topEdge->a == breakpoint or topEdge->b == breakpoint) {
      updateMapping();
      return MERGE_BREAK;
    }

    double dist = topEdge->value;
    errorAccumulator += std::pow(dist,2);
    mergeNext(false);

    if (!dbg)
      continue;

    if (dbg->enabled() and i % debug_iter == 0 and vertexNum <= maxSegments)
    {
      updateMapping();
      DECLARE_GUARDED_MINIMG(vis);
      visualize(&vis, *imageMap);
      dbg->save(std::to_string(i) + "_" +
                dist_to_string(std::sqrt(errorAccumulator / N)) + "_" +
                dist_to_string(dist) + "_" +
                std::to_string(vertexNum), "segm", &vis, "");
    }
    i++;
  }

  updateMapping();

  return MERGE_OK;
}

template<typename T>
int Segmentator<T>::mergeToLimit(EdgeValue distanceLimit, EdgeValue errorLimit, int segmentsLimit,
                                 i8r::PLogger dbg, int debug_iter, int maxSegments)
{
  assert(!isEmpty());
  int result = MERGE_OK;

  if (blocking_policy == BLOCK_SEGMENTS)
  {
    result = mergeToLimitCycle(distanceLimit, errorLimit, segmentsLimit, dbg, debug_iter, maxSegments);
    if (result != MERGE_OK)
      return result;
  }

  if (!blockList.empty() || blocking_policy == BLOCK_EDGES)
  {
    updateMapping(true);
    for (auto const & stat : imageMap->getSegmentStats())
    {
      int id = stat.first;
      T *v = &vertices[id];
      for (auto const & n : stat.second.neighbours)
        if (vertices[n].isBlocked != v->isBlocked)
          connect(v, &vertices[n]);
    }

    result = mergeToLimitCycle(distanceLimit, errorLimit, segmentsLimit, dbg, debug_iter, maxSegments);
    if (result != MERGE_OK)
      return result;
  }

  return MERGE_OK;
}

template<typename T>
T *Segmentator<T>::mergeBackground(int areaLimit)
{
  if (isEmpty())
    throw std::runtime_error("Empty segmentator");

  T *seed = 0;

  // searching for the first piece of background (seed)
  for (int i = 0; i < sizeOfVertices; i++)
    if (vertices[i].exists()  and  vertices[i].area > areaLimit)
    {
      seed = &vertices[i];
      break;
    }

  if (!seed)
    return 0;

  int n = 1;
  for (int i = seed - vertices + 1; i < sizeOfVertices; i++)
    if (vertices[i].exists()  and  vertices[i].area > areaLimit)
    {
      merge(seed, &vertices[i]);
      n++;
    }

  // LOG_INFO(n << " pieces have been merged to produce background");
  updateMapping();
  return seed;
}

template<typename T>
void Segmentator<T>::removeHoles(T *background)
{
  // LOG_INFO("Removing holes...");
  std::vector<T *> lst;
  T *vv = vertices;
  for (int i = 0; i < sizeOfVertices; i++, vv++)
    if (vv->exists()  and  vv!= background  and  !vv->isConnectedTo(background))    // граничит с единственным сегментом, который не является фоном
      lst.push_back(vv);

  // TODO: uncomment and make this work
  // for (std::vector<T *>::iterator it = lst.begin(); it != lst.end(); it++)
  // 	merge(background, *it);

  updateMapping();
}

template<typename T>
void Segmentator<T>::merge(T *absorbent, T *v)
{
  assert(goodVertex(absorbent) and goodVertex(v));
  assert(absorbent != v);

  stepNumber++;

  if (absorbent->size() > max_neighbours)
    max_neighbours = absorbent->size();
  if (v->size() > max_neighbours)
    max_neighbours = v->size();

  bool connected = false;	// были ли соединены v и absorbent? (нужно для выявления ошибок)
  absorbent->absorb(v);

  EdgeValue dist = -1;

  for (Joint it = absorbent->begin(); it != absorbent->end(); it++)    // помечаем соседей absorbent для последующего выявления дублей
    mergeAuxArray[getId(dynamic_cast<T*>(it->vertex))] = stepNumber;


  for (Joint it = v->begin(); it != v->end();)
  {
    Joint next = std::next(it);
    // элементы в v являются либо absorbent (1), либо общими с absorbent (2), либо новыми для absorbent (3)

    // (1)
    if (it->vertex == absorbent)
    {
      connected = true;
      dist = it->edge->value;
      absorbent->erase(it->joint);
      edgeHeap->remove(it->edge);
    }
    else
    {
      // (2)
      if (mergeAuxArray[getId(dynamic_cast<T*>(it->vertex))] == stepNumber)
      {
        it->vertex->erase(it->joint);	// удаление v из общей вершины
        edgeHeap->remove(it->edge);
      }
      else
      {
        // (3)
        Vertex *common = it->vertex;
        Joint commonIt = it->joint;	// позиция v в третьей вершине
        Edge *e = commonIt->edge;	// ребро, соединяющее v и третью вершину

        absorbent->splice(absorbent->begin(), *v, it); // перемещаем Link it из v в absorbent
        commonIt->vertex = absorbent;
        commonIt->joint = absorbent->begin();

        // мы будем использовать уже существующее ребро e, нужно только поменять его концы
        e->a = absorbent;
        e->b = common;
        e->jointA = commonIt->joint;
        e->jointB = commonIt;
      }
    }
    it = next;
  }

  // пересчет весов ребер absorbent
  for (Joint it = absorbent->begin(); it != absorbent->end(); it++)
    edgeHeap->update(it->edge, distance_function(absorbent, dynamic_cast<T*>(it->vertex)));

  	// LOG_INFO(absorbent-vertices << " has absorbed " << v-vertices << " (distance " << dist << ")");

  assert(connected);	// объединяемые вершины должны были быть соединены

  vertexNum--;
  needUpdateMapping = true;
}

template<typename T>
Segmentator<T>::~Segmentator()
{
  // TODO: FIX
  // if (vertices)
  // {
  //   if (vertices[i].empty())
  //   LOG_WARNING("existing empty vertex");
  // 	 delete[] vertices;
  // }
  if (edgeHeap)
    delete edgeHeap;
  if (mergeAuxArray)
    delete[] mergeAuxArray;
  if (imageMap)
    delete imageMap;
  // LOG_INFO("Memory deallocated ");
  // LOG_INFO("Maximum neighbours detected: " << max_neighbours);
}

template<typename T>
void Segmentator<T>::initialize(int vNum, int eNum)
{
  if (vNum <= 0 or eNum <= 0)
    throw std::invalid_argument("invalid Segmentator parameters");

  if (not isEmpty())
    throw std::runtime_error("Segmentator is not empty");

  if (!(vertices = new T[vNum]))
    throw std::runtime_error("cannot allocate vertices");

  vertexNum = sizeOfVertices = vNum;

  for (int i = 0; i < sizeOfVertices; i++)
    vertices[i].Initialize(channelsNum);

  if (!(edgeHeap = new EdgeHeap(eNum)))
    throw std::runtime_error("cannot allocate edges heap");

  if (!(mergeAuxArray = new int [sizeOfVertices]))
    throw std::runtime_error("failed to allocate mergeAuxArray");

  memset(mergeAuxArray, 0, sizeOfVertices * sizeof(int));
}

template<typename T>
void Segmentator<T>::saveLog(std::string const & filename)
{
  const ImageMap &imageMap = getImageMap();

  Json::Value root;
  root["number_of_segments"] = numberOfSegments();
  root["segments"] = Json::objectValue;

  auto colorMap = imageMap.getColorMap();
  auto stats = imageMap.getSegmentStats();

  for (auto const & stat : stats)
  {
    SegmentID id = stat.first;
    Json::Value segment;
    segment["id"] = id;
    segment["color"] = Json::arrayValue;
    for (int i = 0; i < 3; ++i)
      segment["color"].append(colorMap[id][i]);

    segment["leftTopPoint"] = Json::arrayValue;
    segment["leftTopPoint"].append(stat.second.leftTopPoint.first);
    segment["leftTopPoint"].append(stat.second.leftTopPoint.second);

    T * vertex = &vertices[id];

    segment["neighbours"] = Json::arrayValue;
    for (auto const& n : stat.second.neighbours)
      segment["neighbours"].append(n);

    segment["scores"] = Json::arrayValue;
    for (auto const& n : stat.second.neighbours)
    {
      T* v_n = vertices + n;
      segment["scores"].append(distance_function(vertex, v_n));
    }
    segment["statistics"] = vertex->jsonLog();
    root["segments"][std::to_string(id)] = segment;
  }

  vi::json_to_file(filename, root);
}

}}	// ns vi::remseg
