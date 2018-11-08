/*

Copyright (c) 2016, Smart Engines Limited. All rights reserved.

All rights reserved.

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
#ifndef MINGEO_MINGEO_H_INCLUDED
#define MINGEO_MINGEO_H_INCLUDED

#include <minbase/mingeo.h>

#ifdef __cplusplus

#include <algorithm>
#include <vector>

//// minpoint.h
namespace se {

template < class CT > // coords type
struct pnt2
{
  CT x,y; ///< x,y - coordinates of the point
  pnt2( CT _x=0, CT _y=0 ):x(_x),y(_y){}
  template< class CT2 >
  pnt2( const pnt2< CT2 >& p ):
    x(static_cast<CT>(p.x)),y(static_cast<CT>(p.y)){}
  CT& operator [] ( int i ) { return (&x)[i]; }
  CT  operator [] ( int i ) const { return (&x)[i]; }
  int size() { return 2; }
  template< class CT2 >  pnt2<CT>& operator *= ( const pnt2<CT2>& p )
  { x = static_cast<CT>(x*p.x); y=static_cast<CT>(y*p.y); return *this; }
  template< class CT2 >  pnt2<CT>& operator /= ( const pnt2<CT2>& p )
  { x /= p.x; y/=p.y; return *this; }
};

template < class CT > // coords type
struct pnt3
{
  CT x,y,z; ///< x,y,z - coordinates of the point
  pnt3( CT _x=0, CT _y=0, CT _z=0 ):x(_x),y(_y),z(_z){}
  template< class CT2 >
  pnt3( const pnt3< CT2 >& p ):
    x(static_cast<CT>(p.x)),y(static_cast<CT>(p.y)),z(static_cast<CT>(p.z)){}
  template< class CT2 >
  pnt3( const pnt2< CT2 >& p, CT2 _z ):
    x(static_cast<CT>(p.x)),y(static_cast<CT>(p.y)),z(static_cast<CT>(_z)){}
  CT& operator [] ( int i ) { return (&x)[i]; }
  CT  operator [] ( int i ) const { return (&x)[i]; }
  int size() { return 3; }
  template< class CT2 > pnt3<CT>& operator *= ( const pnt3<CT2>& p )
  { x *= p.x; y*=p.y; z*=p.z; return *this; }
  template< class CT2 > pnt3<CT>& operator /= ( const pnt3<CT2>& p )
  { x /= p.x; y/=p.y; z/=p.z; return *this; }
};

template < class CT >
inline pnt2<CT> operator * (const pnt2<CT>& p, double alpha )
{ return pnt2<CT>(static_cast<CT>(p.x*alpha), static_cast<CT>(p.y*alpha)); }

template < class CT >
inline pnt2<CT> operator / ( const pnt2<CT>& p, double alpha )
{ return pnt2<CT>(static_cast<CT>(p.x/alpha), static_cast<CT>(p.y/alpha)); }

template < class CT >
inline pnt2<CT> operator * ( double alpha, const pnt2<CT>& p )
{ return pnt2<CT>(static_cast<CT>(p.x*alpha), static_cast<CT>(p.y*alpha)); }

template < class CT >
inline pnt2<CT>& operator *= ( pnt2<CT>& p, double alpha )
{ p.x=static_cast<CT>(p.x*alpha); p.y=static_cast<CT>(p.y*alpha);  return p; }

template < class CT >
inline pnt2<CT>& operator /= ( pnt2<CT>& p, double alpha )
{ p.x=static_cast<CT>(p.x/alpha); p.y=static_cast<CT>(p.y/alpha);  return p; }

template < class CT >
inline pnt2<CT> operator - ( const pnt2<CT>& p )
{ return pnt2<CT>(-p.x, -p.y); }

template < class CT >
inline pnt2<CT> operator + ( const pnt2<CT>& p, const pnt2<CT>& q )
{ return pnt2<CT>(p.x+q.x, p.y+q.y); }

template < class CT >
inline pnt2<CT>& operator += ( pnt2<CT>& p, const pnt2<CT>& add_q )
{ p.x += add_q.x; p.y += add_q.y; return p;}

template < class CT >
inline pnt2<CT> operator - ( const pnt2<CT>& p, const pnt2<CT>& q )
{ return pnt2<CT>(p.x-q.x, p.y-q.y); }

template < class CT >
inline pnt2<CT>& operator -= ( pnt2<CT>& p, const pnt2<CT>& sub_q )
{ p.x -= sub_q.x; p.y -= sub_q.y; return p; }

template < class CT >
inline double dot( const pnt2<CT>& p, const pnt2<CT>& q )
{ double res = p.x*double(q.x)+p.y*double(q.y); return res; }

template < class CT > // z-component of cross product of 2d points
inline double cross( const pnt2<CT>& p, const pnt2<CT>& q )
{ double res = p.x*double(q.y) - p.y*double(q.x); return res; }

template < class CT >
inline double sqlen( const pnt2<CT>& p ) // squared euclidean length
{ double res = p.x*double(p.x) + p.y*double(p.y); return res; }

template < class CT >
inline double length( const pnt2<CT>& p ) // euclidean length
{ double res = sqrt( p.x*double(p.x) + p.y*double(p.y) ); return res; }

template < class CT >
inline bool is_clockwise( const pnt2<CT>& a,  const pnt2<CT>& b,  const pnt2<CT>& c )
// returns true if triangle a-b-c is not degenerated (has non zero area)
// and points a-b-c are enumerated in clockwise order in lefthand (image) coords
{ return cross( b-a, c-a ) > 0; } // example 0,0->1,0->0,1


template < class CT >
inline bool is_counterclockwise( const pnt2<CT>& a,  const pnt2<CT>& b,  const pnt2<CT>& c )
// returns true if triangle a-b-c is not degenerated (has non zero area)
// and points a-b-c are enumerated in counterclockwise order in lefthand (image) coords
{ return cross( b-a, c-a ) < 0; } // example 0,0->0,1->1,0

template < class CT >
inline double cosangle( const pnt2<CT>& a,  const pnt2<CT>& b )
// returns [-1...1] ranged cosine of angle between a and b (or between b and a, symmetric)
{
  double res = dot( a, b ) / (length(a)*length(b));
  if (res > 1.)
    res=1.;
  if (res < -1.)
    res=-1.;
  return res;
}

///////////////////////////////////////////////////////
// pnt3

template < class CT >
inline pnt3<CT> operator * ( const pnt3<CT>& p, double alpha )
{ return pnt3<CT>( static_cast<CT>(p.x*alpha), static_cast<CT>(p.y*alpha), static_cast<CT>(p.z*alpha) );}

template < class CT >
inline pnt3<CT> operator * ( double alpha, const pnt3<CT>& p )
{ return pnt3<CT>( p.x*alpha, p.y*alpha, p.z*alpha ); }

template < class CT >
inline pnt3<CT> operator / ( const pnt3<CT>& p, double alpha )
{ return pnt3<CT>( p.x/alpha, p.y/alpha, p.z/alpha ); }

template < class CT >
inline pnt3<CT>& operator *= ( pnt3<CT>& p, double alpha )
{ p.x *= alpha; p.y *= alpha; p.z *= alpha;  return p; }

template < class CT >
inline pnt3<CT>& operator /= ( pnt3<CT>& p, double alpha )
{ p.x /= alpha; p.y /= alpha; p.z /= alpha;  return p; }

template < class CT >
inline pnt3<CT> operator - ( const pnt3<CT>& p )
{ return pnt3<CT>( -p.x, -p.y, -p.z ); }

template < class CT >
inline pnt3<CT> operator + ( const pnt3<CT>& p, const pnt3<CT>& q )
{ return pnt3<CT>( p.x+q.x, p.y+q.y, p.z+q.z ); }

template < class CT >
inline pnt3<CT>& operator += ( pnt3<CT>& p, const pnt3<CT>& add_q )
{ p.x += add_q.x; p.y += add_q.y; p.z += add_q.z;  return p;}

template < class CT >
inline pnt3<CT> operator - ( const pnt3<CT>& p, const pnt3<CT>& q )
{ return pnt3<CT>(p.x-q.x, p.y-q.y, p.z-q.z); }

template < class CT >
inline pnt3<CT>& operator -= ( pnt3<CT>& p, const pnt3<CT>& sub_q )
{ p.x -= sub_q.x; p.y -= sub_q.y; p.z -= sub_q.z;  return p; }

template < class CT >
inline double dot( const pnt3<CT>& p, const pnt3<CT>& q )
{ double res = p.x*double(q.x)+p.y*double(q.y)+p.z*double(q.z); return res; }

template < class CT >
inline pnt3<CT> cross( const pnt3<CT>& p, const pnt3<CT>& q )
{
  pnt3<CT> res(
    p.y*q.z - p.z*q.y,
    p.z*q.x - p.x*q.z,
    p.x*q.y - p.y*q.x
  );
  return res;
}

template < class CT >
inline double sqlen( const pnt3<CT>& p ) // squared euclidean length
{ double res = p.x*double(p.x) + p.y*double(p.y) + p.z*double(p.z); return res; }

template < class CT >
inline double length( const pnt3<CT>& p ) // euclidean length
{ double res = sqrt( p.x*double(p.x) + p.y*double(p.y) + p.z*double(p.z) ); return res; }

template < class CT >
inline double cosangle( const pnt3<CT>& a,  const pnt3<CT>& b )
// returns [-1...1] ranged cosine of angle between a and b (or between b and a, symmetric)
{
  double res = dot( a, b ) / (length(a)*length(b));
  if (res > 1.)
    res=1.;
  else if (res < -1.)
    res=-1.;
  return res;
}

} // namespace se

typedef se::pnt2< int32_t > MinPoint2i;
typedef se::pnt3< int32_t > MinPoint3i;
typedef se::pnt2< int16_t > MinPoint2s;
typedef se::pnt3< int16_t > MinPoint3s;
typedef se::pnt2< real32_t > MinPoint2f;
typedef se::pnt3< real32_t > MinPoint3f;
typedef se::pnt2< real64_t > MinPoint2d;
typedef se::pnt3< real64_t > MinPoint3d;

typedef MinPoint2i MinPoint;

#endif // __cplusplus

/**
 * @brief   Inline constructors for @c MinPoint data type.
 * @param   x The x-coordinate of the point.
 * @param   y The y-coordinate of the point.
 * @returns A new @c MinPoint object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinPoint object using x-coordinate and
 * y-coordinate values.
 */
