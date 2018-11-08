/*

Copyright (c) 2011, Smart Engines Limited. All rights reserved.

All rights reserved.

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

#include <cassert>
#include <cstring>

#include <minbase/minresult.h>
#include <minbase/crossplat.h>

#include "camstream.h"

CamStream::CamStream(Device *p_device) : p_device_(NULL)
{
  assert(p_device != NULL);
  if (p_device == NULL)
    return;

  if (p_device->Take() < 0)
    return;

  p_device_ = p_device;
}

CamStream::~CamStream()
{
  assert(p_device_ != NULL);
  if (p_device_ == NULL)
    return;

  p_device_->Release();
}

int CamStream::PushImage(const MinImg * /* pImg */)
{
  return NO_SENSE;
}

int CamStream::QueryImage(const MinImg *pImg)
{
  assert(p_device_);
  if (p_device_ == NULL)
    return INTERNAL_ERROR;

  return p_device_->QueryImage(pImg);
}

int CamStream::QueryImagePropsEx(MinImg *pImg, ExtImgProps *pProps)
{
  assert(p_device_);
  if (p_device_ == NULL)
    return INTERNAL_ERROR;

  return p_device_->QueryImagePropsEx(pImg, pProps);
}

int CamStream::GetProperty(const char *pKey, char *pValue, int size)
{
  assert(p_device_);
  if (p_device_ == NULL)
    return INTERNAL_ERROR;

  return p_device_->GetProperty(pKey, pValue, size);
}

int CamStream::SetProperty(const char *pKey, const char *pValue)
{
  assert(p_device_);
  if (p_device_ == NULL)
    return INTERNAL_ERROR;

  return p_device_->SetProperty(pKey, pValue);
}
