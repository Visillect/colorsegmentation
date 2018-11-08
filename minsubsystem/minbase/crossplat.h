/*

Copyright (c) 2016, Smart Engines Limited. All rights reserved.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are
permitted provided that the following conditions are met:

   1. Redistributions of source code must retain the above copyright notice, this list of
      conditions and the following disclaimer.

   2. Redistributions in binary form must reproduce the above copyright notice, this list
      of conditions and the following disclaimer in the documentation and/or other materials
      provided with the distribution.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those of the
authors and should not be interpreted as representing official policies, either expressed
or implied, of copyright holders.

*/

/**
 * @file   crossplat.h
 * @brief  Different cross-platform declarations.
*/

#pragma once
#ifndef MINSBASE_CROSSPLAT_H_INCLUDED
#define MINSBASE_CROSSPLAT_H_INCLUDED

#include <minbase/macro_helpers.h>

/**
 * @defgroup MinBase_Crossplat Cross-platform Declarations
 * @brief    The module contains macros and functions which provide
 *           cross-platform compiling of the library.
 */

/**
 * @def     MUSTINLINE
 * @brief   Specifiers a cross-platform instruct the compiler to insert a copy
 *          of the function body into each place the function is called
 * @ingroup MinBase_Crossplat
 */
#ifndef NO_INLINE
# ifndef MUSTINLINE
#   ifdef _MSC_VER
#     define MUSTINLINE inline __forceinline
#   else
#     define MUSTINLINE inline __attribute__((always_inline))
#   endif
# endif
#else
# ifndef MUSTINLINE
#   define MUSTINLINE inline
# endif
#endif // NO_INLINE

/**
 * @def     STATIC_SPECIAL
 * @brief   On MSVC a specialization of template function does not inherit
 *          storage class. So one has to set storage class manually which is
 *          forbidden by C++ Standard and is not compilable on g++.
 * @ingroup MinBase_Crossplat
 */
#ifndef STATIC_SPECIAL
# if !defined(__clang__) && (defined(_MSC_VER) || \
      (__GNUC__ < 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ < 3)))
#   define STATIC_SPECIAL static
# else
#   define STATIC_SPECIAL
# endif
#endif // STATIC_SPECIAL

/**
 * @def     alignedmalloc(size, alignment)
 * @brief   Specifies cross-platform aligned allocation.
 * @ingroup MinBase_Crossplat
 */

/**
 * @def     alignedfree(ptr)
 * @brief   Specifies cross-platform aligned deallocations.
 * @ingroup MinBase_Crossplat
 */
#if defined(__MINGW32__)
# include <malloc.h>
# define alignedmalloc(size, alignment) __mingw_aligned_malloc(size, alignment)
# define alignedfree(ptr)               __mingw_aligned_free(ptr)
#elif defined(USE_SSE_SIMD)
# include <emmintrin.h>
# define alignedmalloc(size, alignment) _mm_malloc(size, alignment)
# define alignedfree(ptr)               _mm_free(ptr)
#else
# include <stdlib.h>
# define alignedmalloc(size, alignment) malloc(size)
# define alignedfree(ptr)               free(ptr)
#endif

#if defined(_MSC_VER)
# define MIN_ALIGNED(N) __declspec(align(N))
#else
# define MIN_ALIGNED(N) __attribute__((aligned(N)))
#endif

#if defined(__GNUC__)
# define MIN_HAS_BUILTIN_PREFETCH
#elif defined(__clang__)
# if defined(__has_builtin) && __has_builtin(__builtin_prefetch)
#   define MIN_HAS_BUILTIN_PREFETCH
# endif
#endif

#ifdef MIN_HAS_BUILTIN_PREFETCH
# define MIN_PREFETCH(ADDR, MODE) __builtin_prefetch(ADDR, MODE)
#else
# define MIN_PREFETCH(ADDR, MODE)
#endif

#if (defined _MSC_VER) && (_MSC_VER < 1900)
# define snprintf _snprintf
#endif

#ifdef _MSC_VER
# define MIN_FUNCNAME_MACRO __FUNCSIG__
#else // Assume GCC-compatible compiler here.
# define MIN_FUNCNAME_MACRO __PRETTY_FUNCTION__
#endif

/**
 * @def     IS_BY_DEFAULT(a)
 * @brief   Specifies a default value for a parameter of a function.
 * @ingroup MinBase_Crossplat
 */
#ifdef __cplusplus
# define IS_BY_DEFAULT(a) = a
#else
# define IS_BY_DEFAULT(a)
#endif

#if defined(_MSC_VER)
# define THIRDPARTY_INCLUDES_BEGIN IMPLANT_PRAGMA(warning(push, 0))
# define THIRDPARTY_INCLUDES_END IMPLANT_PRAGMA(warning(pop))
#elif defined(__clang__)
# define THIRDPARTY_INCLUDES_BEGIN                  \
  IMPLANT_PRAGMA(clang diagnostic push)             \
  IMPLANT_PRAGMA(clang diagnostic ignored "-Wall")  \
  IMPLANT_PRAGMA(clang diagnostic ignored "-Wextra")
# define THIRDPARTY_INCLUDES_END IMPLANT_PRAGMA(clang diagnostic pop)
#elif defined(__GNUC__)
# include <minbase/src/gcc_warnings.h>
# define THIRDPARTY_INCLUDES_BEGIN    \
  IMPLANT_PRAGMA(GCC diagnostic push) \
  DISABLE_GCC_WARNINGS
# define THIRDPARTY_INCLUDES_END IMPLANT_PRAGMA(GCC diagnostic pop)
#endif

#ifdef __cplusplus
# if __cplusplus >= 201100L
# include <type_traits>
namespace minstd {

template<class T, T v>
using integral_constant = std::integral_constant<T, v>;

using true_type = std::true_type;
using false_type = std::false_type;

template<class T, class U> using is_same = std::is_same<T, U>;

template<bool cond, class T = void> using enable_if = std::enable_if<cond, T>;

template<bool B, class T, class F>
using conditional = std::conditional<B, T, F>;

} // namespace minstd
# else // __cplusplus >= 201100L
namespace minstd {

template<class T, T v>
struct integral_constant {
  static const T value = v;
  typedef T value_type;
  typedef integral_constant type;
  operator value_type() const { return value; }
};

typedef integral_constant<bool, true> true_type;
typedef integral_constant<bool, false> false_type;

template<class T, class U>
struct is_same : minstd::false_type {};
template<class T>
struct is_same<T, T> : minstd::true_type {};

template<bool cond, class T = void>
struct enable_if {};
template<class T>
struct enable_if<true, T> { typedef T type; };

template<bool B, class T, class F>
struct conditional { typedef T type; };
template<class T, class F>
struct conditional<false, T, F> { typedef F type; };

} // namespace minstd
# endif // __cplusplus >= 201100L #else
#endif // __cplusplus

#endif // #ifndef MINSBASE_CROSSPLAT_H_INCLUDED