MUSTINLINE MinPoint minPoint(int32_t x, int32_t y)
{
#ifndef __cplusplus
  MinPoint point = {x, y};  return point;
#else
  return MinPoint(x,y);
#endif
}

/**
 * @brief   Inline constructors for @c MinPoint2f data type.
 * @param   x The x-coordinate of the point.
 * @param   y The y-coordinate of the point.
 * @returns A new @c MinPoint2f object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinPoint2f object using x-coordinate and
 * y-coordinate values.
 */
MUSTINLINE MinPoint2f minPoint2f(real32_t x, real32_t y)
{
#ifndef __cplusplus
  MinPoint2f point = {x, y};  return point;
#else
  return MinPoint2f(x,y);
#endif
}

/**
 * @brief   Inline constructors for @c MinPoint3f data type.
 * @param   x The x-coordinate of the point.
 * @param   y The y-coordinate of the point.
 * @param   z The y-coordinate of the point.
 * @returns A new @c MinPoint3f object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinPoint3f object using x-coordinate and
 * y-coordinate values.
 */
MUSTINLINE MinPoint3f minPoint3f(real32_t x, real32_t y, real32_t z)
{
#ifndef __cplusplus
  MinPoint3f point = {x, y, z};  return point;
#else
  return MinPoint3f(x, y, z);
#endif
}


//// minsegm.h

//---------------------------------------------------------- new 23 feb 2012
#ifdef __cplusplus
/**
 * @brief   Line segment represented as two ends points.
 * @details The struct @c MinLineSegment represents a line segment as a tuple of
 * two MinPoint's.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  MinPoint u;     ///< First point.
  MinPoint v;     ///< Second point.
} MinLineSegment;

/**
 * @brief   Inline constructor for @c MinLineSegment data type.
 * @param   u      First point.
 * @param   v      Second point.
 * @returns A new @c MinLineSegment object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinLineSegment object using two point.
 */
MUSTINLINE MinLineSegment minLineSegment(MinPoint u, MinPoint v)
{
  MinLineSegment segment = {u, v};
  return segment;
}

namespace se {

template < class CT > // coords type
struct segm2
{
  pnt2< CT > a,b; ///< a,b - end points of 2d line segment
  segm2( CT _x1=0, CT _y1=0,  CT _x2=0, CT _y2=0 ):a(_x1, _y1),b(_x2, _y2){}

  template< class CT2 >
  segm2( const segm2< CT2 >& s ):
    a(s.a), b(s.b){}  ///a(static_cast<CT>(s.a)),b(static_cast<CT>(s.b)){}

  template< class CT2 >
  segm2( const pnt2< CT2 >& _a,  const pnt2< CT2 >& _b ):
    a(_a),b(_b){}

  pnt2< CT >& operator [] ( int i ) { return (&a)[i]; }
  int size() { return 2; }
  template< class CT2 >  segm2<CT>& operator *= ( const pnt2<CT2>& p )
  { a *= p; b*=p; return *this; }
  template< class CT2 >  segm2<CT>& operator /= ( const pnt2<CT2>& p )
  { a /= p; b/=p; return *this; }
};

template < class CT > // coords type
struct segm3
{
  pnt3< CT > a,b;  ///< a,b - end points of 3d line segment
  segm3(
    CT _x1=0, CT _y1=0, CT _z1=0,
    CT _x2=0, CT _y2=0, CT _z2=0  ):
    a(_x1, _y1, _z1),
    b(_x2, _y2, _z2)
    {}

  template< class CT2 >
  segm3( const segm3< CT2 >& s ):
    a(s.a), b(s.b){}  ///a(static_cast<CT>(s.a)),b(static_cast<CT>(s.b)){}

  template< class CT2 >
  segm3( const pnt3< CT2 >& _a,  const pnt3< CT2 >& _b ):
    a(_a),b(_b){}

  pnt3< CT >& operator [] ( int i ) { return (&a)[i]; }
  int size() { return 2; }
  template< class CT2 >  segm3<CT>& operator *= ( const pnt3<CT2>& p )
  { a *= p; b*=p; return *this; }
  template< class CT2 >  segm3<CT>& operator /= ( const pnt3<CT2>& p )
  { a /= p; b/=p; return *this; }
};

template < class CT >
inline segm2<CT>& operator *= ( segm2<CT>& p, double alpha )
{  p.a *= alpha; p.b *= alpha; return p; }

template < class CT >
inline segm3<CT>& operator *= ( segm3<CT>& p, double alpha )
{  p.a *= alpha; p.b *= alpha; return p; }

template < class CT >
inline segm2<CT>& operator /= ( segm2<CT>& p, double alpha )
{ p.a /= alpha; p.b /= alpha; return p; }

template < class CT >
inline segm3<CT>& operator /= ( segm3<CT>& p, double alpha )
{ p.a /= alpha; p.b /= alpha; return p; }

template < class CT >
inline double length( const segm2<CT>& s ) // euclidean length
{ double res = length( s.b-s.a ); return res; }

template < class CT >
inline double length( const segm3<CT>& s ) // euclidean length
{ double res = length( s.b-s.a ); return res; }

template < class CT >
inline double sqlen( const segm2<CT>& s ) // squared euclidean length
{ double res = sqlen( s.b-s.a );  return res; }

template < class CT >
inline double sqlen( const segm3<CT>& s ) // squared euclidean length
{ double res = sqlen( s.b-s.a );  return res; }

} // namespace se::

typedef se::segm2< int16_t > MinSegm2s;
typedef se::segm2< int32_t > MinSegm2i;
typedef se::segm2< real32_t > MinSegm2f;
typedef se::segm2< real64_t > MinSegm2d;

typedef MinSegm2i MinSegm;

typedef se::segm3< int16_t > MinSegm3s;
typedef se::segm3< int32_t > MinSegm3i;
typedef se::segm3< real32_t > MinSegm3f;
typedef se::segm3< real64_t > MinSegm3d;

#endif // __cplusplus
//---------------------------------------------------------- end of new  23 feb 2012


//// minsize.h

#ifdef __cplusplus

namespace se {
  template < class CT > // coords type
  struct size2
  {
    CT width, height;   // width and height values of the size
    size2( CT _width = 0, CT _height = 0 ) : width(_width), height(_height) {}
    template< class CT2 >
    size2( const size2< CT2 >& size ):
      width(static_cast<CT>(size.width)), height(static_cast<CT>(size.height)){}
  };

