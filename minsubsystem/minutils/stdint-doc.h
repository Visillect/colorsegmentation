/*

Copyright (c) 2011, Smart Engines Limited. All rights reserved.

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
 * @file   stdint-doc.h
 * @brief  Documentation for <stdint.h> members.
 */

#pragma once
#ifndef MINUTILS_STDINT_VC_DOC_H_INCLUDED
#define MINUTILS_STDINT_VC_DOC_H_INCLUDED

/**
 * @file   stdint-vc.h
 * @brief  C99 standard library header file for use with MS VC++.
 * @author Alexander Chemeris

 */

/**
 * @defgroup MinUtils_Stdint C99 Standard Data Types
 * @brief    The module describes the @c stdint.h file with is a header file in
 *           the C standard library introduced in the C99 standard library
 *           section 7.18 to allow programmers to write more portable code by
 *           providing a set of typedefs that specify exact-width integer types,
 *           together with the defined minimum and maximum allowable values for
 *           each type, using macros. This header is particularly useful for
 *           embedded programming which often involves considerable manipulation
 *           of hardware specific I/O registers requiring integer data of fixed
 *           widths, specific locations and exact alignments.
 * @{
 */

/**
 * @def   _W64
 * @brief Defines @c _W64 macros to mark types changing their size,
 *        like @c intptr_t or @c uintptr_t.
 */

/**
 * @def   INT8_MIN
 * @brief Defines a minimum value of a signed 8-bit integer.
 */

/**
 * @def   INT8_MAX
 * @brief Defines a maximum value of a signed 8-bit integer.
 */

/**
 * @def   INT16_MIN
 * @brief Defines a minimum value of a signed 16-bit integer.
 */

/**
 * @def   INT16_MAX
 * @brief Defines a maximum value of a signed 16-bit integer.
 */

/**
 * @def   INT32_MIN
 * @brief Defines a minimum value of a signed 32-bit integer.
 */

/**
 * @def   INT32_MAX
 * @brief Defines a maximum value of a signed 32-bit integer.
 */

/**
 * @def   INT64_MIN
 * @brief Defines a minimum value of a signed 64-bit integer.
 */

/**
 * @def   INT64_MAX
 * @brief Defines a maximum value of a signed 64-bit integer.
 */

/**
 * @def   UINT8_MAX
 * @brief Defines a maximum value of an unsigned 8-bit integer.
 */

/**
 * @def   UINT16_MAX
 * @brief Defines a maximum value of an unsigned 16-bit integer.
 */

/**
 * @def   UINT32_MAX
 * @brief Defines a maximum value of an unsigned 32-bit integer.
 */

/**
 * @def   UINT64_MAX
 * @brief Defines a maximum value of an unsigned 64-bit integer.
 */

/**
 * @def   INT_LEAST8_MIN
 * @brief Defines a minimum value of a signed integer with a width
 *        of at least 8 bits.
 */

/**
 * @def   INT_LEAST8_MAX
 * @brief Defines a maximum value of a signed integer with a width
 *        of at least 8 bits.
 */

/**
 * @def   INT_LEAST16_MIN
 * @brief Defines a minimum value of a signed integer with a width
 *        of at least 16 bits.
 */

/**
 * @def   INT_LEAST16_MAX
 * @brief Defines a maximum value of a signed integer with a width
 *        of at least 16 bits.
 */

/**
 * @def   INT_LEAST32_MIN
 * @brief Defines a minimum value of a signed integer with a width
 *        of at least 32 bits.
 */

/**
 * @def   INT_LEAST32_MAX
 * @brief Defines a maximum value of a signed integer with a width
 *        of at least 32 bits.
 */

/**
 * @def   INT_LEAST64_MIN
 * @brief Defines a minimum value of a signed integer with a width
 *        of at least 64 bits.
 */

/**
 * @def   INT_LEAST64_MAX
 * @brief Defines a maximum value of a signed integer with a width
 *        of at least 64 bits.
 */

/**
 * @def   UINT_LEAST8_MAX
 * @brief Defines a maximum value of an unsigned integer with a width
 *        of at least 8 bits.
 */

/**
 * @def   UINT_LEAST16_MAX
 * @brief Defines a maximum value of an unsigned integer with a width
 *        of at least 16 bits.
 */

/**
 * @def   UINT_LEAST32_MAX
 * @brief Defines a maximum value of an unsigned integer with a width
 *        of at least 32 bits.
 */

/**
 * @def   UINT_LEAST64_MAX
 * @brief Defines a maximum value of an unsigned integer with a width
 *        of at least 64 bits.
 */

