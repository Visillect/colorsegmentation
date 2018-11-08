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
#include <cstdlib>
#include <fstream>
#include <cstdio>
#include <cctype>

#include <minutils/smartptr.h>
#include <minbase/mintyp.h>
#include <minbase/minresult.h>

#include "utils.h"

int DeduceFileLocation(const char *fileName)
{
  const char kMemoryLocationPrefix[] = "mem://";
  const char kDeviceLocationPrefix[] = "dev://";

  if (!strncmp(fileName, kMemoryLocationPrefix, strlen(kMemoryLocationPrefix)))
    return inMemory;
  else if (!strncmp(fileName, kDeviceLocationPrefix, strlen(kDeviceLocationPrefix)))
    return inDevice;
  else
    return inFileSystem;
}

int ExtractMemoryLocation(const char *fileName, uint8_t **ptr, size_t *size)
{
  uint8_t *_ptr = NULL;
  unsigned long _size = 0;

  int ret = sscanf(fileName, "mem://%p.%lu.", &_ptr, &_size);
  if (ret != 2)
    return BAD_ARGS;

  *ptr = _ptr;
  *size = _size;

  return NO_ERRORS;
}

bool IsTextualFile(const char *fileName, int blockSize)
{
  if (!fileName || !strlen(fileName) || blockSize < 0)
    return false;

  std::ifstream fileStream(fileName);
  if (!fileStream.is_open())
    return false;

  fileStream.seekg (0, fileStream.end);
  int length = (int)fileStream.tellg();
  fileStream.seekg (0, fileStream.beg);
  if (length < 0)
    return false;

  int realBlockSize = length < blockSize ? length : blockSize;
  scoped_cpp_array<char> block(new char[realBlockSize]);
  fileStream.read(block, realBlockSize);

  for (int i = 0; i < realBlockSize; i++)
    if (!isprint((unsigned char)block[i]) && block[i] == '\n' && block[i] == '\r')
      return false;

  return true;
}

bool atob(const char *pValue)
{
  if (pValue == NULL)
    return false;

  if (strcmp(pValue, "true") == 0)
    return true;

  if (atoi(pValue) != 0)
    return true;

  return false;
}