  template < class CT > // coords type
  struct size3
  {
    CT width, height, depth;   // width, height, and depth values of the size
    size3( CT _width = 0, CT _height = 0, CT _depth = 0 ) :
      width(_width), height(_height), depth(_depth) {}
    template< class CT2 >
    size3( const size3< CT2 >& size ):
      width(static_cast<CT>(size.width)),
      height(static_cast<CT>(size.height)),
      depth(static_cast<CT>(size.depth)){}
    template< class CT2 >
    size3( const size2< CT2 >& size, CT2 _depth ):
      width(static_cast<CT>(size.width)),
      height(static_cast<CT>(size.height)),
      depth(static_cast<CT>(_depth)){}
  };

  template < class CT >
  inline size2<CT> operator + ( const size2<CT>& p, const size2<CT>& q )
  { return size2<CT>(p.width+q.width, p.height+q.height); }

}  // namespace se

typedef se::size2< int32_t > MinSize2i;
typedef se::size3< int32_t > MinSize3i;
typedef se::size2< int16_t > MinSize2s;
typedef se::size3< int16_t > MinSize3s;
typedef se::size2< real32_t > MinSize2f;
typedef se::size3< real32_t > MinSize3f;
typedef se::size2< real64_t > MinSize2d;
typedef se::size3< real64_t > MinSize3d;

typedef MinSize2i MinSize;

#endif  // __cplusplus

/**
 * @brief   Inline constructors for @c MinSize data type.
 * @param   width  The width of the size object.
 * @param   height The height of the size object.
 * @returns A new @c MinSize object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinSize object using width and
 * height values.
 */
MUSTINLINE MinSize minSize(int32_t width, int32_t height)
{
#ifndef __cplusplus
  MinSize size = {width, height}; return size;
#else
  return MinSize(width, height);
#endif
}


//// minrect.h

#ifdef __cplusplus
namespace se {

template<class CT> // coords type
struct rect2 // 2d parallelepiped coaxial to XY axes
{
  CT x,y;          ///< x,y - coordinates of the left-top corner
  CT width,height; ///< width,height - x size and y size
  rect2(CT _x = 0, CT _y = 0, CT _width = 0, CT _height = 0)
    : x(_x), y(_y), width(_width), height(_height)
  {}
  template<class CT2>
  rect2(const rect2<CT2> &rc)
    : x(static_cast<CT>(rc.x)),
      y(static_cast<CT>(rc.y)),
      width(static_cast<CT>(rc.width)),
      height(static_cast<CT>(rc.height))
  {}
};

template<class CT> // coords type
struct rect3 // 3d parallelepiped coaxial to XYZ axes
{
  CT x,y,z;              ///< x,y,z - coordinates of the corner (usually with min values)
  CT width,height,depth; ///< width,height,depth - x size, y size, z size respectively
  rect3(CT _x = 0, CT _y = 0, CT _z = 0,
        CT width = 0, CT height = 0, CT depth = 0)
    : x(_x), y(_y), z(_z), width(width), height(height), depth(depth)
  {}

  template<class CT2>
  rect3(const rect3<CT2> &rc)
    : x(static_cast<CT>(rc.x)),
      y(static_cast<CT>(rc.y)),
      z(static_cast<CT>(rc.z)),
      width(static_cast<CT>(rc.width)),
      height(static_cast<CT>(rc.height)),
      depth(static_cast<CT>(rc.depth))
  {}
};

template<class CT>
inline rect2<CT> operator *(const rect2<CT> &p, double alpha)
{
  return rect2<CT>(static_cast<CT>(p.x * alpha), static_cast<CT>(p.y * alpha),
      static_cast<CT>(p.width * alpha), static_cast<CT>(p.height * alpha));
}

template<class CT>
inline rect2<CT> operator /(const rect2<CT> &p, double alpha)
{
  return rect2<CT>(static_cast<CT>(p.x / alpha), static_cast<CT>(p.y / alpha),
      static_cast<CT>(p.width / alpha), static_cast<CT>(p.height / alpha));
}

template<class CT>
inline rect3<CT> operator *(const rect3<CT> &p, double alpha)
{
  return rect3<CT>(static_cast<CT>(p.x * alpha), static_cast<CT>(p.y * alpha),
      static_cast<CT>(p.z * alpha), static_cast<CT>(p.width * alpha),
      static_cast<CT>(p.height * alpha), static_cast<CT>(p.depth * alpha));
}

template<class CT>
inline rect3<CT> operator /(const rect3<CT> &p, double alpha)
{
  return rect3<CT>(static_cast<CT>(p.x / alpha), static_cast<CT>(p.y / alpha),
      static_cast<CT>(p.z / alpha), static_cast<CT>(p.width / alpha),
      static_cast<CT>(p.height / alpha), static_cast<CT>(p.depth / alpha));
}

template<class CT>
inline rect2<CT> &inflate(rect2<CT> &r, double delta) // inflate rectangle (or deflate with negative delta)
{
  r.x -= static_cast<CT>(delta);
  r.y -= static_cast<CT>(delta);
  r.width += static_cast<CT>(2 * delta);
  r.height += static_cast<CT>(2 * delta);
  return r;
}

template<class CT>
inline rect3<CT> &inflate(rect3<CT> &r, double delta) // inflate rectangle (or deflate with negative delta)
{
  r.x -= static_cast<CT>(delta);
  r.y -= static_cast<CT>(delta);
  r.z -= (delta);
  r.width += static_cast<CT>(2 * delta);
  r.height += static_cast<CT>(2 * delta);
  r.depth += static_cast<CT>(2 * delta);
  return r;
}

template<class CT>
inline CT area(const rect2<CT> &r) {
  if (r.width <= 0 || r.height <= 0)
     return 0;
  return r.width * r.height;
}

//========== plus minus point
//2d
template<class CT>
inline rect2<CT> operator +(const rect2<CT> &r, const pnt2<CT> &p)
{
  return rect2<CT>(r.x + p.x, r.y + p.y, r.width, r.height);
}

template<class CT>
inline rect2<CT> &operator +=(const rect2<CT> &r, const pnt2<CT> &p)
{
  r.x += p.x;
  r.y += p.y;
  return r;
}

template<class CT>
inline rect2<CT> operator -(const rect2<CT> &r, const pnt2<CT> &p)
{
  return rect2<CT>(r.x - p.x, r.y - p.y, r.width, r.height);
}

template<class CT>
inline rect2<CT> &operator -=(const rect2<CT> &r, const pnt2<CT> &p)
{
  r.x -= p.x;
  r.y -= p.y;
  return r;
}
// 3d
template<class CT>
inline rect3<CT> operator +(const rect3<CT> &r, const pnt3<CT> &p)
{
  return rect3<CT>(r.x + p.x, r.y + p.y, r.z + p.z, r.width, r.height, r.depth);
}

template<class CT>
inline rect3<CT> &operator +=(const rect3<CT> &r, const pnt3<CT> &p)
{
  r.x += p.x;
  r.y += p.y;
  r.z += p.z;
  return r;
}

template<class CT>
inline rect3<CT> operator -(const rect3<CT> &r, const pnt3<CT> &p)
{
  return rect3<CT>(r.x - p.x, r.y - p.y, r.z - p.z, r.width, r.height, r.depth);
}

template<class CT>
inline rect3<CT> &operator -=(const rect3<CT> &r, const pnt3<CT> &p)
{
  r.x -= p.x;
  r.y -= p.y;
  r.z -= p.z;
  return r;
}

} // namespace se

typedef se::rect2<int32_t> MinRect2i;
typedef se::rect3<int32_t> MinRect3i;
typedef se::rect2<int16_t> MinRect2s;
typedef se::rect3<int16_t> MinRect3s;
typedef se::rect2<real32_t> MinRect2f;
typedef se::rect3<real32_t> MinRect3f;
typedef se::rect2<real64_t> MinRect2d;
typedef se::rect3<real64_t> MinRect3d;

typedef MinRect2i MinRect;

#endif // __cplusplus


/**
 * @brief   Inline constructors for @c MinRect data type.
 * @param   x      The x-coordinate of the top-left corner.
 * @param   y      The x-coordinate of the top-left corner.
 * @param   width  The width of the rectangle.
 * @param   height The height of the rectangle.
 * @returns A new @c MinRect object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinRect object using x-coordinate,
 * y-coordinate, width, and height values.
 */
