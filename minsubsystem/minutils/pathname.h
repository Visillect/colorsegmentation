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

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDERS ``AS IS'' AND ANY EXPRESS OR
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

/**
 * @file   pathname.h
 * @brief  Crossplatform parsing filenames to path, name, extension
 */

#pragma once
#ifndef MINUTILS_PATHNAME_H_INCLUDED
#define MINUTILS_PATHNAME_H_INCLUDED

#include <string>

namespace se {
class pathname
{
public:
  std::string path; // given path
  // contains path string that should be parsed
  // path = "C:\\ququ\\file.ext" or "..\\ququ\\file.ext"  or "file.ext" etc.

  // below results of parsing
  std::string folder;      // respectively, results are "C:\\ququ", "..\\ququ", ""
  std::string name;        // "file.ext"
  std::string name_base;    // "file"
  std::string ext;        // "ext"
  pathname( const char* str = NULL ) { parse(str); }
  void parse( const char* str = NULL )
  {
    if (str!=NULL)
    {
      path = str;

      size_t found = path.find_last_of("/\\");
      if (found == std::string::npos) // slash not found
      {
        folder = "";
        name = path;
      }
      else
      {
        folder = path.substr(0,found);
        name = path.substr(found+1);
      }

      size_t found2 = name.find_last_of('.');
      name_base = name.substr(0,found2);
      ext = name.substr(found2+1);
    }
  }

  bool is_relative() const
  {
    if (path.empty())
      return false;
    size_t len = path.length();
    if (path[0] == '\\' || path[0] == '/')
      return false;
    if (len > 2 && path[1] == ':') // c:\sdf\sdf
      return false;
    // todo: correct for macOS, say "::" and other cases ...
    // for different cases may refer to
    // http://en.wikipedia.org/wiki/Path_(computing)
    return true;
  }
}; // class pathname
}; // namespace se::

#endif // #ifndef MINUTILS_PATHNAME_H_INCLUDED
