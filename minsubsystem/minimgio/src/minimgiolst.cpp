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

#include <minutils/smartptr.h>
#include <minbase/minresult.h>

#include <cstring>
#include <fstream>
#include <string>
#include <algorithm>

#include "minimgiojpeg.h"
#include "utils.h"

static std::string trimLeft(const std::string &s)
{
  static const std::string whitespace = " \n\r\t";
  size_t startpos = s.find_first_not_of(whitespace);
  return (startpos == std::string::npos) ? "" : s.substr(startpos);
}

static std::string trimRight(const std::string &s)
{
  static const std::string whitespace = " \n\r\t";
  size_t endpos = s.find_last_not_of(whitespace);
  return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
}

static std::string trim(const std::string &s)
{
  return trimRight(trimLeft(s));
}

static std::string getTrimmedLine(std::istream &stream)
{
  std::string imageName;
  std::getline(stream, imageName);
  return trim(imageName);
}

static std::string absoluteImagePath(const std::string &lstPath, const std::string &imageName)
{
  std::string::size_type pos = lstPath.find_last_of("/\\");
  const std::string &lstDirPath = (pos == std::string::npos) ? "." : lstPath.substr(0, pos);
  return lstDirPath + "/" + imageName;
}

int GetLstPages(const char *pFileName)
{
  if (!pFileName || !strlen(pFileName))
    return BAD_ARGS;

  std::ifstream fileStream(pFileName);
  if (fileStream.is_open() == false)
    return FILE_ERROR;

  int pageCount = 0;
  while(fileStream.eof() == false)
  {
    std::string imageName = getTrimmedLine(fileStream);
    if (imageName.length() == 0 || imageName[0] == ';')
      continue;
    if (imageName == "#stop" || imageName == "#end")
      break;

    pageCount++;
  }

  return pageCount;
}

int GetLstPageName(char *pPageName, int pageNameSize, const char *pFileName, int page)
{
  if (!pPageName || pageNameSize <= 0)
    return BAD_ARGS;
  if (!pFileName || !strlen(pFileName))
    return BAD_ARGS;

  int pageIndex = -1;
  std::ifstream fileStream(pFileName);
  if (fileStream.is_open() == false)
    return FILE_ERROR;

  while(fileStream.eof() == false)
  {
    std::string imageName = getTrimmedLine(fileStream);
    if (imageName.length() == 0 || imageName[0] == ';')
      continue;
    if (imageName == "#stop" || imageName == "#end")
      break;

    if (++pageIndex == page)
    {
      ::memset(pPageName, 0, pageNameSize);
      ::memcpy(pPageName, imageName.c_str(), std::min((int)imageName.size(), pageNameSize));
      return NO_ERRORS;
    }
  }

  return BAD_ARGS;
}

int GetLstPropsEx(MinImg *pImg, ExtImgProps *pProps, const char *pFileName, int page)
{
  char szPageName[4096] = {0};
  PROPAGATE_ERROR(GetLstPageName(szPageName, sizeof(szPageName), pFileName, page));
  return GetMinImageFilePropsEx(pImg, pProps, absoluteImagePath(pFileName, szPageName).c_str());
}

int LoadLst(const MinImg *pImg, const char *pFileName, int page)
{
  char szPageName[4096] = {0};
  PROPAGATE_ERROR(GetLstPageName(szPageName, sizeof(szPageName), pFileName, page));
  return LoadMinImage(pImg, absoluteImagePath(pFileName, szPageName).c_str());
}

int SaveLstEx(const char * /*pFileName*/, const MinImg * /*pImg*/,
              const ExtImgProps * /*pProps*/, int /*page*/)
{
  return NOT_IMPLEMENTED;
}

