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

#pragma warning (disable : 4995)

#include "dshowsubsystem.h"
#include "dshowdevice.h"
#include "dshowutils.h"
#include "camstream.h"

#include <cstring>
#include <comdef.h>
#include <algorithm>

#include <minbase/minresult.h>
#include <minbase/crossplat.h>

DShowSubSystem::DShowSubSystem()
{
  CoInitialize(NULL);
}

SubSystem * DShowSubSystem::Instance()
{
  static DShowSubSystem *pInstance = NULL;

  if (pInstance == NULL)
    pInstance = new DShowSubSystem();

  return pInstance;
}

int DShowSubSystem::GetDeviceNameList(char *pDeviceNames, int size) const
{
  if (pDeviceNames == NULL)
    return BAD_ARGS;

  std::string deviceNameList;
  HRESULT hr = GetCaptureDeviceList(deviceNameList);
  if (FAILED(hr))
    return INTERNAL_ERROR;

  snprintf(pDeviceNames, size, "%s", deviceNameList.c_str());
  return NO_ERRORS;
}

int DShowSubSystem::OpenStream(const char *pDeviceName, char *pURI, int size)
{
  Device *pDevice = DShowDevice::Instance(pDeviceName);
  if (pDevice == NULL)
    return INTERNAL_ERROR;

  Stream *pStream = new CamStream(pDevice);
  if (pStream == NULL)
    return INTERNAL_ERROR;

  snprintf(pURI, size, "dev://%p", pStream);
  return NO_ERRORS;
}
