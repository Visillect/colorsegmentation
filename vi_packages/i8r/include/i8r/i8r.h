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


/*  DESIGN GOALS

  * Fully configurable at runtime
  * Absolutely minimal impact on performance when disabled
  * Does not force heavy header dependency (i.e. no transitively including
      OpenCV if working only with MinImg)
  * Per-stream and per-kind output control
*/


#pragma once
#include <memory>
#include <functional>
#include <i8r/types.h>
#include <i8r/thirdparty_fwd.h>

namespace i8r {

I8R_API void configure(std::string const& root, Json::Value const& props);
I8R_API void configure(std::string const& root, std::string const& props = "");
I8R_API void configure_from_file(std::string const& root, std::string const& filename);
I8R_API void shutdown();

class AutoShutdown
{
public:
  AutoShutdown() { }
  ~AutoShutdown() { shutdown(); }
private:
  AutoShutdown(AutoShutdown const&) = delete;
  AutoShutdown & operator= (AutoShutdown const&) = delete;
};

I8R_API PLogger logger(std::string const& stream_id);
I8R_API PLogger disabled_logger();

I8R_API PLogger derive_logger(PLogger const& ll, std::string const& kind_group);
I8R_API PLogger derive_stream(PLogger const& ll, std::string const& substream);

I8R_API bool enabled();

I8R_API std::string path_for_stream(std::string const& stream_id);
I8R_API std::string path_for_element(std::string const& stream_id, std::string const& filename);

inline bool enabled(std::string const& stream_id)
{
  return enabled() && logger(stream_id)->enabled();
}

inline void with_logger(std::string const& stream_id,
                        std::function<void(PLogger const&)> const& action)
{
  if (enabled(stream_id))
    action(logger(stream_id));
}

inline void with_logger(PLogger const& ll,
                        std::function<void()> const& action)
{
  if (ll && ll->enabled())
    action();
}

I8R_API std::string do_make_id_from_num(int id);
I8R_API std::string do_make_id_from_short_num(int id);
I8R_API std::string do_make_id_from_time(boost::posix_time::ptime const& t);

inline std::string id_from_num(int num)
{
  if (enabled())
    return do_make_id_from_num(num);
  else
    return std::string();
}

inline std::string id_from_short_num(int num)
{
  if (enabled())
    return do_make_id_from_short_num(num);
  else
    return std::string();
}

inline std::string id_from_t(boost::posix_time::ptime const& t)
{
  if (enabled())
    return do_make_id_from_time(t);
  else
    return std::string();
}

I8R_API void get_computed_config(Json::Value & out,
                                 std::string const& stream_id,
                                 std::string const& kind,
                                 std::string const& inline_config);

} // ns i8r