/**
 * @def   INT_FAST8_MIN
 * @brief Defines a minimum value of a fastest signed 8-bit integer.
 */

/**
 * @def   INT_FAST8_MAX
 * @brief Defines a maximum value of a fastest signed 8-bit integer.
 */

/**
 * @def   INT_FAST16_MIN
 * @brief Defines a minimum value of a fastest signed 16-bit integer.
 */

/**
 * @def   INT_FAST16_MAX
 * @brief Defines a maximum value of a fastest signed 16-bit integer.
 */

/**
 * @def   INT_FAST32_MIN
 * @brief Defines a minimum value of a fastest signed 32-bit integer.
 */

/**
 * @def   INT_FAST32_MAX
 * @brief Defines a maximum value of a fastest signed 32-bit integer.
 */

/**
 * @def   INT_FAST64_MIN
 * @brief Defines a minimum value of a fastest signed 64-bit integer.
 */

/**
 * @def   INT_FAST64_MAX
 * @brief Defines a maximum value of a fastest signed 64-bit integer.
 */

/**
 * @def   UINT_FAST8_MAX
 * @brief Defines a maximum value of a fastest unsigned 8-bit integer.
 */

/**
 * @def   UINT_FAST16_MAX
 * @brief Defines a maximum value of a fastest unsigned 16-bit integer.
 */

/**
 * @def   UINT_FAST32_MAX
 * @brief Defines a maximum value of a fastest unsigned 32-bit integer.
 */

/**
 * @def   UINT_FAST64_MAX
 * @brief Defines a maximum value of a fastest unsigned 64-bit integer.
 */

/**
 * @def   INTPTR_MIN
 * @brief Defines a minimum value of a signed integer which is guaranteed to
 *        hold the value of a pointer.
 */

/**
 * @def   INTPTR_MAX
 * @brief Defines a maximum value of a signed integer which is guaranteed to
 *        hold the value of a pointer.
 */

/**
 * @def   UINTPTR_MAX
 * @brief Defines a maximum value of an unsigned integer which is guaranteed to
 *        hold the value of a pointer.
 */

/**
 * @def   INTMAX_MIN
 * @brief Defines a minimum value of a signed integer which has the
 *        greatest limits.
 */

/**
 * @def   INTMAX_MAX
 * @brief Defines a maximum value of a signed integer which has the
 *        greatest limits.
 */

/**
 * @def   UINTMAX_MAX
 * @brief Defines a maximum value of an unsigned integer which has the
 *        greatest limits.
 */

/**
 * @def   PTRDIFF_MIN
 * @brief Defines a minimum value @c ptrdiff_t can hold.
 */

/**
 * @def   PTRDIFF_MAX
 * @brief Defines a maximum value @c ptrdiff_t can hold.
 */

/**
 * @def   SIG_ATOMIC_MIN
 * @brief Defines a minimum value @c sig_atomic_t can hold.
 */

/**
 * @def   SIG_ATOMIC_MAX
 * @brief Defines a maximum value @c sig_atomic_t can hold.
 */

/**
 * @def   SIZE_MAX
 * @brief Defines a maximum value @c size_t can hold.
 */

/**
 * @def   WCHAR_MIN
 * @brief Defines a minimum value for type @c wchar_t.
 */

/**
 * @def   WCHAR_MAX
 * @brief Defines a maximum value for type @c wchar_t.
 */

/**
 * @def   WINT_MIN
 * @brief Defines a minimum value for type @c wint_t.
 */

/**
 * @def   WINT_MAX
 * @brief Defines a maximum value for type @c wint_t.
 */

/**
 * @def   INT8_C(val)
 * @brief Defines a macros which converts an integer literal to a signed integer
 *        with a width of at least 8 bits.
 */

/**
 * @def   INT16_C(val)
 * @brief Defines a macros which converts an integer literal to a signed integer
 *        with a width of at least 16 bits.
 */

/**
 * @def   INT32_C(val)
 * @brief Defines a macros which converts an integer literal to a signed integer
 *        with a width of at least 32 bits.
 */

/**
 * @def   INT64_C(val)
 * @brief Defines a macros which converts an integer literal to a signed integer
 *        with a width of at least 64 bits.
 */

/**
 * @def   UINT8_C(val)
 * @brief Defines a macros which converts an integer literal to an unsigned
 *        integer with a width of at least 8 bits.
 */

