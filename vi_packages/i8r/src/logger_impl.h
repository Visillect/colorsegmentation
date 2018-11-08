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
#include <fstream>
#include <i8r/i8r.h>
#include <json-cpp/writer.h>
#include <mutex>
#include "environment.h"
#include "image_writer.h"

namespace i8r {

class LoggerImpl : public ILogger,
                   public std::enable_shared_from_this<LoggerImpl>
{
public:
  LoggerImpl(std::string const& stream_id, PWEnvironment const& env)
  : env_(env),
    stream_id_(stream_id)
  {
  }

  bool enabled() const override { return true; }

  void save(std::string const& kind,
            std::string const& id,
            Json::Value const& val,
            std::string const& opts) override;

  void save(std::string const& kind,
            std::string const& id,
            cv::Mat const& val,
            std::string const& opts) override;

  void save(std::string const& kind,
            std::string const& id,
            MinImg const* val,
            std::string const& opts) override;

  std::string stream_id() const override { return stream_id_; }

private:
  std::mutex guard_;

  PWEnvironment env_;
  std::string stream_id_;

  Json::FastWriter json_writer_;
  std::ofstream ldj_stream_;

  std::map<std::string, PImageWriter> writer_by_group_;
  std::map<std::string, PImageWriter> writer_by_kind_;

  std::string current_id_;
  int         current_id_msg_counter_ = 0;

  int number_a_msg(std::string const& id);

  PImageWriter writer_by_kind(std::string const& kind, Json::Value const& config);

  PImageWriter create_writer(std::string const& kind, Json::Value const& config);

  PImageWriter group_writer(std::string const& group_name);
};

} // ns i8r
