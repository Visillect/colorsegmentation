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


#include <colorseg/color_vertex.h>
#include <colorseg/colorspace_homography.hpp>

#include <cmath>

namespace vi { namespace colorseg {

float ColorVertex::homographyA = 0.0;
float ColorVertex::homographyK = 3.0;
float ColorVertex::LTDistance = 20;

ColorVertex::ColorVertex(const ColorVertex* v)
: Vertex(v)
{
  channelsSumOfSquares = new double* [channelsNum];
  for (int i = 0; i < channelsNum; i++)
  {
    channelsSumOfSquares[i] = new double[channelsNum];
    for (int j = 0; j < channelsNum; j++)
      channelsSumOfSquares[i][j] = v->channelsSumOfSquares[i][j];
  }
  helperStats = v->helperStats;
  needToUpdate = v->needToUpdate;
}

ColorVertex::~ColorVertex()
{
	if (channelsSumOfSquares != NULL)
	{
		for (int i = 0; i < channelsNum; i++)
			delete[] channelsSumOfSquares[i];
		delete [] channelsSumOfSquares;
	}
}

void ColorVertex::Initialize(int _channelsNum)
{
  Vertex::Initialize(_channelsNum);

	channelsSumOfSquares = new double* [channelsNum];
	for (int i = 0; i < channelsNum; i++)
	{
		channelsSumOfSquares[i] = new double[channelsNum];
		for (int j = 0; j < channelsNum; j++)
			channelsSumOfSquares[i][j] = 0;
	}
}

void ColorVertex::update(const float_t * pix)
{
  float pix_proj[3] = {0, 0, 0};
  homography(pix_proj, pix);
  Vertex::update(pix_proj);

	Eigen::Vector3f pix_vec(pix_proj[0], pix_proj[1], pix_proj[2]);
	Eigen::Matrix3f pix_sq_mat = pix_vec * pix_vec.transpose();
	for (int i = 0; i < channelsNum; ++i)
		for (int j = 0; j < channelsNum; ++j)
			channelsSumOfSquares[i][j] += pix_sq_mat(i, j);

  needToUpdate = true;
}

void ColorVertex::absorb(Vertex *v)
{
  Vertex::absorb(v);

  ColorVertex* cv = dynamic_cast<ColorVertex *>(v);
	for (int i = 0; i < channelsNum; i++)
		for (int j = 0; j < channelsNum; j++)
			channelsSumOfSquares[i][j] += cv->channelsSumOfSquares[i][j];

  needToUpdate = true;
}

const ColorVertex::HelperStats & ColorVertex::getHelperStats() const
{
  if (needToUpdate)
    updateHelperStats();
  return helperStats;
}

void ColorVertex::updateHelperStats() const
{
  HelperStats & hs = helperStats;

  // calculate mean and covariance matrix
  {
    Eigen::Vector3d sum(channelsSum[0], channelsSum[1], channelsSum[2]);
    Eigen::Matrix3d sumSquares;
    for (int i = 0; i < channelsNum; ++i)
        for (int j = 0; j < channelsNum; ++j)
            sumSquares(i,j) = channelsSumOfSquares[i][j];

    hs.mean = sum / area;
    hs.covariance = sumSquares / area - hs.mean * hs.mean.transpose();
  }

  // calculate eigen vectors
  hs.eigSolver.compute(hs.covariance);
  needToUpdate = false;
}

Json::Value ColorVertex::jsonLog() const
{
	Json::Value root = Vertex::jsonLog();

  HelperStats const & hs = getHelperStats();

  root["cov"] = Json::arrayValue;
  for (int i = 0; i < channelsNum; ++i)
  {
    Json::Value cov_row = Json::arrayValue;
    for (int j = 0; j < channelsNum; ++j)
      cov_row.append(hs.covariance(i,j));
    root["cov"].append(cov_row);
  }

  root["values"] = Json::arrayValue;
  for (int i = 0; i < channelsNum; i++)
    root["values"].append(hs.eigenvalues()[i]);

  root["vectors"] = Json::arrayValue;
  for (int i = 0; i < channelsNum; i++)
  {
    Json::Value v = Json::arrayValue;
    for (int j = 0; j < channelsNum; ++j)
      v.append(double(hs.eigenvectors()(j, i)));
    root["vectors"].append(v);
  }

  return root;
}

}}	// ns vi::colorseg
