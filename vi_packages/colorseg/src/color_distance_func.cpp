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


#include <colorseg/color_distance_func.h>

#include <colorseg/colorspace_homography.hpp>
#include <remseg/distance_func.h>

#include <cassert>
#include <cmath>
#include <algorithm>

THIRDPARTY_INCLUDES_BEGIN
#include <Eigen/Geometry>
THIRDPARTY_INCLUDES_END

namespace vi { namespace colorseg {

double dist_point_to_line(Eigen::Vector3d const& p,
                          Eigen::Vector3d const& v,
                          Eigen::Vector3d const& mean)
{
  Eigen::Vector3d r = mean - p;
  Eigen::Vector3d res = r - v.dot(r) * v;
  return res.norm();
}

double dist_point_to_segment(const Eigen::Vector3d& p,
                             const Eigen::Vector3d& a,
                             const Eigen::Vector3d& b)
{
  if ((b - a).dot(p - a) < 0)
    return (p - a).norm();
  if ((a - b).dot(p - b) < 0)
    return (p - b).norm();
  return dist_point_to_line(p, (b - a).normalized(), a);
}

EdgeValue criteria_r0(const ColorVertex *v1, const ColorVertex *v2)
{
  return std::sqrt(student_distance(v1, v2));
}

EdgeValue error_r1(const ColorVertex *v) {
  ColorVertex::HelperStats const & hs = v->getHelperStats();
  double err = hs.eigenvalues()[0] + hs.eigenvalues()[1];
  return err * v->area;
}

EdgeValue criteria_r1(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex v(v1);
  ColorVertex tmp(v2);
  v.absorb(&tmp);

  return std::sqrt(error_r1(&v) - error_r1(v1) - error_r1(v2)) ;
}

EdgeValue error_r2(const ColorVertex *v) {
  ColorVertex::HelperStats const & hs = v->getHelperStats();
  double err = hs.eigenvalues()[0];
  return err * v->area;
}

bool isLTCluster(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex::HelperStats const & hs1 = v1->getHelperStats();
  ColorVertex::HelperStats const & hs2 = v2->getHelperStats();

  Eigen::Vector3d vec1 = hs1.eigenvectors().col(2);
  Eigen::Vector3d vec2 = hs2.eigenvectors().col(2);
  double semiAxis1 = std::sqrt(hs1.eigenvalues()(2));
  double semiAxis2 = std::sqrt(hs2.eigenvalues()(2));
  Eigen::Vector3d a1 = hs1.mean - vec1 * semiAxis1;
  Eigen::Vector3d a2 = hs2.mean - vec2 * semiAxis2;
  Eigen::Vector3d b1 = hs1.mean + vec1 * semiAxis1;
  Eigen::Vector3d b2 = hs2.mean + vec2 * semiAxis2;

  double dists[4] = {
    dist_point_to_segment(a1, a2, b2),
    dist_point_to_segment(b1, a2, b2),
    dist_point_to_segment(a2, a1, b1),
    dist_point_to_segment(b2, a1, b1)
  };

  double minDist = *std::min_element(dists, dists + 4);
  return minDist < ColorVertex::getMaxModelDistance();
}

EdgeValue criteria_r2(const ColorVertex *v1, const ColorVertex *v2)
{
  if (!isLTCluster(v1, v2))
    return std::numeric_limits<double>::infinity();

  ColorVertex v(v1);
  ColorVertex tmp(v2);
  v.absorb(&tmp);
  return std::sqrt(error_r2(&v) - error_r2(v1) - error_r2(v2));
}

}}	// ns vi::colorseg
