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
#include <QPoint>
#include <QSize>
#include <QPointF>
#include <mingeo/mingeo.h>

namespace vi { namespace cvt { namespace qt {

inline QPoint as_qpoint(MinPoint2i const& pt) {
  return { pt.x, pt.y };
}

// Note that QSomethingF's type depends on qreal's type,
// which can be either float or double depending on Qt
// compile-time configuration. For generality (and as
// the default is double), we treat conversions from
// qreal to float as narrowing and declare 'to' functions.
inline QPointF as_qpoint(MinPoint2d const& pt) {
  return { static_cast<qreal>(pt.x),
           static_cast<qreal>(pt.y) };
}

inline QSizeF as_qsize(MinSize2d const& sz) {
  return { static_cast<qreal>(sz.width),
           static_cast<qreal>(sz.height) };
}

inline MinSize2d to_minsize2d(QSizeF const& sz) {
  return { sz.width(), sz.height() };
}

inline MinPoint2d to_minpoint2d(QPointF const& p) {
  return { p.x(), p.y() };
}

}}} // ns vi::cvt::qt
