/*
Copyright (c) 2011-2013, Smart Engines Limited. All rights reserved.

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

/**
 * @file   imgguard.hpp
 * @brief  Definition of utility classes.
 */

#pragma once
#ifndef MINIMGAPI_IMGGUARD_HPP_INCLUDED
#define MINIMGAPI_IMGGUARD_HPP_INCLUDED

#include <minimgapi/minimgapi.h>

/**
 * @brief      Specifies a class which is used to avoid "free image" problems.
 * @deprecated This class is deprecated now, it is better to use @c MinImgGuard.
 * @ingroup    MinImgAPI_Utility
 */
class imgGuard {
public:
  /// Constructor. Setups the image.
  imgGuard(const MinImg &image) : image(image) {
  }
  virtual ~imgGuard() { ///< Destructor. Frees the image.
    FreeMinImage(&image);
  }
private:
  imgGuard(const imgGuard &); ///< Forbidden copy constructor.
  imgGuard &operator =(const imgGuard &); ///< Forbidden assignment operator.
  MinImg image; ///< The image to be freed while a function exit.
};

/**
 * @brief   Specifies a class which is used to avoid "free image" problems.
 *          Instead of the @c imgGuard class this one stores a reference
 *          to the image.
 * @ingroup MinImgAPI_Utility
 */
class MinImgGuard {
public:
  /// Constructor. Setups the image.
  MinImgGuard(MinImg &image) : image(image) {
  }
  virtual ~MinImgGuard() { ///< Destructor. Frees the image.
    FreeMinImage(&image);
  }
private:
  MinImg &image; ///< The reference to the image to be freed.
};

/**
 * @brief   Declares a new @MinImg called <name> and the @c MinImgGuard
 *          called <name>_MinImgGuard.
 * @ingroup MinImgAPI_Utility
 */
#define DECLARE_GUARDED_MINIMG(name) \
  MinImg name = {}; MinImgGuard name##_MinImgGuard(name)

#endif // #ifndef MINIMGAPI_IMGGUARD_HPP_INCLUDED
