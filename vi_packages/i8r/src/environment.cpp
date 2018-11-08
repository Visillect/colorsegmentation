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


#include "environment.h"
#include "logger_impl.h"
#include "utils.h"
#include <boost/filesystem.hpp>

namespace i8r {

namespace bfs = boost::filesystem;

void Environment::configure(bool global_enabled,
                            std::string const& root,
                            Json::Value const& cfg)
{
  std::lock_guard<decltype(guard_)> lock(guard_);
  root_path_ = root;
  logger_by_stream_.clear();
  enabled_.store(global_enabled);

  std::string const schema = R"({
    "type": ["object", "null"],
    "properties": {
      "default": { "type": "object" },
      "streams": {
        "type": "object",
        "additionalProperties": {
          "oneOf": [
            { "enum": ["disabled"]},
            {
              "type": "object",
              "additionalProperties": { "type": "object" }
            }
          ]
        }
      },
      "kinds": {
        "type": "object",
        "additionalProperties": { "type": "object" }
      }
    }
  })";
  validate_json(cfg, schema);
  config_ = cfg;
}

PLogger Environment::logger(std::string const& stream_id)
{
  if (!enabled())
    return disabled_logger_;

  std::lock_guard<decltype(guard_)> lock(guard_);
  PLogger & ll = logger_by_stream_[stream_id];
  if (!ll)
    ll = create_default_logger(stream_id);
  return ll;
}

PLogger Environment::create_default_logger(std::string const& stream_id)
{
  if (!enabled() || !is_stream_enabled(stream_id))
    return disabled_logger_;
  return PLogger(new LoggerImpl(stream_id, shared_from_this()));
}

void Environment::do_synchronized(std::function<void(Environment & env)> const& task,
                                  bool allow_async)
{
  if (enabled())
  {
    std::lock_guard<decltype(guard_)> lock(guard_);
    task(*this);
  }
}

std::string Environment::path_to_element_file(std::string const & stream_id, std::string const & filename) const
{
  bfs::path const dir = bfs::path(root_path_) / stream_id;
  bfs::create_directories(dir);
  return (dir / filename).string();
}

std::string Environment::path_to_stream_file(std::string const & filename) const
{
  bfs::create_directories(root_path_);
  return (bfs::path(root_path_) / filename).string();
}

void Environment::get_config_for_kind(Json::Value & out,
                                      std::string const & stream_id,
                                      std::string const & kind,
                                      std::string const & inline_config)
{
  // TODO: cache parsed and combined configs
  Json::Value inline_jso;
  parse_json(inline_jso, inline_config);
  compute_config_for_kind(out, stream_id, kind, inline_jso);
}

bool Environment::is_stream_enabled(std::string const& stream_id) const
{
  Json::Value const config_for_stream = config_["streams"][stream_id];
  if (config_for_stream.isString())
  {
    if (config_for_stream.asString() == "disabled")
      return false;
    else
      throw Exception("Unsupported stream config string: " + config_for_stream.asString());
  }
  return true;
}


void Environment::compute_config_for_kind(Json::Value & out,
                                          std::string const& stream_id,
                                          std::string const& kind,
                                          Json::Value const& inline_config)
{
  if (!is_stream_enabled(stream_id))
  {
    out == Json::Value(Json::nullValue);
    return;
  }
  out = config_["default"];
  override_json_objects(out, inline_config);
  override_json_objects(out, config_["kinds"][kind]);
  override_json_objects(out, config_["streams"][stream_id][kind]);
}


} // ns i8r
