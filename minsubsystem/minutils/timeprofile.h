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
#ifndef MINUTILS_TIMEPROFILE_H_INCLUDED
#define MINUTILS_TIMEPROFILE_H_INCLUDED

#include <map>
#include <set>
#include <string>
#include <fstream>

#include <minstopwatch/hiresclock.h>

class TimeProfile
{
private:
  static const int unknownValue = 0;
  std::map<std::string, double> values;

public:
  TimeProfile() : values() {}
  TimeProfile(const TimeProfile& other) : values(other.values) {}
  TimeProfile& operator=(const TimeProfile& other)
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
      return 0.0;
    return it->second;
  }

  void updateSubsystems(std::set<std::string>& subsystems,
                        const std::string& pattern = "") const
  {
    std::map<std::string, double>::const_iterator it;
    for(it = values.begin(); it != values.end(); ++it)
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

    out << "TimeProfile" << std::endl;
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
    if(in.fail())
      return false;

    std::string key;
    double value = 0;
    std::getline(in, key);
    while(key.size() > 0)
    {
      in >> value;
      setValue(key.c_str(), value);
      std::getline(in, key);
      std::getline(in, key);
    }
    return true;
  }
};

class Timer
{
  std::string timerName;
  long long t_start;
  TimeProfile* profile;

public:
  Timer(std::string name, TimeProfile* profile) :
  timerName(name), profile(profile), t_start(highResolutionClock())  { }

  virtual ~Timer()
  {
    if (profile)
    {
      const double lastTime = profile->getValue(timerName.c_str());
      const double currTime = (highResolutionClock() - t_start) /
        static_cast<double>(highResolutionClocksPerSecond());
      profile->setValue(timerName.c_str(), lastTime + currTime);
    }
  }

private:
  Timer(const Timer&);
  Timer& operator=(const Timer&);
};

#endif // #ifndef MINUTILS_TIMEPROFILE_H_INCLUDED
