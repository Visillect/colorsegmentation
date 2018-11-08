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
#include "image_writer.h"
#include "image_converter.h"
#include "draw_utils.h"
#include <opencv2/core/core.hpp>
#include <json-cpp/writer.h>


namespace i8r {

class ImageGroup : public IImageWriter
{
public:
  ImageGroup(Json::Value const& config,
             std::string const& group_kind,
             PWLogger const& ll,
             PWEnvironment const& env)
  : logger_(ll),
    env_(env),
    group_kind_(group_kind)
  {
    Json::FastWriter fw;
    config_str_ = fw.write(config);

    std::string static const config_schema = R"({
      "type": ["object", "null"],
      "properties": {
        "is_vertical": { "type": "boolean" }
      }
    })";
    validate_json(config, config_schema);
    if (config.isMember("is_vertical"))
      is_vertical_ = config["is_vertical"].asBool();
  }

  ~ImageGroup()
  {
    PLogger ll = logger_.lock();
    if (ll)
      emit_acc_image(ll);
  }

  void write(cv::Mat const& img,
             std::string const& id,
             std::string const& kind,
             std::string const& inline_config,
             int msg_num) override
  {
    PLogger ll = logger_.lock();
    if (id != current_id_)
    {
      emit_acc_image(ll);
      current_id_ = id;
      accumulated_image_ = cv::Mat();
    }

    if (!ll)
      return;
    PImageConverter & cvt = cvt_by_kind_[kind];
    if (!cvt)
    {
      PEnvironment env = env_.lock();
      if (!env)
        return;
      Json::Value cvt_cfg;
      env->get_config_for_kind(cvt_cfg, ll->stream_id(), kind, inline_config);
      cvt.reset(new ImageConverter(cvt_cfg));
    }
    process_converted(cvt->convert(img, img.size(), CV_8UC3));
  }
private:
  PWLogger logger_;
  PWEnvironment env_;
  std::string config_str_;
  std::map<std::string, PImageConverter> cvt_by_kind_;
  std::string group_kind_;
  bool is_vertical_ = false;

  std::string current_id_;
  cv::Mat accumulated_image_;

  void emit_acc_image(PLogger const& ll)
  {
    if (!accumulated_image_.empty())
      ll->save(group_kind_, current_id_, accumulated_image_, config_str_);
  }

  void process_converted(cv::Mat const& img)
  {
    if (accumulated_image_.empty())
      accumulated_image_ = img.clone();
    else if (is_vertical_)
      accumulated_image_ = draw_utils::vstack(accumulated_image_, img);
    else
      accumulated_image_ = draw_utils::hstack(accumulated_image_, img);
  }

};

}
