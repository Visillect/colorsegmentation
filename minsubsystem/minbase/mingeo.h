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
#ifndef MINSBASE_MINGEO_H_INCLUDED
#define MINSBASE_MINGEO_H_INCLUDED

#include <minbase/mintyp.h>
#include <minbase/crossplat.h>

#ifdef __cplusplus
#include <cmath>
#endif


//// minpoint.h

#ifndef __cplusplus
/**
 * @brief   2D point <x,y> with integer/real coordinates.
 * @details @c MinPoint represents a 2D point where x and y are 32 bit integers.
 *          @c MinPoint2f represents a 2D point where x and y are 32 bit float.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  int32_t x; ///< The x-coordinate of the point
  int32_t y; ///< The y-coordinate of the point
} MinPoint2i;

typedef struct
{
  int16_t x; ///< The x-coordinate of the point
  int16_t y; ///< The y-coordinate of the point
} MinPoint2s;

typedef struct
{
  real32_t x; ///< The x-coordinate of the point
  real32_t y; ///< The y-coordinate of the point
} MinPoint2f;

typedef struct
{
  real64_t x; ///< The x-coordinate of the point
  real64_t y; ///< The y-coordinate of the point
} MinPoint2d;

typedef MinPoint2i MinPoint;


/**
 * @brief   3D point with real coordinates (zero-based).
 * @details The structure @c MinPoint2f represents a 3D point as a triplet (x, y, z),
 *          where x, y and z are real.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  real32_t x; ///< The x-coordinate of the point.
  real32_t y; ///< The y-coordinate of the point.
  real32_t z; ///< The z-coordinate of the point.
} MinPoint3f;

#endif  // !__cplusplus


//// minsegm.h

#ifndef __cplusplus
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
#endif  // !__cplusplus

//---------------------------------------------------------- new 23 feb 2012

#ifndef __cplusplus
/**
 * @brief   2D segment <a,b> of <x,y> points with integer/real coordinates.
 * @details @c MinSegm2[s|i|f|d] represents a 2D segment of line where a and b are 2d endpoints.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  MinPoint a; ///< staring 2d point (integer coords)
  MinPoint b; ///< ending 2d point (integer coords)
} MinSegm;

typedef struct
{
  MinPoint2f a; ///< staring 2d point (real coords, single precision)
  MinPoint2f b; ///< ending 2d point (real coords, single precision)
} MinSegm2f;


#endif  // !__cplusplus
//---------------------------------------------------------- end of new  23 feb 2012


//// minsize.h

#ifndef __cplusplus

/**
 * @brief   Size of an image or a rectangle.
 * @details The struct @c MinSize represents a size of an image or a rectangle
 *          as a tuple (width, height), where width and height are integers.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  int32_t width;    ///< The width of the rectangle or the image.
  int32_t height;   ///< The height of the rectangle or the image.
} MinSize;

typedef struct
{
  real32_t width;   ///< The width of the rectangle or the image.
  real32_t height;  ///< The height of the rectangle or the image.
} MinSize2f;

typedef struct
{
  int32_t width;    ///< The width of the rectangle or the image.
  int32_t height;   ///< The height of the rectangle or the image.
  int32_t depth;    ///< The depth of the rectangle or the image.
} MinSize3i;

typedef struct
{
  real32_t width;   ///< The width of the rectangle or the image.
  real32_t height;  ///< The height of the rectangle or the image.
  real32_t depth;   ///< The depth of the rectangle or the image.
} MinSize3f;

#endif  // __cplusplus


//// minrect.h

#ifndef __cplusplus
/**
 * @brief   2D rectangle <x,y,width,height> with integer/real coordinates.
 * @details @c MinRect represents a 2D rectangle where x,y,width,height are 32 bit integers.
 *          @c MinRect2f represents a 2D rectangle where x,y,width,height are 32 bit float.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  int32_t x; ///< The x-coordinate of the left-top corner point
  int32_t y; ///< The y-coordinate of the left-top corner point
  int32_t width;  ///< The width (x-size) of the rectangle
  int32_t height; ///< The height (y-size) of the rectangle
} MinRect;

typedef struct
{
  real32_t x; ///< The x-coordinate of the left-top corner point
  real32_t y; ///< The y-coordinate of the left-top corner point
  real32_t width;  ///< The width (x-size) of the rectangle
  real32_t height; ///< The height (y-size) of the rectangle
} MinRect2f;


/**
 * @brief   3D rectangle with real coordinates (zero-based).
 * @details The structure @c MinRect3f represents a 3D rectangle
 *          (parallelepiped) as  (x, y, z, width, height, depth),
 *          where x, y, z, width, height, depth are real.
 * @ingroup MinUtils_Geometry
 */
typedef struct
{
  real32_t x; ///< The x-coordinate of the left-top corner point.
  real32_t y; ///< The y-coordinate of the left-top corner point.
  real32_t z; ///< The z-coordinate of the left-top corner point.
  real32_t width;  ///< The width (x-size) of the rectangle
  real32_t height; ///< The height (y-size) of the rectangle
  real32_t depth;  ///< The depth (z-size) of the rectangle
} MinRect3f;

#endif  // !__cplusplus


//// minband.h

/**
 * @brief   Band object represented as two ends points and band width.
 * @details The struct @c MinBand represents a band object as a tuple of two
 *          MinPoint's and band width.
 * @ingroup MinUtils_Geometry
 */

#ifndef __cplusplus
typedef struct
{
  MinPoint u;   ///< First point.
  MinPoint v;   ///< Second point.
  int32_t  w;   ///< Band width.
} MinBand;

typedef struct
{
  MinPoint2f u;   ///< First point
  MinPoint2f v;   ///< Second point
  real32_t   w;   ///< Band width
} MinBand2f;

#endif  // __cplusplus


//// minquad.h

/**
 * @brief   2D plain quadrangle with real (..2f, ..2d suffixes) or int (..2s, ..2i) coordinates.
 * @details The structure @c Minquad2f represents a 2D quadrangle as a quartet <a,b,c,d>
 *          where a, b, c, d are 2D real points MinPoint2x (x = f,d,s,i).
 * @ingroup MinUtils_Geometry
 */

#ifndef __cplusplus

typedef struct { MinPoint2f a,b,c,d; } Minquad2f; // float32
typedef struct { MinPoint2i a,b,c,d; } Minquad2i; // int32
typedef struct { MinPoint2s a,b,c,d; } Minquad2s; // int16
typedef struct { MinPoint2d a,b,c,d; } Minquad2d; // float64

//inline MUSTINLINE Minquad2f minquad2f(MinPoint2f a, MinPoint2f b, MinPoint2f c, MinPoint2f d)
//{
//  Minquad2f q = {a,b,c,d};
//  return q;
//}

#endif  // !__cplusplus

#endif // #ifndef MINSBASE_MINGEO_H_INCLUDED