MUSTINLINE MinRect minRect(int32_t x, int32_t y, int32_t width, int32_t height)
{
#ifndef __cplusplus
  MinRect rect = {x, y, width, height};
#else
  MinRect rect(x, y, width, height);
#endif
  return rect;
}


#ifdef __cplusplus

/**
 * @brief   Returns minimal rectangle which contains @first and @second
 * @param   first  The first rectangle
 * @param   second The second rectangle
 * @returns A new @c MinRect object which contains both @first and
            @second rectangles.
 * @ingroup MinUtils_Geometry
 */

inline MinRect MergeRects(MinRect first, MinRect second)
{
  MinRect dst = first;
  int right = std::max<int32_t>(dst.x + dst.width, second.x + second.width);
  int bottom = std::max<int32_t>(dst.y + dst.height, second.y + second.height);
  dst.x = std::min<int32_t>(dst.x, second.x);
  dst.y = std::min<int32_t>(dst.y, second.y);
  dst.width = right - dst.x;
  dst.height = bottom - dst.y;
  return dst;
}

/**
 * @brief   Returns @first and @second rects intersection
 * @param   first  The first rectangle
 * @param   second The second rectangle
 * @returns A new @c MinRect object which is an intersection of @first and @second
 * @ingroup MinUtils_Geometry
 */
inline MinRect IntersectRects(MinRect first, MinRect second)
{
  MinRect dst = first;
  int right = std::min<int32_t>(dst.x + dst.width, second.x + second.width);
  int bottom = std::min<int32_t>(dst.y + dst.height, second.y + second.height);
  dst.x = std::max<int32_t>(dst.x, second.x);
  dst.y = std::max<int32_t>(dst.y, second.y);
  dst.width = right - dst.x;
  dst.height = bottom - dst.y;
  if (dst.width <= 0 || dst.height <= 0)
  {
    dst = MinRect();
  }
  return dst;
}

#endif // __cplusplus


//// minband.h

#ifdef __cplusplus
namespace se {
  template < class CT >
  struct bnd2
  {
    se::pnt2<CT> u; ///< First point
    se::pnt2<CT> v; ///< Second point
    CT           w; ///< Band width
    bnd2(const se::pnt2<CT>& _u, const se::pnt2<CT>& _v, CT _w) :
      u(_u), v(_v), w(_w) {}
    bnd2(const CT _xB, const CT _yB, const CT _xE, const CT _yE, const CT _w) :
      u(pnt2<CT>(_xB, _yB)), v(pnt2<CT>(_xE, _yE)), w(_w) {}
    bnd2() :w(0) {}
    bnd2(const se::rect2<CT>& rect)
      : u(rect.x, rect.y + rect.height / 2),
        v(rect.x + rect.width, rect.y + rect.height / 2),
        w(rect.height) {}
  };
}  // namespace se

typedef se::bnd2< int32_t > MinBand2i;
typedef se::bnd2< int16_t > MinBand2s;
typedef se::bnd2< real32_t > MinBand2f;
typedef se::bnd2< real64_t > MinBand2d;

typedef MinBand2i MinBand;

#endif  // __cplusplus


/**
 * @brief   Inline constructor for @c MinBand data type.
 * @param   u      First point.
 * @param   v      Second point.
 * @param   w      Band width.
 * @returns A new @c MinBand object.
 * @ingroup MinUtils_Geometry
 *
 * The function constructs a new @c MinBand object using two points and width.
 */
MUSTINLINE MinBand minBand(MinPoint u, MinPoint v, int w)
{
#ifndef __cplusplus
  MinBand band = {u, v, w};  return band;
#else
  return MinBand(u, v, w);
#endif
}


//// minquad.h

#ifdef __cplusplus
namespace se {

#ifndef MAXMIN4_DEFINED
  using std::max;
  using std::min;
  template <class CT>
  CT max4( const CT& x1, const CT& x2, const CT& x3, const CT& x4 ) // CT& -- ?
  {
    return std::max( std::max( x1, x2 ), std::max( x3, x4 ) );
  }
  template <class CT>
  CT min4( const CT& x1, const CT& x2, const CT& x3, const CT& x4 ) // CT& -- ?
  {
    return std::min( std::min( x1, x2 ), std::min( x3, x4 ) );
  }
#define MAXMIN4_DEFINED
#endif

template <class CT>
class quad2
{
public:
  pnt2<CT> a,b,c,d; ///< 2d nodes of quadrangle <a,b,c,d>;
  quad2<CT>(){}
  template <class CT2>
  quad2<CT>( const pnt2<CT2>& a, const pnt2<CT2>& b, const pnt2<CT2>& c, const pnt2<CT2>& d ):
    a(a),b(b),c(c),d(d){}
  template <class CT2>
  quad2<CT>( const quad2<CT2>& q ) : a(q.a), b(q.b), c(q.c), d(q.d) {}
  pnt2<CT>& operator [] ( int i ) { return (&a)[i]; } ///< access to <a,b,c,d> as array[0..3];
  const pnt2<CT>& operator [] ( int i ) const { return (&a)[i]; } ///< const access to <a,b,c,d> as array[0..3];
  int size() const { return 4; }
};

template <class CT>
class quad3
{
public:
  pnt3<CT> a,b,c,d; ///< 3d nodes of quadrangle <a,b,c,d>;
  quad3<CT>(){}
  template <class CT2>
  quad3<CT>( const pnt3<CT2>& a, const pnt3<CT2>& b, const pnt3<CT2>& c, const pnt3<CT2>& d ):
  a(a),b(b),c(c),d(d){}
  pnt3<CT>& operator [] ( int i ) { return (&a)[i]; } ///< access to <a,b,c,d> as array[0..3];
  const pnt3<CT>& operator [] ( int i ) const { return (&a)[i]; } ///< access to <a,b,c,d> as array[0..3];
  int size() const { return 4; }
};
//======= mul scalar
template < class CT >
inline quad2<CT> operator * ( const quad2<CT>& p, double alpha )
{ return quad2<CT>(p.a*alpha, p.b*alpha, p.c*alpha, p.d*alpha); }

template < class CT >
inline quad2<CT> operator * ( double alpha, const quad2<CT>& p )
{ return quad2<CT>(p.a*alpha, p.b*alpha, p.c*alpha, p.d*alpha); }

template < class CT >
inline quad3<CT> operator * ( const quad3<CT>& p, double alpha )
{ return quad3<CT>(p.a*alpha, p.b*alpha, p.c*alpha, p.d*alpha); }

template < class CT >
inline quad3<CT> operator * ( double alpha, const quad2<CT>& p )
{ return quad3<CT>(p.a*alpha, p.b*alpha, p.c*alpha, p.d*alpha); }

template < class CT >
inline quad2<CT>& operator *= ( quad2<CT>& p, double alpha )
{ p.a *= alpha; p.b *= alpha;  p.c *= alpha; p.d *= alpha;  return p; }

template < class CT >
inline quad3<CT>& operator *= ( quad3<CT>& p, double alpha )
{ p.a *= alpha; p.b *= alpha;  p.c *= alpha; p.d *= alpha;  return p; }

//======== div scalar
template < class CT >
inline quad2<CT> operator / ( const quad2<CT>& p, double alpha )
{ return quad2<CT>(p.a/alpha, p.b/alpha, p.c/alpha, p.d/alpha); }

template < class CT >
inline quad2<CT>& operator /= ( quad2<CT>& p, double alpha )
{ p.a /= alpha; p.b /= alpha;  p.c /= alpha; p.d /= alpha;  return p; }

template < class CT >
inline quad3<CT> operator / ( const quad3<CT>& p, double alpha )
{ return quad3<CT>(p.a/alpha, p.b/alpha, p.c/alpha, p.d/alpha); }

template < class CT >
inline quad3<CT>& operator /= ( quad3<CT>& p, double alpha )
{ p.a /= alpha; p.b /= alpha;  p.c /= alpha; p.d /= alpha;  return p; }

//========== unary minus
template < class CT >
inline quad2<CT> operator - ( const quad2<CT>& p )
{ return quad2<CT>(-p.a, -p.b, -p.c, -p.d ); }

template < class CT >
inline quad3<CT> operator - ( const quad3<CT>& p )
{ return quad3<CT>(-p.a, -p.b, -p.c, -p.d ); }

//========== plus minus point
//2d
template < class CT >
inline quad2<CT> operator + ( const quad2<CT>& p, const pnt2<CT>& q )
{ return quad2<CT>(p.a+q, p.b+q, p.c+q, p.d+q); }

template < class CT >
inline quad2<CT>& operator += ( quad2<CT>& p, const pnt2<CT>& add_q )
{ p.a += add_q; p.b += add_q; p.c += add_q; p.d += add_q; return p;}

template < class CT >
inline quad2<CT> operator - ( const quad2<CT>& p, const pnt2<CT>& q )
{ return quad2<CT>(p.a-q, p.b-q, p.c-q, p.d-q); }

template < class CT >
inline quad2<CT>& operator -= ( quad2<CT>& p, const pnt2<CT>& add_q )
{ p.a -= add_q; p.b -= add_q; p.c -= add_q; p.d -= add_q; return p; }
// 3d
template < class CT >
inline quad3<CT> operator + ( const quad3<CT>& p, const pnt3<CT>& q )
{ return quad3<CT>(p.a+q, p.b+q, p.c+q, p.d+q); }

template < class CT >
inline quad3<CT>& operator += ( quad3<CT>& p, const pnt3<CT>& add_q )
{ p.a += add_q; p.b += add_q; p.c += add_q; p.d += add_q; return p;}

template < class CT >
inline quad3<CT> operator - ( const quad3<CT>& p, const pnt3<CT>& q )
{ return quad3<CT>(p.a-q, p.b-q, p.c-q, p.d-q); }

template < class CT >
inline quad3<CT>& operator -= ( quad3<CT>& p, const pnt3<CT>& add_q )
{ p.a -= add_q; p.b -= add_q; p.c -= add_q; p.d -= add_q; return p; }

//=== misc

template < class CT >
inline double perimeter( const quad2<CT>& p )
{ return length( p.b-p.a)+length( p.c-p.b)+length( p.d-p.c)+length( p.a-p.d); }
template < class CT >
inline double perimeter( const quad3<CT>& p )
{ return length( p.b-p.a)+length( p.c-p.b)+length( p.d-p.c)+length( p.a-p.d); }

template < class CT >
inline double area( const quad2<CT>& p )
{ return std::abs(cross(p.c - p.a, p.d - p.b)) / 2; }
template < class CT >
inline double area( const quad3<CT>& p )
{ return length(cross( p.c - p.a, p.d - p.b )) / 2; }


template < class CT >
inline quad2<CT>& invert_abcd2dcba( quad2<CT>& q ) // abcd --> dcba
{
  std::swap( q.a, q.d );
  std::swap( q.b, q.c );
  return q;
}

template < class CT >
inline quad2<CT>& invert_abcd2adcb( quad2<CT>& q ) // abcd --> adcb
{
  std::swap( q.d, q.b );
  return q;
}


template < class CT >
inline quad2<CT>& shift_abcd2bcda( quad2<CT>& q ) // abcd --> bcda
{
  pnt2<CT> t = q.a; q.a = q.b; q.b = q.c; q.c = q.d; q.d = t;
  return q;
}

template < class quad >
double sides_ratio( quad& q )
{
  double len1 = length( q.b-q.a ) + length( q.c-q.d );
  double len2 = length( q.c-q.b ) + length( q.d-q.a );
  if (std::max( len1, len2 ) < 0.000000001)
    return 1.;
  return std::min( len1, len2 ) / std::max( len1, len2 );
}

template < class quad >
inline double max_cosangle( quad& q )
// returns [-1...1] ranged cosine of angle between a and b (or between b and a, symmetric)
{
  return max4(
    cosangle( q.d-q.a, q.b-q.a ),
    cosangle( q.a-q.b, q.c-q.b ),
    cosangle( q.b-q.c, q.d-q.c ),
    cosangle( q.c-q.d, q.a-q.d )
    );
}

template < class quad >
inline double min_cosangle( quad& q )
// returns [-1...1] ranged cosine of angle between a and b (or between b and a, symmetric)
{
  return min4(
    cosangle( q.d-q.a, q.b-q.a ),
    cosangle( q.a-q.b, q.c-q.b ),
    cosangle( q.b-q.c, q.d-q.c ),
    cosangle( q.c-q.d, q.a-q.d )
    );
}


} // namespace se

