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
#include <minbase/minimg.h>
#include <stdexcept>
#include "thirdparty_fwd.h"

#if defined _MSC_VER && I8R_EXPORTS
#  define I8R_API __declspec(dllexport)
#else
#  define I8R_API
#endif

namespace i8r {


class Exception : public std::runtime_error
{
public:
  Exception(std::string const& msg)
  : std::runtime_error(msg)
  { }
};


class ILogger
{
public:
  ILogger(ILogger &) = delete;
  ILogger & operator= (ILogger const&) = delete;

  virtual ~ILogger() { }
  virtual bool enabled() const = 0;

  virtual void save(std::string const& kind,
                    std::string const& id,
                    Json::Value const& val,
                    std::string const& opts) = 0;

  virtual void save(std::string const& kind,
                    std::string const& id,
                    cv::Mat const& val,
                    std::string const& opts) = 0;

  virtual void save(std::string const& kind,
                    std::string const& id,
                    MinImg const* val,
                    std::string const& opts) = 0;

  virtual std::string stream_id() const = 0;

protected:
  ILogger() { }
};

using PLogger = std::shared_ptr<ILogger>;
using PWLogger = std::weak_ptr<ILogger>;

} // ns i8r
