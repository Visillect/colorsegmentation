/*

Copyright (c) 2011, Smart Engines Limited. All rights reserved.

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
 * @file   smartptr.h
 * @brief  Different cross-platform declarations.
 */

#pragma once
#ifndef MINUTILS_SMARTPTR_H_INCLUDED
#define MINUTILS_SMARTPTR_H_INCLUDED

#include <cstdlib>
#include <minbase/crossplat.h>
#include <minbase/mintyp.h>

/**
 * @defgroup MinUtils_SmartPtr Smart Pointers
 * @brief    The module specifies classes which will take care about freeing
 *           memory of  allocated array when the scope of such array ends.
 */

/**
 * @brief   Defines a template class which will take care about freeing memory
 *          of allocated array when the scope of such array ends.
 * @details There are at least two types of arrays: allocated by new[] call (see
 *          @c scoped_cpp_array class) and allocated by malloc-like call (see
 *          @c scoped_c_array class). The code of classes for both cases is the
 *          same but one line in destructor. So the macro is defined for that
 *          purpose.
 * @ingroup MinUtils_SmartPtr
 */
#define DEFINE_SCOPED_OBJECT(name, freeing_proc) \
template<typename T> class name \
{ \
public: \
  explicit name(T *p = 0): p(p) {} \
  ~name() \
  { \
    freeing_proc(p); \
  } \
  operator T *() const \
  { \
    return p; \
  } \
private: \
  name(const name &); \
  void operator =(const name &); \
  T *p; \
};

/**
 * @class   scoped_c_array
 * @brief   Specifies a class which will take care about freeing memory with
 *          @c free() function.
 * @details The class takes care about freeing memory with @c free() function.
 * @ingroup MinUtils_SmartPtr
 */

DEFINE_SCOPED_OBJECT(scoped_c_array, free)

/**
 * @class   scoped_cpp_array
 * @brief   Specifies a class which will take care about freeing memory with
 *          @c delete[] function.
 * @details The class takes care about freeing memory with @c delete[] function.
 * @ingroup MinUtils_SmartPtr
 */

DEFINE_SCOPED_OBJECT(scoped_cpp_array, delete[])

template<typename TData, typename TShift> static MUSTINLINE TData *ShiftPtr
(
  TData *ptr,
  TShift shift
)
{
  return const_cast<TData *>(
           reinterpret_cast<const TData *>(
             reinterpret_cast<const uint8_t *>(ptr) + shift));
}


// Helper to create scoped handles.
#define DEFINE_SCOPED_HANDLE(name, type, freeing_proc)    \
  class name {                                            \
   public:                                                \
    name() : handle(NULL) { }                             \
    ~name() { freeing_proc(&handle); }                    \
    operator type () const  { return handle; }            \
    type * get() { return &handle; }                      \
    void free() { freeing_proc(&handle); handle = NULL; } \
    void reset(type new_handle) {                         \
      freeing_proc(&handle);                              \
      handle = new_handle;                                \
    }                                                     \
    type release() {                                      \
      type res = handle;                                  \
      handle = NULL;                                      \
      return res;                                         \
    }                                                     \
   private:                                               \
    name(const name &);                                   \
    void operator =(const name &);                        \
    type handle;                                          \
  };

#endif // #ifndef MINUTILS_SMARTPTR_H_INCLUDED
