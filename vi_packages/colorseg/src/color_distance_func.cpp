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

#include <colorseg/colorspace_transform.h>
#include <remseg/distance_func.h>

#include <cassert>
#include <cmath>
#include <algorithm>
#include <iostream>

THIRDPARTY_INCLUDES_BEGIN
#include <Eigen/Geometry>
THIRDPARTY_INCLUDES_END

namespace vi { namespace colorseg {

Eigen::Vector3d fixDir(Eigen::Vector3d vec) {
  if (vec(0) < 0) return -vec;
  return vec;
}

double variance(ColorVertex const * v)
{
  if (v->area == 1)
    return 0;

  double res = 0;
  auto const & hs = v->getHelperStats();
  for (int i = 0; i < 3; ++i)
    res += hs.eigenvalues()[i];
  return res;
}

double dist_point_to_point(Eigen::Vector3d const& p1,
                           Eigen::Vector3d const& p2)
{
  return (p1 - p2).norm();
}

double dist_point_to_line(Eigen::Vector3d const& p,
                          Eigen::Vector3d const& v,
                          Eigen::Vector3d const& mean)
{
  Eigen::Vector3d r = mean - p;
  Eigen::Vector3d res = r - v.dot(r) * v;
  return res.norm();
}

double dist_point_to_plane(Eigen::Vector3d const& p,
                           Eigen::Vector3d const& norm,
                           Eigen::Vector3d const& mean)
{
  double res = std::abs(norm.dot(mean - p)) / norm.norm();
  return res;
}

double dist_point_to_segment(
      const Eigen::Vector3d& p,
      const Eigen::Vector3d& a,
      const Eigen::Vector3d& b
) {
  if ((b - a).dot(p - a) < 0)
    return (p - a).norm();
  if ((a - b).dot(p - b) < 0)
    return (p - b).norm();
  return dist_point_to_line(p, (b - a).normalized(), a);
}

double proj_err_to_line(Eigen::Matrix3d const & covariance,
                        Eigen::Vector3d const & cm_axe,
                        Eigen::Vector3d const & line)
{
  Eigen::Quaterniond const q = Eigen::Quaterniond().setFromTwoVectors(cm_axe, line);
  Eigen::Matrix3d const R = q.toRotationMatrix();

  Eigen::Matrix3d const proj_covariance = R * covariance * R.transpose();

  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> const
      es(proj_covariance, Eigen::EigenvaluesOnly);
  return es.eigenvalues()[0] + es.eigenvalues()[1];
}

double proj_err_to_plane(Eigen::Matrix3d const & covariance,
                         Eigen::Vector3d const & cm_axe,
                         Eigen::Vector3d const & line)
{
  Eigen::Quaterniond const q = Eigen::Quaterniond().setFromTwoVectors(cm_axe, line);
  Eigen::Matrix3d const R = q.toRotationMatrix();

  Eigen::Matrix3d const proj_covariance = R * covariance * R.transpose();

  Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> const
      es(proj_covariance, Eigen::EigenvaluesOnly);
  return es.eigenvalues()[0];
}

ColorVertex merge(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex v1_tmp(v1);
  ColorVertex v2_tmp(v2);

  v1_tmp.absorb(&v2_tmp);
  return v1_tmp;
}

EdgeValue error_zero(const ColorVertex*) {
  return 0;
}

EdgeValue criteria_r0_old(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex::HelperStats const & hs1 = v1->getHelperStats();
  ColorVertex::HelperStats const & hs2 = v2->getHelperStats();
  Eigen::Vector3d const & mean1 = hs1.mean;
  Eigen::Vector3d const & mean2 = hs2.mean;

  double var1 = variance(v1) + ME;
  double var2 = variance(v2) + ME;

  Eigen::Vector3d mean = (mean1 * v1->area + mean2 * v2->area) / (v1->area + v2->area);
  double btw = (mean1 - mean).squaredNorm() * v1->area +  (mean2 - mean).squaredNorm() * v2->area;

  return std::sqrt((var1 * v1->area + var2 * v2->area + btw) / (v1->area + v2->area));
}

EdgeValue criteria_r1_old(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex v(v1);
  ColorVertex tmp(v2);
  v.absorb(&tmp);

  ColorVertex::HelperStats const & hs1 = v1->getHelperStats();
  ColorVertex::HelperStats const & hs2 = v2->getHelperStats();

  ColorVertex::HelperStats const & hs = v.getHelperStats();
  Eigen::Vector3d const & direction = fixDir(hs.eigenvectors().col(2));

  double err1 = ME, err2 = ME;
  const double EPS = 1.e-5;

  if (variance(v1) < EPS)
    err1 += std::pow(dist_point_to_line(hs1.mean, direction, hs.mean), 2);
  else
    err1 += proj_err_to_line(hs1.covariance, fixDir(hs1.eigenvectors().col(2)), direction);

 if (variance(v2) < EPS)
    err2 += std::pow(dist_point_to_line(hs2.mean, direction, hs.mean), 2);
  else
    err2 += proj_err_to_line(hs2.covariance, fixDir(hs2.eigenvectors().col(2)), direction);

  return std::sqrt((err1 * v1->area + err2 * v2->area) / (v1->area + v2->area));
}

EdgeValue criteria_r2_old(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex v(v1);
  ColorVertex tmp(v2);
  v.absorb(&tmp);

  ColorVertex::HelperStats const & hs1 = v1->getHelperStats();
  ColorVertex::HelperStats const & hs2 = v2->getHelperStats();

  ColorVertex::HelperStats const & hs = v.getHelperStats();
  Eigen::Vector3d const & norm = fixDir(hs.eigenvectors().col(0));

  if (!isLTCluster(v1, v2))
    return std::numeric_limits<double>::infinity();

  double err1 = ME, err2 = ME;
  const double EPS = 1.e-5;

  if (variance(v1) < EPS)
    err1 += std::pow(dist_point_to_plane(hs1.mean, norm, hs.mean), 2);
  else
    err1 += proj_err_to_plane(hs1.covariance, fixDir(hs1.eigenvectors().col(0)), norm);

 if (variance(v2) < EPS)
    err2 += std::pow(dist_point_to_plane(hs2.mean, norm, hs.mean), 2);
  else
    err2 += proj_err_to_plane(hs2.covariance, fixDir(hs2.eigenvectors().col(0)), norm);

  return std::sqrt((err1 * v1->area + err2 * v2->area) / (v1->area + v2->area));
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


EdgeValue criteria_r2(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex v(v1);
  ColorVertex tmp(v2);
  v.absorb(&tmp);

  return std::sqrt(error_r2(&v) - error_r2(v1) - error_r2(v2));
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

EdgeValue criteria_r2_new(const ColorVertex *v1, const ColorVertex *v2)
{
  if (!isLTCluster(v1, v2))
    return std::numeric_limits<double>::infinity();

  ColorVertex v(v1);
  ColorVertex tmp(v2);
  v.absorb(&tmp);
  return std::sqrt(error_r2(&v) - error_r2(v1) - error_r2(v2));
}


EdgeValue criteria_r2_supernew(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex::HelperStats const & hs1 = v1->getHelperStats();
  ColorVertex::HelperStats const & hs2 = v2->getHelperStats();

  const double GLARE_THRES = ColorVertex::getGlareThresh();

  bool v1_glare = Konovalenko2RGB(hs1.mean).mean() > GLARE_THRES;
  bool v2_glare = Konovalenko2RGB(hs2.mean).mean() > GLARE_THRES;

//  if (v1->area == 185 || v2->area == 185) {
//    std::cout << v1->area << ", " << Konovalenko2RGB(hs1.mean).mean() << "    " << v2->area << ", " << Konovalenko2RGB(hs2.mean).mean() << std::endl;
//     std::cout << Konovalenko2RGB(hs1.mean) << "  , " << Konovalenko2RGB(hs2.mean)<< std::endl;
//     std::cout << std::endl;
//  }

  if (v1_glare == v2_glare)
    return criteria_r2_new(v1, v2);

  //  if (v1->area == 92 || v2->area == 92)

  ColorVertex const * glare = v1_glare ? v1 : v2;
  ColorVertex const * matte = v1_glare ? v2 : v1;
  //      std::cout << "glare" << std::endl;

  // check that all neighbours already passed through possible merges
  //  if (glare->area == 92)
  //    std::cout   << glare->size() << " neighbours"<< std::endl;
  for (ConstJoint it1 = glare->begin(); it1 != glare->end(); it1++)
  {
   // if (glare->area == 92)

   //   std::cout   << it1->vertex->size() << " neighbours of neighbour, " << it1->vertex->area << std::endl;

    for (ConstJoint it2 = it1->vertex->begin(); it2 != it1->vertex->end(); it2++)
    {
   // if (glare->area == 92)
    //  std::cout   << it2->vertex->area << " neighbours of neighbour area"<< std::endl;

    //  if (it1->vertex->area == 185 || it2->vertex->area == 185)
     //   std::cout << "ururu" << std::endl;

      if (glare == dynamic_cast<ColorVertex*>(it2->vertex))
      {
     //   std::cout << "yep" << std::endl;
        continue;
      }
      if (isLTCluster(dynamic_cast<ColorVertex*>(it1->vertex),
                      dynamic_cast<ColorVertex*>(it2->vertex)))
      {
        //if (it1->vertex->area == 185 || it2->vertex->area == 185)
        //  std::cout << criteria_r2(v1, v2) << std::endl;
        return std::numeric_limits<double>::infinity();
      }
    }
  }

  // check if there possible merge of non-bordering with each other glare neighbours
  if (glare->size() == 1)
      return criteria_r2(v1, v2);

  for (ConstJoint it1 = glare->begin(); it1 != glare->end(); it1++)
  {
    ColorVertex const * glare_neighbour = dynamic_cast<ColorVertex*>(it1->vertex);
    // std::cout << glare_neighbour << " " << matte << std::endl;
    if (glare_neighbour == matte)
      continue;
    // std::cout << "r ";
    if (isLTCluster(glare_neighbour, matte))
    {
      // std::cout << std::endl;
      return criteria_r2(v1, v2);
    }
  }
    //   std::cout << std::endl;

  return std::numeric_limits<double>::infinity();
}


EdgeValue offscale_fix(const ColorVertex *v1, const ColorVertex *v2)
{
  ColorVertex::HelperStats const & hs1 = v1->getHelperStats();
  ColorVertex::HelperStats const & hs2 = v2->getHelperStats();

  const double GLARE_THRES = ColorVertex::getGlareThresh();

  bool v1_glare = Konovalenko2RGB(hs1.mean).mean() > GLARE_THRES;
  bool v2_glare = Konovalenko2RGB(hs2.mean).mean() > GLARE_THRES;

  if (v1_glare == v2_glare)
    return std::numeric_limits<double>::infinity();

  ColorVertex const * glare = v1_glare ? v1 : v2;
  ColorVertex const * matte = v1_glare ? v2 : v1;

  // check if there possible merge of non-bordering with each other glare neighbours
  if (glare->size() == 1)
      return criteria_r2(v1, v2);

  for (ConstJoint it1 = glare->begin(); it1 != glare->end(); it1++)
  {
    ColorVertex const * glare_neighbour = dynamic_cast<ColorVertex*>(it1->vertex);
    if (glare_neighbour == matte)
      continue;
    if (isLTCluster(glare_neighbour, matte))
      return 0;// criteria_r2(v1, v2);
  }

  return std::numeric_limits<double>::infinity();
}


}}	// ns vi::colorseg
