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

namespace draw_utils {

inline cv::Mat hstack(cv::Mat const& m1, cv::Mat const& m2)
{
  if (m1.empty())
    return m2.clone();
  if (m2.empty())
    return m1.clone();

  cv::Mat r(std::max(m1.rows, m2.rows), m1.cols + m2.cols, m1.type());
  r = cv::Scalar(0);
  cv::Mat d1 = r(cv::Rect(cv::Point(0, 0), m1.size()));
  cv::Mat d2 = r(cv::Rect(cv::Point(m1.cols, 0), m2.size()));
  m1.copyTo(d1);
  m2.copyTo(d2);
  return r;
}

inline cv::Mat vstack(cv::Mat const& m1, cv::Mat const& m2)
{
  if (m1.empty())
    return m2.clone();
  if (m2.empty())
    return m1.clone();

  cv::Mat r(m1.rows + m2.rows, std::max(m1.cols, m2.cols), m1.type());
  r = cv::Scalar(0);
  cv::Mat d1 = r(cv::Rect(cv::Point(0, 0), m1.size()));
  cv::Mat d2 = r(cv::Rect(cv::Point(0, m1.rows), m2.size()));
  m1.copyTo(d1);
  m2.copyTo(d2);
  return r;
}

}
