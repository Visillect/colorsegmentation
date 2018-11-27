#pragma once

#include <mximg/image.h>
#include <vi_cvt/ocv/image.hpp>

namespace mximg
{

inline cv::Mat asMat(PImage const& img)
{
  if (!img)
    return cv::Mat();
  return vi::cvt::ocv::as_cvmat(*img);
}

inline PImage createByCopy(cv::Mat const& mat)
{
  MinImg const mi = vi::cvt::ocv::as_minimg(mat);
  return createByCopy(&mi);
}

} // ns mximg
