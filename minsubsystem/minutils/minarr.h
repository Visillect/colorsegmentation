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
 * @file   minarr.h
 * @brief  Definition of a multi-dimensional dense multi-channel array.
 */

#pragma once
#ifndef MINUTILS_MINARR_H_INCLUDED
#define MINUTILS_MINARR_H_INCLUDED

#include <minbase/mintyp.h>

/**
 * @defgroup MinUtils_MinArr Multi-channel Array Representation
 * @brief    The module specifies a multi-dimensional dense multi-channel
 *           array representation.
 */

/**
 * @brief   A multi-dimensional dense multi-channel array representation.
 * @details The struct @c MinArr represents a multi-dimensional dense numerical
 *          single-channel or multi-channel array. The data layout of the array
 *          is defined by the field @c MinArr::pStrides. Let us @f$ M @f$ is an
 *          instance of @c MinArr. So the address of element
 *          @f$ (i_0,...,i_{M.dim - 1}) @f$, where @f$ 0 \le i_k \le M.pSizes[k] @f$
 *          is computed as:
 *            @f[ M.pStart+M.pStrides[0]*i_0+...+M.pStrides[M.dim-1]*i_{M.dim - 1} @f]
 *          For example, in the case of 2-dimensional array the above formula is
 *          reduced to:
 *            @f[ M.pStart+M.pStrides[0]*i+M.pStrides[1]*j @f]
 *          Note that @f$ M.pStrides[i] \ge M.pStrides[i+1] @f$ that is, 2-dimensional
 *          matrices are stored row-by-row, 3-dimensional matrices are stored plane-by
 *          plane etc. @f$ M.pStrides[M.dim-1] @f$ is minimal and always equal to the
 *          element size @f$ M.channelDepth @f$.
 * @ingroup MinUtils_MinArr
 */
typedef struct
{
  int32_t  dim;          ///< The number of array dimensions. It must be
                         ///  positive.
  int32_t *pSizes;       ///< Array size of each dimension. It must be
                         ///  nonnegative.
  int32_t *pStrides;     ///< Full row length (in bytes) for each dimension. It
                         ///  must be positive.
  int32_t  channelDepth; ///< Size of element in bytes. It must be positive.
  MinFmt   format;       ///< Element format (supported formats are represented
                         ///  by MinFmt).
  uint8_t *pStart;       ///< The pointer to the (0, 0, ... 0) element.
} MinArr;

#endif // #ifndef MINUTILS_MINARR_H_INCLUDED
