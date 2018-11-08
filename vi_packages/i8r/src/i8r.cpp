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


#include <i8r/i8r.h>
#include <json-cpp/writer.h>
#include <json-cpp/reader.h>
#include <inttypes.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <fstream>
#include "utils.h"
#include "environment.h"
#include "derived_logger.h"
#include "disabled_logger.h"

namespace i8r {

PEnvironment g_env(new Environment); // not static: avoid duplicate linkage


bool enabled()
{
  return g_env->enabled();
}

std::string path_for_stream(std::string const& stream_id)
{
  return enabled() ? g_env->path_to_stream_file(stream_id) : std::string();
}

std::string path_for_element(std::string const& stream_id, std::string const& filename)
{
  return enabled() ? g_env->path_to_element_file(stream_id, filename) : std::string();
}

void configure_from_file(std::string const& root,
                         std::string const& filename)
{
  bool const logger_enabled = !root.empty();
  Json::Value jso;
  if (logger_enabled && !filename.empty())
  {
    std::ifstream ifs(filename, std::ios_base::in | std::ios_base::binary);
    if (!ifs.is_open()) {
      throw std::runtime_error("i8r::configure_from_file(" + root + ", " + filename + "): cannot open file for reading");
    }
    ifs >> jso;
  }
  configure(root, jso);
}

void configure(std::string const& root, std::string const& cfg)
{
  Json::Value jso;
  if (!root.empty())
    parse_json(jso, cfg);
  return configure(root, jso);
}

void configure(std::string const& root, Json::Value const& cfg)
{
  g_env->configure(!root.empty(), root, cfg);
}

void shutdown()
{
  g_env.reset();
}

PLogger logger(std::string const& stream_id)
{
  return g_env->logger(stream_id);
}

PLogger disabled_logger()
{
  return std::make_shared<DisabledLogger>();
}

PLogger derive_logger(PLogger const& ll, std::string const& kind_group)
{
  return PLogger(new DerivedLogger(ll, kind_group + "."));
}

PLogger derive_stream(PLogger const& ll, std::string const& substream)
{
  return logger(ll->stream_id() + "." + substream);
}


std::string do_make_id_from_time(boost::posix_time::ptime const & t)
{
  return boost::posix_time::to_iso_string(t);
}

std::string do_make_id_from_num(int id)
{
  return unsafe_fmt_number(id, "%06d");
}

std::string do_make_id_from_short_num(int id)
{
  return unsafe_fmt_number(id, "%02d");
}

void get_computed_config(Json::Value & out,
                         std::string const& stream_id,
                         std::string const& kind,
                         std::string const& inline_config)
{
  g_env->get_config_for_kind(out, stream_id, kind, inline_config);
}
} // ns i8r
