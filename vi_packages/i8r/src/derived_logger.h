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
#include <i8r/types.h>


namespace i8r {

class DerivedLogger : public ILogger
{
public:
  DerivedLogger(PLogger const& logger,
                std::string const& kind_prefix)
  : logger_(logger),
    kind_prefix_(kind_prefix)
  {
    auto as_derived = std::dynamic_pointer_cast<DerivedLogger>(logger);
    if (as_derived)
    {
      // "un-derive"
      logger_ = as_derived->underlying_logger();
      kind_prefix_ = as_derived->kind_prefix() + kind_prefix_;
    }
  }

  bool enabled() const override { return logger_->enabled(); }

  void save(std::string const& kind,
            std::string const& id,
            Json::Value const& val,
            std::string const& opts) override
  {
    logger_->save(kind_prefix_ + kind, id, val, opts);
  }

  void save(std::string const& kind,
            std::string const& id,
            cv::Mat const& val,
            std::string const& opts) override
  {
    logger_->save(kind_prefix_ + kind, id, val, opts);
  }

  void save(std::string const& kind,
            std::string const& id,
            MinImg const* val,
            std::string const& opts) override
  {
    logger_->save(kind_prefix_ + kind, id, val, opts);
  }

  std::string stream_id() const override { return logger_->stream_id(); }

  std::string const& kind_prefix() const { return kind_prefix_; }

  PLogger underlying_logger() const { return logger_; }

private:
  PLogger     logger_;
  std::string kind_prefix_;
};

} // ns i8r
