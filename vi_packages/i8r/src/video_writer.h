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
#include <memory>
#include <string>
#include <mutex>
#include <opencv2/highgui/highgui.hpp>
#include <json-cpp/value.h>
#include <boost/filesystem.hpp>
#include "environment.h"
#include "utils.h"
#include "image_writer.h"
#include "image_converter.h"

namespace i8r {

class VideoWriter : public IImageWriter
{
public:
  VideoWriter(PWEnvironment const& env,
              std::string const& stream_id,
              Json::Value const& config)
  : env_(env),
    stream_id_(stream_id),
    converter_(config)
  {
    load_config(config);
  }

  void write(cv::Mat const& img,
             std::string const& id,
             std::string const& kind,
             std::string const& inline_config,
             int msg_num) override
  {
    std::lock_guard<decltype(guard_)> lock(guard_);

    if (kind_.empty())
      kind_ = kind;
    if (kind != kind_)
      throw Exception("Cannot write different kinds: was created for " + kind_ + ", got " + kind);

    if (!video_writer_.isOpened())
    {
      PEnvironment const p_env = env_.lock();
      if (!p_env)
        return;

      if (video_props_.frame_size.width == 0
      &&  video_props_.frame_size.height == 0)
      {
        video_props_.frame_size = img.size();
      }

      p_env->do_synchronized([this](Environment & env) {
        std::string const filename = env.path_to_stream_file(stream_id_ + "." + kind_ + ".avi");
        if (boost::filesystem::exists(filename))
          throw Exception("Video file already exists: " + filename);
        if (!video_writer_.open(filename, video_props_.fourcc,
                                video_props_.fps, video_props_.frame_size, true))
          throw Exception("Unable to open video writer: " + filename);
      });
    }

    if (!video_writer_.isOpened())
      throw Exception("Video writer for stream " + stream_id_ + " is not open");
#if OPENCV3
    video_writer_.set(cv::VIDEOWRITER_PROP_QUALITY, video_props_.quality);
#endif
    // Not using do_synchronized to avoid lock contention between writers;
    // private mutex should be locked now.
    cv::Mat frame;
    if (video_props_.invert_channels) {
      cv::cvtColor(img, frame, CV_RGB2BGR);
    } else {
      frame = img;
    }
    video_writer_.write(converter_.convert(frame, video_props_.frame_size, CV_8UC3));
  }

protected:
  struct VideoProps
  {
    int fourcc = CV_FOURCC('F', 'M', 'P', '4');
    cv::Size frame_size = {0, 0};
    double fps = 25;
    double quality = 0.9;
    bool invert_channels = false;
  };

  std::mutex guard_;
  PWEnvironment env_;
  std::string const stream_id_;
  std::string kind_;
  VideoProps video_props_;
  cv::VideoWriter video_writer_;
  ImageConverter converter_;

  void load_config(Json::Value const& config)
  {
    std::string static const config_schema = R"({
      "type": ["object", "null"],
      "properties": {
        "fourcc": {
          "type": "string",
          "pattern": "^[0-9A-Z]{4}$"
        },
        "fps": {
          "type": "number",
          "minimum": 0,
          "exclusiveMinimum": true
        },
        "invert_channels": { "type": "boolean" },
        "quality": {
          "type": "number",
          "minimum": 0,
          "maximum": 1.0,
          "exclusiveMinimum": true
        },
        "frame_size": {
          "type": ["array", "null"],
          "items": {
            "type": "integer",
            "minimum": 0,
            "exclusiveMinimum": true
          },
          "minItems": 2,
          "maxItems": 2
        }
      },
      "additionalProperties": true
    })";
    validate_json(config, config_schema);

    if (config.isMember("fourcc"))
    {
      std::string const fourcc_str = config["fourcc"].asString();
      video_props_.fourcc = CV_FOURCC(fourcc_str[0], fourcc_str[1], fourcc_str[2], fourcc_str[3]);
    }
    if (config.isMember("frame_size") && !config["frame_size"].isNull())
    {
      video_props_.frame_size = { config["frame_size"][static_cast<Json::Value::UInt>(0)].asInt(),
                                  config["frame_size"][1].asInt() };
    }
    if (config.isMember("fps"))
    {
      video_props_.fps = config["fps"].asDouble();
    }
    if (config.isMember("quality"))
    {
      video_props_.quality = config["quality"].asDouble();
    }
    if (config.isMember("invert_channels"))
    {
      video_props_.invert_channels = config["invert_channels"].asBool();
    }
  }
};

} // ns i8r
