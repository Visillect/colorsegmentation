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
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "environment.h"

namespace i8r {

class ImageConverter
{
public:
  ImageConverter(ImageConverter const&) = delete;
  ImageConverter & operator= (ImageConverter const&) = delete;

  ImageConverter(Json::Value const& config);

  cv::Mat convert(cv::Mat const& img, cv::Size const& expected_size, int expected_type)
  {
    return convert_type(convert_size(img, expected_size), expected_type);
  }

private:
  bool do_upscale_ = false;
  bool do_downscale_ = false;
  bool do_interpolate_ = false;
  bool anisotropic_ = false;
  bool autocontrast_ = false;
  bool fixed_zero_ = false;
  double min_value_ = 0.0;
  double max_value_ = 255.0;

  static cv::Size correct_aspect_ratio(cv::Size const& in_size, cv::Size const& expected_size);

  cv::Mat convert_size(cv::Mat const& img, cv::Size const& expected_size);

  static cv::Mat resize_no_scale(cv::Mat const& img, cv::Size const& expected_size);

  cv::Mat convert_type(cv::Mat const& img, int expected_type);
};

using PImageConverter = std::shared_ptr<ImageConverter>;

} // ns i8r