typedef se::quad2< int32_t > MinQuad2i;
typedef se::quad2< int16_t > MinQuad2s;
typedef se::quad2< real32_t > MinQuad2f;
typedef se::quad2< real64_t > MinQuad2d;

typedef MinQuad2i MinQuad;

typedef se::quad3< int32_t > MinQuad3i;
typedef se::quad3< int16_t > MinQuad3s;
typedef se::quad3< real32_t > MinQuad3f;
typedef se::quad3< real64_t > MinQuad3d;

#endif // __cplusplus


//// projective.h

#ifdef __cplusplus

class HCoords // image to homogenious coords translation
{
private:
	double xwi; ///< image width
	double yhe; ///< image height
	double zde; ///< z-depth // 0 -> auto -> sqrt( wi*wi+he*he )  // depth of view point; positive
public:
  double x() const { return xwi; }
  double y() const { return yhe; }
  double z() const { return zde; }

  double image_width() const { return xwi; }
  MinPoint2d image_size() const { return MinPoint2d(xwi, yhe); }
  double image_height() const { return yhe; }
  double focal_length() const { return zde; }

  void setup( double _xwi, double _yhe )
  {
    xwi = _xwi;
    yhe = _yhe;
//#define LIKE_IPHONE_CAMERA
#ifdef LIKE_IPHONE_CAMERA
    zde = (1.732050) * std::max( xwi, yhe );
#else  // old version
    zde = 0.5*sqrt( xwi*xwi+yhe*yhe );
#endif
  }

  HCoords(){ setup(1,1); }
  HCoords( double xwi, double yhe ) { setup( xwi, yhe ); }

  MinPoint2d to2d( const MinPoint3d& a ) { return MinPoint2d( a.x + xwi/2.,  a.y + yhe/2. ); }
  MinPoint3d to3d( const MinPoint2f& a ) { return MinPoint3d( a.x - xwi/2.,  a.y - yhe/2., zde ); }

};


