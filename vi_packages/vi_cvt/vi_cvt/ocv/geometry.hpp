/*
Copyright (c) 2012-2018, Visillect Service LLC. All rights reserved.

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
#include <mingeo/mingeo.h>
#include <minbase/crossplat.h>
THIRDPARTY_INCLUDES_BEGIN
#include <opencv2/core/core.hpp>
THIRDPARTY_INCLUDES_END


namespace vi { namespace cvt { namespace ocv {

template <class T>
inline se::pnt2<T> as_minpoint(cv::Point_<T> const& x) {
  return { x.x, x.y };
}

template <class T>
inline cv::Point_<T> as_cvpoint(se::pnt2<T> const& x) {
  return { x.x, x.y };
}

template <class T>
inline se::pnt3<T> as_minpoint(cv::Point3_<T> const& x) {
  return { x.x, x.y, x.z };
}

template <class T>
inline cv::Point3_<T> as_cvpoint(se::pnt3<T> const& x) {
  return { x.x, x.y, x.z };
}

template <class T>
inline cv::Point to_cvpoint2i(se::pnt2<T> const& x) {
  return { static_cast<int>(x.x), static_cast<int>(x.y) };
}

template <class T>
inline cv::Point round_to_cvpoint2i(se::pnt2<T> const& x) {
  return { static_cast<int>(x.x + 0.5), static_cast<int>(x.y + 0.5) };
}


}}} // ns vi::cvt::ocv
