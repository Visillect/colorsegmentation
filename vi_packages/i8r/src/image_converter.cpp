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


#include "image_converter.h"
#include "utils.h"

namespace i8r {


ImageConverter::ImageConverter(Json::Value const& config)
{
  std::string static const schema = R"({
    "type": ["object", "null"],
    "properties": {
      "do_upscale": {"type": "boolean"},
      "do_downscale": {"type": "boolean"},
      "do_interpolate": {"type": "boolean"},
      "anisotropic": {"type": "boolean"},
      "autocontrast": {"type": "boolean"},
      "fixed_zero": {"type": "boolean"},
      "min_value": {"type": "number"},
      "max_value": {"type": "number"}
    }
  })";
  validate_json(config, schema);
  if (config.isMember("do_upscale"))
    do_upscale_ = config["do_upscale"].asBool();
  if (config.isMember("do_downscale"))
    do_downscale_ = config["do_downscale"].asBool();
  if (config.isMember("do_interpolate"))
    do_interpolate_ = config["do_interpolate"].asBool();
  if (config.isMember("anisotropic"))
    anisotropic_ = config["anisotropic"].asBool();
  if (config.isMember("autocontrast"))
    autocontrast_ = config["autocontrast"].asBool();
  if (config.isMember("fixed_zero"))
    fixed_zero_ = config["fixed_zero"].asBool();
  if (config.isMember("min_value"))
    min_value_ = config["min_value"].asDouble();
  if (config.isMember("max_value"))
    max_value_ = config["max_value"].asDouble();
}

cv::Size ImageConverter::correct_aspect_ratio(cv::Size const& in_size,
                                              cv::Size const& expected_size)
{
  cv::Size const eq_w = { expected_size.width,
                          static_cast<int>(0.5 + in_size.height
                                               * (1.0 * expected_size.width / in_size.width)) };
  if (eq_w.height <= expected_size.height)
    return eq_w;

  cv::Size const eq_h = { static_cast<int>(0.5 + in_size.width
                                               * (1.0 * expected_size.height / in_size.height)),
                          expected_size.height };
  if (eq_h.width <= expected_size.width)
    return eq_h;

  throw Exception("correct_aspect_ratio internal error");
}

cv::Mat ImageConverter::convert_size(cv::Mat const& img, cv::Size const& expected_size)
{
  if (img.size() == expected_size)
    return img;

  cv::Size scaled_size = img.size();
  if (anisotropic_)
  {
    if ((img.rows > expected_size.height && do_downscale_)
    ||  (img.rows < expected_size.height && do_upscale_))
    {
      scaled_size.height = expected_size.height;
    }
    if ((img.cols > expected_size.width && do_downscale_)
    ||  (img.cols < expected_size.width && do_upscale_))
    {
      scaled_size.width = expected_size.width;
    }
  }
  else
  {
    cv::Size const iso_size = correct_aspect_ratio(img.size(), expected_size);
    if ((img.cols > iso_size.width && do_downscale_)
    ||  (img.cols < iso_size.width && do_upscale_))
    {
      scaled_size = iso_size;
    }
  }

  cv::Mat scaled;
  int interpolation = cv::INTER_AREA;
  if (do_interpolate_
  &&  (scaled_size.width > img.cols || scaled_size.height > img.rows))
    interpolation = cv::INTER_LINEAR;

  cv::resize(img, scaled, scaled_size, 0, 0, interpolation);
  return resize_no_scale(scaled, expected_size);
}

cv::Mat ImageConverter::convert_type(cv::Mat const& img, int expected_type)
{
  int stype = img.type();
  if (stype == expected_type)
    return img;

  cv::Mat intermediate;
  if (img.channels() == 1)
  {
    double minval = min_value_;
    double maxval = max_value_;
    if (autocontrast_)
    {
      cv::minMaxLoc(img, &minval, &maxval);
      if (fixed_zero_)
        minval = 0;
    }
    double factor = 255.0 / (maxval - minval);
    double const add = -minval;
    img.convertTo(intermediate, CV_8UC1, factor, add);
  }
  else
  {
    intermediate = img;
  }
  stype = intermediate.type();

  cv::Mat result;
  if (expected_type == CV_8UC1)
  {
    if (stype == CV_8UC3)
      cv::cvtColor(intermediate, result, CV_BGR2GRAY);
    else
      throw Exception("Unsupported conversion");
  }
  else if (expected_type == CV_8UC3)
  {
    if (stype == CV_8UC1)
      cv::cvtColor(intermediate, result, CV_GRAY2BGR);
    else
      throw Exception("Unsupported conversion");
  }
  else
    throw Exception("Unsupported destination type");
  return result;
}

cv::Mat ImageConverter::resize_no_scale(cv::Mat const& img,
                                        cv::Size const& expected_size)
{
  cv::Mat frame(expected_size, img.type(), cv::Scalar(0));
  cv::Rect const roi(0, 0, std::min(img.cols, frame.cols), std::min(img.rows, frame.rows));
  img(roi).copyTo(frame(roi));
  return frame;
}

} // ns i8r
