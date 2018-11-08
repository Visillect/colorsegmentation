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
#include <opencv2/highgui/highgui.hpp>
#include <json-cpp/value.h>
#include "environment.h"
#include "image_writer.h"
#include "utils.h"
#include "image_converter.h"
#include <inttypes.h>


namespace i8r {

enum class OverwriteAction
{
  SKIP,
  THROW,
  OVERWRITE
};

OverwriteAction parse_overwrite_action(std::string const& s)
{
  if (s == "skip")
    return OverwriteAction::SKIP;
  if (s == "throw`")
    return OverwriteAction::THROW;
  if (s == "overwrite")
    return OverwriteAction::OVERWRITE;
  throw std::runtime_error("Unknown overwrite action: " + s);
}

class ImageDirWriter : public IImageWriter
{
public:
  ImageDirWriter(PWEnvironment const& env,
                 std::string const& stream_id,
                 Json::Value const& config)
  : env_(env),
    stream_id_(stream_id),
    converter_(config)
  {
    std::string static const config_schema = R"({
      "type": ["object", "null"],
      "properties": {
        "overwrite_action": {
          "type": "string",
          "enum": ["skip", "throw", "overwrite"]
        },
        "file_extension": {
          "type": "string",
          "enum": ["png", "jpg", "tif"]
        }
      },
      "additionalProperties": true
    })";
    validate_json(config, config_schema);
    if (config.isMember("overwrite_action"))
      overwrite_action_ = parse_overwrite_action(config["overwrite_action"].asString());
    if (config.isMember("file_extension"))
      file_extension_ = config["file_extension"].asString();
  }

  void write(cv::Mat const& img,
             std::string const& id,
             std::string const& kind,
             std::string const& inline_config,
             int msg_num) override
  {
    namespace bfs = boost::filesystem;

    PEnvironment p_env = env_.lock();
    std::string const numbered_kind = unsafe_fmt_number(msg_num, "%02d") + kind;
    if (p_env)
    {
      cv::Mat const converted = converter_.convert(img.clone(), img.size(), CV_8UC3);
      std::string const stream_id = stream_id_;
      std::string const ext = "." + file_extension_;
      OverwriteAction const overwrite_action = overwrite_action_;
      p_env->do_synchronized([stream_id,
                              numbered_kind,
                              converted,
                              id,
                              ext,
                              overwrite_action](Environment & env) {
        std::string const basename = id + "_" + numbered_kind + ext;
        std::string const filename = env.path_to_element_file(stream_id, basename);

        if (bfs::exists(filename))
        {
          if (overwrite_action == OverwriteAction::THROW)
            throw Exception("Will not overwrite file: " + filename);
          else if (overwrite_action == OverwriteAction::SKIP)
            return;
        }

        if (!cv::imwrite(filename, converted))
          throw Exception("Unable to save image: " + filename);
      });
    }
  }

protected:
  PWEnvironment env_;
  std::string const stream_id_;
  std::string file_extension_ = "png";
  ImageConverter converter_;
  OverwriteAction overwrite_action_ = OverwriteAction::THROW;
};

} // ns i8r
