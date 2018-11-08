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

#pragma once
#ifndef MINUTILS_RESIDUALS_H_INCLUDED
#define MINUTILS_RESIDUALS_H_INCLUDED

#include <map>
#include <set>
#include <string>
#include <fstream>
#include <limits>

// "Nevyazka" - distance from ideal - accumulated reports by subsystems
class Residuals
{
private:
  static const int unknownValue = -1;
  //static const int unknownGroundTruth = -1;
  std::map<std::string, double> values;

public:
  Residuals() : values() {}
  Residuals(const Residuals& other) : values(other.values) {}
  Residuals& operator=(const Residuals& other)
  {
    if (this != &other)
      values = other.values;
    return *this;
  }

public:
  void setValue(const char* key, double value)
  {
    values[key] = value;
  }

  double getValue(const char* key) const
  {
    std::map<std::string, double>::const_iterator it = values.find(key);
    if (it == values.end())
      return static_cast<double>(unknownValue);
    return it->second;
  }

  double getMax(const std::set<std::string>& subsystems) const
  {
    double maxValue = std::numeric_limits<double>::min();
    std::set<std::string>::const_iterator it;
    for (it = subsystems.begin(); it != subsystems.end(); ++it)
    {
      maxValue = std::max(maxValue, getValue((*it).c_str()));
    }
    return maxValue;
  }

  void updateSubsystems(std::set<std::string>& subsystems,
                        const std::string& pattern = "") const
  {
    std::map<std::string, double>::const_iterator it;
    for (it = values.begin(); it != values.end(); ++it)
    {
      const std::string& cur = it->first;
      if (cur.find(pattern) != cur.npos)
      {
        subsystems.insert(cur);
      }
    }
  }

  bool isEmpty() const
  {
    return values.empty();
  }

  bool write(std::fstream& out) const
  {
    if (out.fail())
      return false;

    out << "Residuals" << std::endl;
    std::map<std::string, double>::const_iterator it;
    for(it = values.begin(); it != values.end(); ++it)
    {
      out << it->first << std::endl << it->second << std::endl;
    }

    out << std::endl;
    out.flush();
    return true;
  }

  bool read(std::fstream& in)
  {
    if (in.fail())
      return false;

    std::string key;
    std::getline(in, key);
    while (key.size() > 0)
    {
      double value = 0;
      in >> value;
      setValue(key.c_str(), value);

      std::getline(in, key);
      std::getline(in, key);
    }
    return true;
  }
};

#endif // #ifndef MINUTILS_RESIDUALS_H_INCLUDED