namespace se {

// different focal lenths
// use: image width * value
#define FOCAL_LENGTH_DEFAULT 0.625
#define FOCAL_LENGTH_IPH4 0.838
#define FOCAL_LENGTH_IPH4S 0.932

template< class CT >
inline bool restore_parallelogram_by_projection(
  const quad2< CT >& prj, // projection
  quad3< CT >& res, // result parallelogram
  const CT& flen, // known focal length
  double& angle_to_normal // angle to quadrangle plane in radians
  )
{
  pnt3<CT> p( prj.a.x-prj.b.x,  prj.c.x-prj.b.x, prj.b.x-prj.d.x );
  pnt3<CT> q( prj.a.y-prj.b.y,  prj.c.y-prj.b.y, prj.b.y-prj.d.y );
  pnt3<CT> m = cross( p, q );
  if (std::abs(m.z) < 0.000000001) // todo use EPS...
    return false;
  m /= m.z;
  double mx = static_cast<double>(m.x) / m.z;
  double my = static_cast<double>(m.y) / m.z;
  double lambda1 = mx;
  double lambda3 = my;
  double lambda2 = lambda1 + lambda3 - 1.0;
  double lambda4 = 1.0;

  if (lambda1 <= 0.0 || lambda2 <= 0.0 || lambda3 <= 0.0) // assuming that lambda4 > 0 ( == 1)
    return false;

  res.a = pnt3<CT>(prj.a, flen) * lambda1;
  res.b = pnt3<CT>(prj.b, flen) * lambda2;
  res.c = pnt3<CT>(prj.c, flen) * lambda3;
  res.d = pnt3<CT>(prj.d, flen) * lambda4;

  // new:
  pnt3<CT> u = res.b - res.a;
  pnt3<CT> v = res.c - res.a;
  pnt3<CT> n = cross( u, v );
  pnt3<CT> e( 0, 0, 1);
  double dd = dot( n, e );
  double nl = length( n );
  dd /= nl;
  angle_to_normal = acos( dd );
  //std::cout << angle_to_normal * 180/3.1415 << std::endl;
  return true;
}


template< class CT >
inline bool restore_parallelogram_by_projection(
  const quad2< CT >& prj, // projection
  quad3< CT >& res, // result parallelogram
  const CT& flen // known focal length
  )
{
  double angle_to_normal = 0.0;
  return restore_parallelogram_by_projection<CT>(
      prj, res, flen, angle_to_normal);
}


template< class CT >
inline bool restore_focal_length_by_projection_of_rectangle(
  const quad2< CT >& prj, // projection of rectangle
  CT& flen // focal length to compute
  )
{
  pnt3< CT > p( prj.a.x-prj.b.x,  prj.c.x-prj.b.x, prj.b.x-prj.d.x );
  pnt3< CT > q( prj.a.y-prj.b.y,  prj.c.y-prj.b.y, prj.b.y-prj.d.y );
  pnt3< CT > m = cross( p, q );
  if (std::abs(m.z) < 0.000000001) // todo use EPS...
    return false;
  m /= m.z;
  CT lambda1 = m.x;
  CT lambda3 = m.y;
  CT lambda2 = lambda1 + lambda3 - 1;
  CT lambda4 = 1;

  if (lambda1 <= 0 || lambda2 <=0 || lambda3 <=0 ) // assuming that lambda4 > 0 ( == 1)
    return false;

  quad3< CT > res;
  res.a = pnt3< CT >( prj.a, flen ) * lambda1;
  res.b = pnt3< CT >( prj.b, flen ) * lambda2;
  res.c = pnt3< CT >( prj.c, flen ) * lambda3;
  res.d = pnt3< CT >( prj.d, flen ) * lambda4;

  double f2 = - ((res.b.x - res.a.x) * (res.d.x - res.a.x) + (res.b.y - res.a.y) * (res.d.y - res.a.y))
    / ((lambda2 - lambda1) * (lambda4 - lambda1));

  if (f2 < 0)
    f2 = -f2;//return false;

  flen = sqrt( f2 );

  return true;
}


template< class CT >
inline bool restore_rectangle_by_projection(
  const quad2< CT >& prj, // projection of rectangle
  quad3< CT >& res, // restored rectangle
  CT& flen // focal length to compute
  )
{
  pnt3< CT > p( prj.a.x-prj.b.x,  prj.c.x-prj.b.x, prj.b.x-prj.d.x );
  pnt3< CT > q( prj.a.y-prj.b.y,  prj.c.y-prj.b.y, prj.b.y-prj.d.y );
  pnt3< CT > m = cross( p, q );
  if ( std::abs(m.z) < 0.000000001 ) // todo use EPS...
    return false;
  m /= m.z;
  CT lambda1 = m.x;
  CT lambda3 = m.y;
  CT lambda2 = lambda1+lambda3-1;
  CT lambda4 = 1;

  if (lambda1 <= 0 || lambda2 <=0 || lambda3 <=0 ) // assuming that lambda4 > 0 ( == 1)
    return false;

  res.a = pnt3< CT >( prj.a, flen ) * lambda1;
  res.b = pnt3< CT >( prj.b, flen ) * lambda2;
  res.c = pnt3< CT >( prj.c, flen ) * lambda3;
  res.d = pnt3< CT >( prj.d, flen ) * lambda4;

  double f2 = - ((res.b.x - res.a.x) * (res.d.x - res.a.x) + (res.b.y - res.a.y) * (res.d.y - res.a.y))
    / ((lambda2 - lambda1) * (lambda4 - lambda1));
  //if (f2 < 0)
  //  return false;

  flen = sqrt( std::abs(f2) );

  res.a.z = flen*lambda1;
  res.b.z = flen*lambda2;
  res.c.z = flen*lambda3;
  res.d.z = flen*lambda4;

  return true;
}

template< class CT >
inline bool restore_4th_side_by_3_and_r(
  const pnt2< CT >& a, // point a
  const pnt2< CT >& b, // point b
  const pnt3< CT >& p, // vector to the intersection point
  const double& r, // sides ratio
  CT& flen, // focal length
  quad2< CT >& res // result
  )
{
  pnt3< CT > pp = p / length( p );
  double t4;
  pnt3< CT > c, d, qq, tmp1, cc, dd;
  pnt3< CT > aa(a, flen);
  pnt3< CT > bb(b, flen);
  res.a = a;
  res.b = b;

  if (fabs(dot( pp, (bb - aa) ) ) <  1e-8)
    qq = (bb - aa) / length (bb - aa);
  else
    qq = aa - dot( pp, aa ) / dot( pp, (bb - aa) ) * (bb - aa);
  // this was on the previous line and generated warning
  // about misleading indent
  tmp1 = cross( pp, qq );

  bb *= dot( tmp1, aa ) / dot( tmp1, bb );

  t4 = r * length( aa - bb );
  cc = bb + pp * t4;
  dd = aa + pp * t4;

  cc *= flen / cc.z;
  dd *= flen / dd.z;

  res.c.x = cc.x; res.c.y = cc.y;
  res.d.x = dd.x; res.d.y = dd.y;
  return true;
}



} // namespace se

#endif // __cplusplus



//// projective_rect.h

#ifdef __cplusplus