/**
 * @def   UINT16_C(val)
 * @brief Defines a macros which converts an integer literal to an unsigned
 *        integer with a width of at least 16 bits.
 */

/**
 * @def   UINT32_C(val)
 * @brief Defines a macros which converts an integer literal to an unsigned
 *        integer with a width of at least 32 bits.
 */

/**
 * @def   UINT64_C(val)
 * @brief Defines a macros which converts an integer literal to an unsigned
 *        integer with a width of at least 64 bits.
 */

/**
 * @def   INTMAX_C(val)
 * @brief Defines a macros which converts an integer literal to a signed
 *        integer which has the greatest limits.
 */

/**
 * @def   UINTMAX_C(val)
 * @brief Defines a macros which converts an integer literal to an unsigned
 *        integer which has the greatest limits.
 */

/**
 * @typedef int8_t
 * @brief   Defines a signed integer type with a width of exactly 8 bits.
 */

/**
 * @typedef int16_t
 * @brief   Defines a signed integer type with a width of exactly 16 bits.
 */

/**
 * @typedef int32_t
 * @brief   Defines a signed integer type with a width of exactly 32 bits.
 */

/**
 * @typedef int64_t
 * @brief   Defines a signed integer type with a width of exactly 64 bits.
 */

/**
 * @typedef uint8_t
 * @brief   Defines an unsigned integer type with a width of exactly 8 bits.
 */

/**
 * @typedef uint16_t
 * @brief   Defines an unsigned integer type with a width of exactly 16 bits.
 */

/**
 * @typedef uint32_t
 * @brief   Defines an unsigned integer type with a width of exactly 32 bits.
 */

/**
 * @typedef uint64_t
 * @brief   Defines an unsigned integer type with a width of exactly 64 bits.
 */

/**
 * @typedef int_least8_t
 * @brief   Defines a signed integer type with a width of at least 8 bits.
 */

/**
 * @typedef int_least16_t
 * @brief   Defines a signed integer type with a width of at least  16 bits.
 */

/**
 * @typedef int_least32_t
 * @brief   Defines a signed integer type with a width of at least  32 bits.
 */

/**
 * @typedef int_least64_t
 * @brief   Defines a signed integer type with a width of at least  64 bits.
 */

/**
 * @typedef uint_least8_t
 * @brief   Defines an unsigned integer type with a width of at least  8 bits.
 */

/**
 * @typedef uint_least16_t
 * @brief   Defines an unsigned integer type with a width of at least  16 bits.
 */

/**
 * @typedef uint_least32_t
 * @brief   Defines an unsigned integer type with a width of at least  32 bits.
 */

/**
 * @typedef uint_least64_t
 * @brief   Defines an unsigned integer type with a width of at least  64 bits.
 */

/**
 * @typedef int_fast8_t
 * @brief   Defines a signed integer type being usually fastest with a width
 *          of at least 8 bits.
 */

/**
 * @typedef int_fast16_t
 * @brief   Defines a signed integer type being usually fastest with a width
 *          of at least  16 bits.
 */

/**
 * @typedef int_fast32_t
 * @brief   Defines a signed integer type being usually fastest with a width
 *          of at least  32 bits.
 */

/**
 * @typedef int_fast64_t
 * @brief   Defines a signed integer type being usually fastest with a width
 *          of at least  64 bits.
 */

/**
 * @typedef uint_fast8_t
 * @brief   Defines an unsigned integer type being usually fastest with a width
 *          of at least  8 bits.
 */

/**
 * @typedef uint_fast16_t
 * @brief   Defines an unsigned integer type being usually fastest with a width
 *          of at least  16 bits.
 */

/**
 * @typedef uint_fast32_t
 * @brief   Defines an unsigned integer type being usually fastest with a width
 *          of at least  32 bits.
 */

/**
 * @typedef uint_fast64_t
 * @brief   Defines an unsigned integer type being usually fastest with a width
 *          of at least  64 bits.
 */

/**
 * @typedef intptr_t
 * @brief   Defines a signed integer type which is guaranteed to hold the
 *          value of a pointer.
 */

/**
 * @typedef uintptr_t
 * @brief   Defines an unsigned integer type which is guaranteed to hold the
 *          value of a pointer.
 */

/**
 * @typedef intmax_t
 * @brief   Defines a signed integer type which has the greatest limits.
 */

/**
 * @typedef uintmax_t
 * @brief   Defines an unsigned integer type which has the greatest limits.
 */

/**
 * @}
 */

#endif // #ifndef MINUTILS_STDINT_VC_DOC_H_INCLUDED
