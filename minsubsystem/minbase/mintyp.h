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

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR
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

/**
 * @file   mintyp.h
 * @brief  Definition of acceptable image types.
 */

#pragma once

#ifndef MINSBASE_MINTYP_H_INCLUDED
#define MINSBASE_MINTYP_H_INCLUDED


#define __STDC_CONSTANT_MACROS // To have UINT64_C and other macros.
#if defined(_MSC_VER) && (_MSC_VER < 1600)
#include <minutils/stdint-vc.h>
#else
#ifdef _STDINT_H
#undef _STDINT_H
#endif
#include <stdint.h>
#endif // _MSC_VER

#include <minbase/macro_helpers.h>

/**
 * @brief   Specifies acceptable element formats of each individual channel.
 * @details The enum specifies acceptable element formats of each
 *          individual channel.
 * @ingroup MinUtils_MinImg
 */
typedef enum
{
  FMT_UINT, ///< Unsigned integer.
  FMT_INT,  ///< Signed integer.
  FMT_REAL ///< Floating point.
} MinFmt;

/**
 * @brief   Specifies acceptable element types of each individual channel.
 * @details The enum specifies acceptable element types (that is format + size)
 *          of each individual channel.
 * @ingroup MinUtils_MinImg
 */
typedef enum
{
  TYP_UINT1,    ///< 1-bit logical.
  TYP_UINT8,    ///< Unsigned 8-bit integer.
  TYP_INT8,     ///< Signed 8-bit integer.
  TYP_UINT16,   ///< Unsigned 16-bit integer.
  TYP_INT16,    ///< Signed 16-bit integer.
  TYP_REAL16,  ///< Half-precision floating point.
  TYP_UINT32,   ///< Unsigned 32-bit integer.
  TYP_INT32,    ///< Signed 32-bit integer.
  TYP_REAL32,  ///< Single-precision floating point.
  TYP_UINT64,   ///< Unsigned 64-bit integer.
  TYP_INT64,    ///< Signeincompatible with uint8_td 64-bit integer.
  TYP_REAL64   ///< Double-precision floating point.
} MinTyp;

/**
 * @brief   Specifies pack of 8 1-bit fields.
 * @details The struct @c uint1_t represents pack of 8 1-bit fields
 *          incompatible with uint8_t.
 * @ingroup MinUtils_MinImg
 */
struct uint1_t {
  uint8_t b0 : 1;
  uint8_t b1 : 1;
  uint8_t b2 : 1;
  uint8_t b3 : 1;
  uint8_t b4 : 1;
  uint8_t b5 : 1;
  uint8_t b6 : 1;
  uint8_t b7 : 1;
};

DECLARE_COMPOUND_WITH_TYPEDEF(struct, uint1_t);

/**
 * @brief   Specifies half-precision floating point.
 * @details The struct @c real16 represents half-precision floating point.
 * @ingroup MinUtils_MinImg
 */
typedef struct
{
  uint16_t significand : 10;  ///< The mantissa of the number.
  uint16_t exponent    : 5;   ///< The magnitude of the number.
  uint16_t sign        : 1;   ///< The sign of the number.
} real16;

typedef real16  real16_t;  ///<  Specifies @c real16 as @c real16_t.

typedef float   real32_t;  ///<  Specifies @c float as @c real32_t type.
typedef double  real64_t;  ///<  Specifies @c double as @c real64_t type.

#endif // #ifndef MINSBASE_MINTYP_H_INCLUDED