namespace se {

// different focal lenths
// use: image width * value
#define FOCAL_LENGTH_DEFAULT 0.625
#define FOCAL_LENGTH_IPH4 0.838
#define FOCAL_LENGTH_IPH4S 0.932



template< class CT >
inline bool delta_point(
  const pnt2< CT >& point, // given point
  pnt2 < CT >* pntr,       // delta-neighborhood of given point pointer
  const CT& delta          // parameter delta of the delta-neighborhood
  )
{
  pntr->x = point.x - delta;
  pntr->y = point.y - delta;

  (pntr+1)->x = point.x - delta;
  (pntr+1)->y = point.y + delta;

  (pntr+2)->x = point.x + delta;
  (pntr+2)->y = point.y + delta;

  (pntr+3)->x = point.x + delta;
  (pntr+3)->y = point.y - delta;

  return true;
};


template< class CT >
inline bool is_it_true_that_image_is_rectangle(
  const quad2< CT >& prj, // projection of rectangle
  const CT& flen,               // focal length to compute
  const CT& rat,                // ratio of sides
  const CT& accuracy            // maximal apropriate error
  )
{
  pnt2< CT > PA[4], PB[4], PC[4], PD[4];
  CT  ratio[4][4][4][4];
  CT  cosan[4][4][4][4];
  CT  maxratio = -1;
  CT  minratio = 10000000.;
  CT  maxcosan = -2.;
  CT  mincosan = 2.;

  delta_point(prj.a, PA, accuracy);
  delta_point(prj.b, PB, accuracy);
  delta_point(prj.c, PC, accuracy);
  delta_point(prj.d, PD, accuracy);
  double angle_to_normal = 0;

  int fl = 0;
  for(int i = 0; i < 4; ++i) {
    for(int j = 0; j < 4; ++j) {
      for(int k = 0; k < 4; ++k) {
        for(int l = 0; l < 4; ++l) {
          quad2< CT > quadr(PA[i], PB[j], PC[k], PD[l]);
          quad3< CT > quad_3d;
          if(!restore_parallelogram_by_projection(quadr, quad_3d, flen, angle_to_normal))
            fl = 1;
          else {
            ratio[i][j][k][l] = length(quad_3d.a-quad_3d.b) / length(quad_3d.a-quad_3d.d);
            cosan[i][j][k][l] = cosangle((quad_3d.a-quad_3d.b), (quad_3d.a-quad_3d.d));

            if(cosan[i][j][k][l] > maxcosan)
              maxcosan = cosan[i][j][k][l];
            if(cosan[i][j][k][l] < mincosan)
              mincosan = cosan[i][j][k][l];
            if(ratio[i][j][k][l] > maxratio)
              maxratio = ratio[i][j][k][l];
            if(ratio[i][j][k][l] < minratio)
              minratio = ratio[i][j][k][l];
          }
        }
      }
    }
  }

  if(mincosan <= 0 && maxcosan >= 0 && minratio <= rat && maxratio >= rat)
    fl = 1;

  return (fl == 1);
}


template< class CT >
inline double err_dist(         // distance in error-space
  const quad2< CT >& prj,       // projection of rectangle
  const CT& flen,               // focal length to compute
  const CT& rat                 // ratio of sides
  )
{
  double accuracy;              // apropriate error
  double acc1 = 0.0;
  double acc2 = length(prj.a-prj.b)+1.0;

  while(acc2 - acc1 > 0.0001) {
    accuracy = (acc2 + acc1) / 2;
    if(is_it_true_that_image_is_rectangle(prj, flen, rat, accuracy))
      acc2 = accuracy;
    else
      acc1 = accuracy;
  }

  return accuracy;
}



template< class CT >
inline bool normal_check(
  const quad2< CT >& prj,       // projection of rectangle
  const CT& flen,               // focal length to compute
  const CT& min_cos             // minimal apropriate value of the normal angle cosinus
  )
{
  quad3< CT > quad_3d;
  pnt3< CT > v1, v2, norm;
  double cos;

  restore_parallelogram_by_projection(prj, quad_3d, flen);

  v1 = quad_3d.a - quad_3d.c;
  v2 = quad_3d.b - quad_3d.d;
  norm = cross(v1, v2);

  cos = (1.0 * norm.z) / length(norm);
  cos = (cos < 0) ? -1.0 * cos : cos;

  return (cos > (double) min_cos);
}


template< class CT >
inline bool numerical_Jacobi(
  const quad2< CT >& prj,       // projection of rectangle
  const CT& flen,               // focal length to compute
  std::vector< std::pair<double,double> >& J  // Jacobi matrix
  )
// (x1,y1), (x2,y2), (x3,y3), (x4,y4) --> phi1(x1,...,y4), phi2(x1,...,y4) = ( ratio^2, cos^2(alpha) )
//
//     | \frac{\partial phi1}{\partial x1} ... \frac{\partial phi1}{\partial y4} |
// J = |                                                                         |
//     | \frac{\partial phi2}{\partial x1} ... \frac{\partial phi2}{\partial y4} |
{
  bool res = true;
  double eps = 0.001;
  J.resize(8);

  for(int i = 0; i < 4; ++i) {
    quad3< double > paral1, paral2;
    quad2< double > quadr1 = prj, quadr2 = prj;
    quadr1[i].x += eps/2;
    quadr2[i].x -= eps/2;
    if(! restore_parallelogram_by_projection(quadr1, paral1, flen))
      res = false;
    if(! restore_parallelogram_by_projection(quadr2, paral2, flen))
      res = false;
    double r1 = length(paral1.a-paral1.b) / length(paral1.a-paral1.d);
    double r2 = length(paral2.a-paral2.b) / length(paral2.a-paral2.d);
    double c1 = cosangle((paral1.a-paral1.b), (paral1.a-paral1.d));
    double c2 = cosangle((paral2.a-paral2.b), (paral2.a-paral2.d));
    J[i].first  = (r1*r1 - r2*r2) / eps;
    J[i].second = (c1*c1 - c2*c2) / eps;
  }

  for(int i = 0; i < 4; ++i) {
    quad3< double > paral1, paral2;
    quad2< double > quadr1 = prj, quadr2 = prj;
    quadr1[i].y += eps/2;
    quadr2[i].y -= eps/2;
    if(! restore_parallelogram_by_projection(quadr1, paral1, flen))
      res = false;
    if(! restore_parallelogram_by_projection(quadr2, paral2, flen))
      res = false;
    double r1 = length(paral1.a-paral1.b) / length(paral1.a-paral1.d);
    double r2 = length(paral2.a-paral2.b) / length(paral2.a-paral2.d);
    double c1 = cosangle((paral1.a-paral1.b), (paral1.a-paral1.d));
    double c2 = cosangle((paral2.a-paral2.b), (paral2.a-paral2.d));
    J[i+4].first  = (r1*r1 - r2*r2) / eps;
    J[i+4].second = (c1*c1 - c2*c2) / eps;
  }

  return res;
}



template< class CT >
inline bool Jacobi(
  const quad2< CT >& prj,       // projection of rectangle
  const CT& flen,               // focal length to compute
  std::vector< std::pair<double,double> >& J  // Jacobi matrix
  )
// (x1,y1), (x2,y2), (x3,y3), (x4,y4) --> phi1(x1,...,y4), phi2(x1,...,y4) = ( ratio^2, cos^2(alpha) )
//
//     | \frac{\partial phi1}{\partial x1} ... \frac{\partial phi1}{\partial y4} |
// J = |                                                                         |
//     | \frac{\partial phi2}{\partial x1} ... \frac{\partial phi2}{\partial y4} |
{
  bool res = true;
  double ff = flen * flen;
  J.resize(8);

  double x1 = prj.a.x;
  double y1 = prj.a.y;
  double x2 = prj.b.x;
  double y2 = prj.b.y;
  double x3 = prj.c.x;
  double y3 = prj.c.y;
  double x4 = prj.d.x;
  double y4 = prj.d.y;

  std::vector<double> A(9, 0.0), B(9, 0.0), C(9, 0.0);
  // A[8] = A, A[0] = \frac{\partial A}\{\partial x1}, ..., A[7] = \frac{\partial A}{\partial y4}
  // B[8] = B, B[0] = \frac{\partial B}\{\partial x1}, ..., B[7] = \frac{\partial B}{\partial y4}
  // C[8] = C, C[0] = \frac{\partial C}\{\partial x1}, ..., C[7] = \frac{\partial C}{\partial y4}

  std::vector<double> k1(9, 0.0), k2(9, 0.0), k3(9, 0.0), k4(9, 0.0);
  k1[8] = x4*y3-y4*x3 + x3*y2-y3*x2 + x2*y4-y2*x4;
  k2[8] = x4*y3-y4*x3 + x3*y1-y3*x1 + x1*y4-y1*x4;
  k3[8] = x4*y2-y4*x2 + x2*y1-y2*x1 + x1*y4-y1*x4;
  k4[8] = x3*y2-y3*x2 + x2*y1-y2*x1 + x1*y3-y1*x3;

  k1[0] = 0;
  k1[1] = y4 - y3;
  k1[2] = y2 - y4;
  k1[3] = y3 - y2;
  k1[4] = 0;
  k1[5] = x3 - x4;
  k1[6] = x4 - x2;
  k1[7] = x2 - x3;

  k2[0] = y4 - y3;
  k2[1] = 0;
  k2[2] = y1 - y4;
  k2[3] = y3 - y1;
  k2[4] = x3 - x4;
  k2[5] = 0;
  k2[6] = x4 - x1;
  k2[7] = x1 - x3;

  k3[0] = y4 - y2;
  k3[1] = y1 - y4;
  k3[2] = 0;
  k3[3] = y2 - y1;
  k3[4] = x2 - x4;
  k3[5] = x4 - x1;
  k3[6] = 0;
  k3[7] = x1 - x2;

  k4[0] = y3 - y2;
  k4[1] = y1 - y3;
  k4[2] = y2 - y1;
  k4[3] = 0;
  k4[4] = x2 - x3;
  k4[5] = x3 - x1;
  k4[6] = x1 - x2;
  k4[7] = 0;


  A[8] = (k1[8]*x1-k2[8]*x2)*(k1[8]*x1-k2[8]*x2) + (k1[8]*y1-k2[8]*y2)*(k1[8]*y1-k2[8]*y2) + (k1[8]-k2[8])*(k1[8]-k2[8])*ff;
  B[8] = (k1[8]*x1-k4[8]*x4)*(k1[8]*x1-k4[8]*x4) + (k1[8]*y1-k4[8]*y4)*(k1[8]*y1-k4[8]*y4) + (k1[8]-k4[8])*(k1[8]-k4[8])*ff;
  C[8] = (k1[8]*x1-k2[8]*x2)*(k1[8]*x1-k4[8]*x4) + (k1[8]*y1-k2[8]*y2)*(k1[8]*y1-k4[8]*y4) + (k1[8]-k2[8])*(k1[8]-k4[8])*ff;

  A[0] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[0]*x1+k1[8]-k2[0]*x2) + (k1[8]*y1-k2[8]*y2)*(k1[0]*y1-k2[0]*y2)       + (k1[8]-k2[8])*(k1[0]-k2[0])*ff );
  A[1] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[1]*x1-k2[1]*x2-k2[8]) + (k1[8]*y1-k2[8]*y2)*(k1[1]*y1-k2[1]*y2)       + (k1[8]-k2[8])*(k1[1]-k2[1])*ff );
  A[2] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[2]*x1-k2[2]*x2)       + (k1[8]*y1-k2[8]*y2)*(k1[2]*y1-k2[2]*y2)       + (k1[8]-k2[8])*(k1[2]-k2[2])*ff );
  A[3] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[3]*x1-k2[3]*x2)       + (k1[8]*y1-k2[8]*y2)*(k1[3]*y1-k2[3]*y2)       + (k1[8]-k2[8])*(k1[3]-k2[3])*ff );
  A[4] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[4]*x1-k2[4]*x2)       + (k1[8]*y1-k2[8]*y2)*(k1[4]*y1+k1[8]-k2[4]*y2) + (k1[8]-k2[8])*(k1[4]-k2[4])*ff );
  A[5] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[5]*x1-k2[5]*x2)       + (k1[8]*y1-k2[8]*y2)*(k1[5]*y1-k2[8]-k2[5]*y2) + (k1[8]-k2[8])*(k1[5]-k2[5])*ff );
  A[6] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[6]*x1-k2[6]*x2)       + (k1[8]*y1-k2[8]*y2)*(k1[6]*y1-k2[6]*y2)       + (k1[8]-k2[8])*(k1[6]-k2[6])*ff );
  A[7] = 2*( (k1[8]*x1-k2[8]*x2)*(k1[7]*x1-k2[7]*x2)       + (k1[8]*y1-k2[8]*y2)*(k1[7]*y1-k2[7]*y2)       + (k1[8]-k2[8])*(k1[7]-k2[7])*ff );

  B[0] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[0]*x1+k1[8]-k4[0]*x4) + (k1[8]*y1-k4[8]*y4)*(k1[0]*y1-k4[0]*y4)       + (k1[8]-k4[8])*(k1[0]-k4[0])*ff );
  B[1] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[1]*x1-k4[1]*x4)       + (k1[8]*y1-k4[8]*y4)*(k1[1]*y1-k4[1]*y4)       + (k1[8]-k4[8])*(k1[1]-k4[1])*ff );
  B[2] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[2]*x1-k4[2]*x4)       + (k1[8]*y1-k4[8]*y4)*(k1[2]*y1-k4[2]*y4)       + (k1[8]-k4[8])*(k1[2]-k4[2])*ff );
  B[3] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[3]*x1-k4[3]*x4-k4[8]) + (k1[8]*y1-k4[8]*y4)*(k1[3]*y1-k4[3]*y4)       + (k1[8]-k4[8])*(k1[3]-k4[3])*ff );
  B[4] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[4]*x1-k4[4]*x4)       + (k1[8]*y1-k4[8]*y4)*(k1[4]*y1+k1[8]-k4[4]*y4) + (k1[8]-k4[8])*(k1[4]-k4[4])*ff );
  B[5] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[5]*x1-k4[5]*x4)       + (k1[8]*y1-k4[8]*y4)*(k1[5]*y1-k4[5]*y4)       + (k1[8]-k4[8])*(k1[5]-k4[5])*ff );
  B[6] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[6]*x1-k4[6]*x4)       + (k1[8]*y1-k4[8]*y4)*(k1[6]*y1-k4[6]*y4)       + (k1[8]-k4[8])*(k1[6]-k4[6])*ff );
  B[7] = 2*( (k1[8]*x1-k4[8]*x4)*(k1[7]*x1-k4[7]*x4)       + (k1[8]*y1-k4[8]*y4)*(k1[7]*y1-k4[7]*y4-k4[8]) + (k1[8]-k4[8])*(k1[7]-k4[7])*ff );

  C[0] = (k1[0]*x1+k1[8]-k2[0]*x2)*(k1[8]*x1-k4[8]*x4) + (k1[0]*y1-k2[0]*y2)*(k1[8]*y1-k4[8]*y4)       + (k1[0]-k2[0])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[0]*x1+k1[8]-k4[0]*x4) + (k1[8]*y1-k2[8]*y2)*(k1[0]*y1-k4[0]*y4)       + (k1[8]-k2[8])*(k1[0]-k4[0])*ff;
  C[1] = (k1[1]*x1-k2[1]*x2-k2[8])*(k1[8]*x1-k4[8]*x4) + (k1[1]*y1-k2[1]*y2)*(k1[8]*y1-k4[8]*y4)       + (k1[1]-k2[1])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[1]*x1-k4[1]*x4)       + (k1[8]*y1-k2[8]*y2)*(k1[1]*y1-k4[1]*y4)       + (k1[8]-k2[8])*(k1[1]-k4[1])*ff;
  C[2] = (k1[2]*x1-k2[2]*x2)*(k1[8]*x1-k4[8]*x4)       + (k1[2]*y1-k2[2]*y2)*(k1[8]*y1-k4[8]*y4)       + (k1[2]-k2[2])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[2]*x1-k4[2]*x4)       + (k1[8]*y1-k2[8]*y2)*(k1[2]*y1-k4[2]*y4)       + (k1[8]-k2[8])*(k1[2]-k4[2])*ff;
  C[3] = (k1[3]*x1-k2[3]*x2)*(k1[8]*x1-k4[8]*x4)       + (k1[3]*y1-k2[3]*y2)*(k1[8]*y1-k4[8]*y4)       + (k1[3]-k2[3])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[3]*x1-k4[3]*x4-k4[8]) + (k1[8]*y1-k2[8]*y2)*(k1[3]*y1-k4[3]*y4)       + (k1[8]-k2[8])*(k1[3]-k4[3])*ff;
  C[4] = (k1[4]*x1-k2[4]*x2)*(k1[8]*x1-k4[8]*x4)       + (k1[4]*y1+k1[8]-k2[4]*y2)*(k1[8]*y1-k4[8]*y4) + (k1[4]-k2[4])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[4]*x1-k4[4]*x4)       + (k1[8]*y1-k2[8]*y2)*(k1[4]*y1+k1[8]-k4[4]*y4) + (k1[8]-k2[8])*(k1[4]-k4[4])*ff;
  C[5] = (k1[5]*x1-k2[5]*x2)*(k1[8]*x1-k4[8]*x4)       + (k1[5]*y1-k2[5]*y2-k2[8])*(k1[8]*y1-k4[8]*y4) + (k1[5]-k2[5])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[5]*x1-k4[5]*x4)       + (k1[8]*y1-k2[8]*y2)*(k1[5]*y1-k4[5]*y4)       + (k1[8]-k2[8])*(k1[5]-k4[5])*ff;
  C[6] = (k1[6]*x1-k2[6]*x2)*(k1[8]*x1-k4[8]*x4)       + (k1[6]*y1-k2[6]*y2)*(k1[8]*y1-k4[8]*y4)       + (k1[6]-k2[6])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[6]*x1-k4[6]*x4)       + (k1[8]*y1-k2[8]*y2)*(k1[6]*y1-k4[6]*y4)       + (k1[8]-k2[8])*(k1[6]-k4[6])*ff;
  C[7] = (k1[7]*x1-k2[7]*x2)*(k1[8]*x1-k4[8]*x4)       + (k1[7]*y1-k2[7]*y2)*(k1[8]*y1-k4[8]*y4)       + (k1[7]-k2[7])*(k1[8]-k4[8])*ff +\
         (k1[8]*x1-k2[8]*x2)*(k1[7]*x1-k4[7]*x4)       + (k1[8]*y1-k2[8]*y2)*(k1[7]*y1-k4[7]*y4-k4[8]) + (k1[8]-k2[8])*(k1[7]-k4[7])*ff;


  for(int i = 0; i < 8; ++i) {
    J[i].first  = (A[i]*B[8] - A[8]*B[i]) / (B[8]*B[8]);
    J[i].second = (2*C[8]*C[i]*A[8]*B[8] - C[8]*C[8]*A[i]*B[8] - C[8]*C[8]*A[8]*B[i]) / (A[8]*A[8]*B[8]*B[8]);
  }

  return res;
}


} // namespace se

