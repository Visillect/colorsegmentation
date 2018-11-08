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
#include <minbase/minimg.h>
#include <sensor_msgs/Image.h>
#include <minbase/minresult.h>
#include <vi_cvt/std/exceptions.hpp>
#include <vi_cvt/std/exception_macros.hpp>


namespace vi { namespace cvt { namespace ros {

inline sensor_msgs::Image as_image_msg(MinImg const* img) {
  sensor_msgs::Image ros_img;
  std::string encoding = "invalid";
  int32_t channel_size = img->stride/(img->channels*img->width);
  if (channel_size != 8 or channel_size != 16) {
    throw MinResultException(BAD_ARGS,
                             "MinImg channel size " + std::to_string(channel_size)
                             + " is not supported for ros Image",
                             "as_image_msg", __FILE__, __LINE__);
  }
  if (img->channels == 1) {
    encoding = "mono" + std::to_string(channel_size);
  } else if (img->channels == 3) {
    encoding = "rgb" + std::to_string(channel_size);
  } else if (img->channels == 4) {
    encoding = "rgba" + std::to_string(channel_size);
  } else {
    throw MinResultException(BAD_ARGS,
                             "MinImg channel number " + std::to_string(img->channels)
                             + " is not supported for ros Image (only 1, 3 or 4)",
                             "as_image_msg", __FILE__, __LINE__);
  }
  ros_img.height = img->height;
  ros_img.width = img->width;
  ros_img.encoding = encoding;
  ros_img.is_bigendian = false;
  ros_img.step = img->stride;
  for (int i=0; i<(img->height*img->stride); i++) {
    ros_img.data.push_back(img->pScan0[i]);
  }
}


}}} // ns vi::cvt::ros