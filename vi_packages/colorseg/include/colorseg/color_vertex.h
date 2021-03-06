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


#pragma once

#include <remseg/vertex.h>
#include <vector>

#include <minbase/crossplat.h>

THIRDPARTY_INCLUDES_BEGIN
#include <Eigen/Dense>
#include <Eigen/Eigenvalues>
THIRDPARTY_INCLUDES_END

namespace vi {

using namespace remseg;

namespace colorseg {

class ColorVertex : public Vertex
{
public:
  long double **channelsSumOfSquares;

  struct HelperStats
  {
    Eigen::Vector3d mean;
    Eigen::Matrix3d covariance;
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigSolver;

    const Eigen::Vector3d& eigenvalues() const { return eigSolver.eigenvalues(); }
    const Eigen::Matrix3d& eigenvectors() const { return eigSolver.eigenvectors(); }
  };

  ColorVertex() :
    Vertex()
  , channelsSumOfSquares(0)
  , needToUpdate(false)
  {}

  ColorVertex(const ColorVertex* v);

  ~ColorVertex();

  void Initialize(int _channelsNum) override;
  void update(const uint8_t * pix) override;
  void absorb(Vertex *to_be_absorbed) override;

  const HelperStats & getHelperStats() const;

  Json::Value jsonLog() const override;

  static double getHomographyA() { return homographyA; }
  static double getHomographyK() { return homographyK; }
  static void setHomographyA(double d) { homographyA = d; }
  static void setHomographyK(double d) { homographyK = d; }

  static double getMaxModelDistance() { return maxModelDistance; }
  static void setMaxModelDistance(double d) { maxModelDistance = d; }

private:
  mutable HelperStats helperStats;

  mutable bool needToUpdate;

  static double homographyA;
  static double homographyK;
  static double maxModelDistance;

  void updateHelperStats() const;
};

}}	// ns vi::colorseg