#endif // __cplusplus



//// mingeo.h

#ifdef __cplusplus

namespace se {

template < class CT >
inline bool inside( CT a, CT x, CT b )  // a <= x <= b
{ return a <= x && x <= b; }

template < class CT >
inline bool inside( rect2<CT>& rc, pnt2< CT > p ) // point inside rectangle
{ return  inside( rc.x , p.x, rc.x + rc.width ) &&
          inside( rc.y , p.y, rc.y + rc.height );
}

template < class CT >
inline bool inside( rect3<CT>& rc, pnt3< CT > p ) // point inside rectangle 3d
{ return  inside( rc.x , p.x, rc.x + rc.width ) &&
          inside( rc.y , p.y, rc.y + rc.height ) &&
          inside( rc.z , p.z, rc.z + rc.depth );
}

template < class CT >
inline bool inside( rect2<CT>& rc, segm2< CT > s ) // segment inside rectangle
{ return  inside( rc, s.a ) && inside( rc, s.b ); }

template < class CT >
inline bool inside( rect3<CT>& rc, segm3< CT > s ) // segment inside rectangle
{ return  inside( rc, s.a ) && inside( rc, s.b ); }

}; //se

#endif // __cplusplus


#endif // #ifndef MINGEO_MINGEO_H_INCLUDED
