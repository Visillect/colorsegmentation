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
#include <vi_cvt/std/exceptions.hpp>


#ifdef THROW_ON_MINERR
#  error THROW_ON_MINERR definition conflict
#endif

#define THROW_LIKE_MINERR(code, msg) do {\
    throw vi::cvt::MinResultException(code, msg, "THROW_LIKE_MINERR", __FILE__, __LINE__); \
  } while (false)

#define THROW_ON_MINERR(expr) do {\
    int const _THROW_ON_MINERR_ret = (expr); \
    if (_THROW_ON_MINERR_ret < 0) throw vi::cvt::MinResultException(_THROW_ON_MINERR_ret, "THROW_ON_MINERR", #expr, __FILE__, __LINE__); \
  } while (false)

#define THROW_ON_NONZERO(expr) do {\
    int const _THROW_ON_MINERR_ret = (expr); \
    if (_THROW_ON_MINERR_ret != 0) throw vi::cvt::MinResultException(_THROW_ON_MINERR_ret, "THROW_ON_NONZERO", #expr, __FILE__, __LINE__); \
  } while (false)

#ifdef PROPAGATE_EXC_AS_MINERR
#  error PROPAGATE_EXC_AS_MINERR definition conflict
#endif

#define PROPAGATE_EXC_AS_MINERR(expr) do { \
    try { expr; } \
    catch (vi::cvt::MinResultException const& _mre) { return _mre.code(); } \
    catch (std::exception const&) { return INTERNAL_ERROR; } \
  } while (false)

