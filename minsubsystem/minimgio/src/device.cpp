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

#include <cstring>
#include <cstdio>

#include <minimgio/device.h>
#include <minimgio/define.h>
#include <minbase/minresult.h>

#include "subsystem.h"
#include "stream.h"

MINIMGIO_API int GetDeviceList
(
  const char *pSubSystemName,
  char       *pDeviceNames,
  int         size
)
{
  if (pSubSystemName == NULL || pDeviceNames == NULL)
    return BAD_ARGS;

  SubSystem *pSubSystem = SubSystem::Instance(pSubSystemName);
  if (pSubSystem == NULL)
    return BAD_ARGS;

  return pSubSystem->GetDeviceNameList(pDeviceNames, size);
}

MINIMGIO_API int OpenStream
(
  const char *pSubSystemName,
  const char *pDeviceName,
  char       *pURI,
  int         size
)
{
  if (pSubSystemName == NULL || pDeviceName == NULL)
    return BAD_ARGS;

  SubSystem *pSubSystem = SubSystem::Instance(pSubSystemName);
  if (pSubSystem == NULL)
    return BAD_ARGS;

  return pSubSystem->OpenStream(pDeviceName, pURI, size);
}

MINIMGIO_API int CloseStream
(
  const char *pURI
)
{
  if (pURI == NULL)
    return BAD_ARGS;

  Stream *pStream = NULL;
  sscanf(pURI, "dev://%p", &pStream);
  if (pStream == NULL)
    return INTERNAL_ERROR;

  delete pStream;
  return NO_ERRORS;
}

MINIMGIO_API int GetStreamProperty
(
  const char *pURI,
  const char *pPropertyKey,
  char       *pPropertyValue,
  int         size
)
{
  if (pURI == NULL || pPropertyKey == NULL || pPropertyValue == NULL)
    return BAD_ARGS;

  Stream *pStream = NULL;
  sscanf(pURI, "dev://%p", &pStream);
  if (pStream == NULL)
    return INTERNAL_ERROR;

  return pStream->GetProperty(pPropertyKey, pPropertyValue, size);
}

MINIMGIO_API int SetStreamProperty
(
  const char *pURI,
  const char *pPropertyKey,
  const char *pPropertyValue
)
{
  if (pURI == NULL || pPropertyKey == NULL || pPropertyValue == NULL)
    return BAD_ARGS;

  Stream *pStream = NULL;
  sscanf(pURI, "dev://%p", &pStream);
  if (pStream == NULL)
    return INTERNAL_ERROR;

  return pStream->SetProperty(pPropertyKey, pPropertyValue);
}
