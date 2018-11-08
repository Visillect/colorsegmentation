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


#include "logger_impl.h"
#include "utils.h"
#include <minbase/minresult.h>
#include <minbase/minimg.h>
#include <opencv2/highgui/highgui.hpp>
#include <vi_cvt/ocv/image.hpp>
#include <vi_cvt/std/exception_macros.hpp>
#include <boost/filesystem.hpp>
#include <inttypes.h>
#include "image_dir_writer.h"
#include "image_group.h"
#include "video_writer.h"
#include "disabled_image_writer.h"


namespace i8r {

void LoggerImpl::save(std::string const& kind,
                      std::string const& id,
                      Json::Value const& val,
                      std::string const& opts)
{
  // number_a_msg acquires non-recursive lock; not using recursive
  // locks because contiguous locking is not needed and
  // experts advise against them: http://stackoverflow.com/a/1244997/1764
  int const msg_num = number_a_msg(id);

  std::lock_guard<decltype(guard_)> lock(guard_);
  PEnvironment const p_env = env_.lock();
  if (p_env)
  {
    if (!ldj_stream_.is_open())
      ldj_stream_.open(p_env->path_to_stream_file(stream_id_ + ".ldj").c_str(),
                       std::ios_base::out | std::ios_base::app | std::ios_base::binary);
  }
  if (!ldj_stream_.is_open())
    throw Exception("Unable to open .ldj file for stream " + stream_id_);

  Json::Value doc(Json::objectValue);
  doc["id"] = id;
  doc["w"] = kind;
  doc["d"] = val;
  doc["n"] = msg_num;
  ldj_stream_ << json_writer_.write(doc);
}

int LoggerImpl::number_a_msg(std::string const& id)
{
  std::lock_guard<decltype(guard_)> lock(guard_);
  if (id == current_id_)
  {
    ++current_id_msg_counter_;
  }
  else
  {
    current_id_ = id;
    current_id_msg_counter_ = 0;
  }
  return current_id_msg_counter_;
}

void LoggerImpl::save(std::string const& kind,
                      std::string const& id,
                      cv::Mat const& val,
                      std::string const& opts)
{
  int const msg_num = number_a_msg(id);
  PEnvironment env = env_.lock();
  if (!env)
    return;
  Json::Value config;
  env->get_config_for_kind(config, stream_id_, kind, opts);
  writer_by_kind(kind, config)->write(val, id, kind, opts, msg_num);
}

void LoggerImpl::save(std::string const& kind,
                      std::string const& id,
                      MinImg const* val,
                      std::string const& opts)
{
  save(kind, id, vi::cvt::ocv::as_cvmat(val), opts);
}

PImageWriter LoggerImpl::writer_by_kind(std::string const& kind, Json::Value const& config)
{
  std::lock_guard<decltype(guard_)> lock(guard_);
  PImageWriter & w = writer_by_kind_[kind];
  if (!w)
    w = create_writer(kind, config);
  return w;
}

PImageWriter LoggerImpl::create_writer(std::string const & kind,
                                       Json::Value const & config)
{
  std::string const schema = R"({
    "type": ["object", "null"],
    "properties": {
      "type": {
        "enum": ["image_dir", "video", "group", "disabled"]
      },
      "group": {
        "type": "string"
      }
    }
  })";
  validate_json(config, schema);

  std::string type = "image_dir";
  if (config.isMember("type"))
    type = config["type"].asString();
  if (type == "image_dir")
    return PImageWriter(new ImageDirWriter(env_, stream_id_, config));
  else if (type == "video")
    return PImageWriter(new VideoWriter(env_, stream_id_, config));
  else if (type == "disabled")
    return PImageWriter(new DisabledImageWriter);
  else if (type == "group")
  {
    if (!config.isMember("group") || !config["group"].isString())
      throw Exception("Group name must be specified for a grouped kind: " + kind);
    return group_writer(config["group"].asString());
  }
  else
    throw Exception("Unsupported image writer type: " + type);
}

PImageWriter LoggerImpl::group_writer(std::string const& group_name)
{
  PEnvironment env = env_.lock();
  if (!env)
    throw Exception("Must be able to access environment");

  PImageWriter & w = writer_by_group_[group_name];
  if (!w)
  {
    Json::Value cfg;
    env->get_config_for_kind(cfg, stream_id_, group_name, "");
    w.reset(new ImageGroup(cfg, group_name, shared_from_this(), env_));
  }
  return w;
}

} // ns i8r
