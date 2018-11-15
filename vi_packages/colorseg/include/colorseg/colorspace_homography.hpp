/*
Copyright (c) 2012-2018, Visillect Service LLC. All rights reserved.
Developed for Kharkevich Institute for Information Transmission Problems of the
              Russian Academy of Sciences (IITP RAS).

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


#include <cassert>
#include <cmath>
#include <algorithm>

#include <colorseg/color_vertex.h>

THIRDPARTY_INCLUDES_BEGIN
#include <Eigen/Dense>
THIRDPARTY_INCLUDES_END

namespace vi { namespace colorseg {

inline Eigen::Vector3d homographyInv(Eigen::Vector3d const & src, float a, float k)
{
  float EPS = 1.e-5;
  assert(k > -EPS);
  assert(a > -EPS);
  assert(a < 1 + EPS);

  Eigen::Vector4f src_vec(src[0] / 255, src[1] / 255, src[2] / 255, 1);

  Eigen::Matrix<float, 4, 4> H = Eigen::MatrixXf::Identity(4, 4);
  for (int i = 0; i < 3; ++i)
    H(3, i) = k;

  Eigen::Matrix<float, 4, 4> S = Eigen::MatrixXf::Identity(4, 4);
  for (int i = 0; i < 3; ++i)
    S(i, i) = k + 1;

  Eigen::Matrix<float, 4, 4> A = Eigen::MatrixXf::Identity(4, 4);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
        if (i != j)
            A(i, j) = a;

  Eigen::Matrix<float, 4, 4> P = A * S * H;
  Eigen::Vector4f dst_vec = P.inverse() * src_vec;

  if (std::abs(dst_vec[3]) >= EPS)
  {
    for (int i = 0; i < 3; i++)
      dst_vec[i] = (dst_vec[i] / dst_vec[3]) * 255;
  }

  return Eigen::Vector3d(dst_vec[0], dst_vec[1], dst_vec[2]);
}

inline void homography(float * dst, float const * src, float a, float k)
{
  float EPS = 1.e-5;
  assert(k > -EPS);
  assert(a > -EPS);
  assert(a < 1 + EPS);

  Eigen::Vector4f src_vec(src[0] / 255, src[1] / 255, src[2] / 255, 1);

  Eigen::Matrix<float, 4, 4> H = Eigen::MatrixXf::Identity(4, 4);
  for (int i = 0; i < 3; ++i)
    H(3, i) = k;

  Eigen::Matrix<float, 4, 4> S = Eigen::MatrixXf::Identity(4, 4);
  for (int i = 0; i < 3; ++i)
    S(i, i) = k + 1;

  Eigen::Matrix<float, 4, 4> A = Eigen::MatrixXf::Identity(4, 4);
  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
        if (i != j)
            A(i, j) = a;

  Eigen::Matrix<float, 4, 4> P = A * S * H;
  Eigen::Vector4f dst_vec = P * src_vec;

  if (std::abs(dst_vec[3]) >= EPS)
  {
    for (int i = 0; i < 3; i++)
      dst_vec[i] = (dst_vec[i] / dst_vec[3]) * 255;
  }

  dst[0] = dst_vec[0];
  dst[1] = dst_vec[1];
  dst[2] = dst_vec[2];
}

}} // ns vi::colorseg
