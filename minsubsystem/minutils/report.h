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
#ifndef MINUTILS_REPORT_H_INCLUDED
#define MINUTILS_REPORT_H_INCLUDED

#include <algorithm>
#include <iomanip>
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <vector>
#include <ctime>

#ifdef _WIN32
  #include <direct.h> //_mkdir()
#endif

#include <minutils/residuals.h>
#include <minutils/timeprofile.h>
#include <minutils/pathname.h>

class ProcImageResult
{
public:
  std::string jpgName;
  std::string info;
  Residuals diffWithIdeal;
  TimeProfile timeProfile;

  ProcImageResult(const std::string& name = "", const std::string& _info = "")
  : jpgName(name), info(_info)  {}
  ProcImageResult(const ProcImageResult& other)
  : jpgName(other.jpgName), info(other.info),
    diffWithIdeal(other.diffWithIdeal),
    timeProfile(other.timeProfile) {}
  ProcImageResult& operator=(const ProcImageResult& other)
  {
    if (this != &other)
    {
      jpgName = other.jpgName;
      info = other.info;
      diffWithIdeal = other.diffWithIdeal;
      timeProfile = other.timeProfile;
    }
    return *this;
  }
};

namespace {

std::string getTime(std::string format = "%Y_%m_%d_%H_%M_%S")
{
  std::time_t rawtime;
  time(&rawtime);
  tm* timeinfo = localtime(&rawtime);
  char timebuffer[256] = {0};
  strftime(timebuffer, 256, format.c_str() , timeinfo);
  return std::string(timebuffer);
}

bool readReport(const std::string& fileName, std::vector<ProcImageResult>& res)
{
  std::fstream in(fileName.c_str());
  if (in.fail())
    return false;

  while (!in.fail())
  {
    std::string str;
    std::getline(in, str);
    if (str == "Residuals")
    {
      if (res.size() == 0 || !res.back().diffWithIdeal.isEmpty())
      {
        res.push_back(ProcImageResult("unknown"));
      }
      res.back().diffWithIdeal.read(in);
    }
    else if (str == "TimeProfile")
    {
      if (res.size() == 0 || !res.back().timeProfile.isEmpty())
      {
        res.push_back(ProcImageResult("unknown"));
      }
      res.back().timeProfile.read(in);
    }
    else if (str.length() > 0)
    {
      res.push_back(ProcImageResult(str));
      std::getline(in, res.back().info);
    }
  }
  in.close();
  return true;
}

std::string writeReport(const std::string& dir, std::vector<ProcImageResult>& res, bool create = true)
{
  std::string fileName = "";
  if (create)
  {
#ifdef _WIN32
    int status = _mkdir(dir.c_str());
#else
    int status = mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    fileName = dir + "/run_" + getTime() + ".log";
  }
  else
  {
    fileName = dir;
  }

  std::fstream out(fileName.c_str(), std::fstream::app | std::fstream::out);
  out.precision(5);
  out.setf(std::fstream::fixed, std::fstream::floatfield);
  if (out.fail())
    return "";

  for (unsigned int i = 0; i < res.size(); ++i)
  {
    out << res[i].jpgName << std::endl;
    out << res[i].info << std::endl;
    res[i].diffWithIdeal.write(out);
    res[i].timeProfile.write(out);
    out.flush();
  }
  out.close();
  return fileName;
}

std::string writeHtml(const std::string& runFileName, const std::string& pattern = "", bool withTime = true)
{
  std::vector<ProcImageResult> results;
  if (!readReport(runFileName, results))
    return "";

  se::pathname outName(runFileName.c_str());
  std::string dir = outName.folder;

  if (dir.empty())
    dir = "."; // portable??

  std::string report_time = outName.name.substr(4, outName.name.length() - 8);
  std::string htmfilename = dir + "/report_" + report_time;
  if (!pattern.empty())
    htmfilename += "_" + pattern;
  htmfilename += ".html";
  if (!withTime)
    htmfilename = dir + "/report_last.html";
  std::ofstream out(htmfilename.c_str());
  out.precision(5);
  out.setf(std::fstream::fixed, std::fstream::floatfield);
  if (!out)
    return "";

  int nResults = results.size();
  std::set<std::string> subsystems;
  for (int i = 0; i < nResults; ++i)
  {
    results[i].diffWithIdeal.updateSubsystems(subsystems, pattern);
  }

  std::vector<std::pair<double, ProcImageResult* > > reportData(nResults);
  double totalTime = 0;
  for (int i = 0; i < nResults; ++i)
  {
    reportData[i].second = &results[i];
    reportData[i].first = results[i].diffWithIdeal.getMax(subsystems);

    totalTime += results[i].timeProfile.getValue("Total");
  }
  std::sort(reportData.rbegin(), reportData.rend());
  double avg_totalTime = totalTime / static_cast<double>(nResults);

  out << "<html>" << std::endl;
  out << "<body>" << std::endl;
  out << "<span> Report from " << report_time << " </span> <br>";
  out << "<span> Based on " << runFileName << " </span> <br>";
  out << "<span> Configuration is ";
#ifdef _DEBUG
  out << "Debug";
#else
  out << "Release";
#endif

  out << " </span><br>";
  out << "<span> Total time " << totalTime <<
    ".<br> Average time " << avg_totalTime <<
    "</span>" << std::endl;

  out << "<table border=\"1\" cellpadding=\"20\">" << std::endl;
  out << "<tr>" << std::endl;
  out << "<th> 1. </th>" << std::endl;
  out << "<th> 2. Image </th>" << std::endl;
  out << "<th> 3. Time </th>" << std::endl;
  int row_count = 4;
  std::set<std::string>::iterator it;
  for (it = subsystems.begin(); it != subsystems.end(); ++it, ++row_count)
  {
    out << "<th> " << row_count << ". " << *it << "</th>" << std::endl;
  }
  --row_count;
  // out<< "</tr>" << std::endl;

  // out << "<tr>" << std::endl;
  // std::map<std::string, double > avg_residuals;
  // for (std::set<std::string>::iterator it = subsystems.begin();
  //   it != subsystems.end(); ++it)
  // {
  //   for (int i = 0; i < nResults; ++i)
  //   {
  //     avg_residuals[*it] += results[i].diffWithIdeal.getValue((*it).c_str());
  //   }
  //   avg_residuals[*it] /= nResults;
  // }

  // out << "<th> </th> <th> Average </th>" << std::endl;
  // out << "<th>" << avg_totalTime << "</th>" << std::endl;

  // for (std::set<std::string>::iterator it = subsystems.begin();
 //      it != subsystems.end(); ++it)
 //  {
  //   out << "<th>" << avg_residuals[*it] << "</th>" << std::endl;
 //  }
 //  out<< "</tr>" << std::endl;

  int col_count = 1;
  for (int i = 0; i < nResults; ++i, ++col_count)
  {
    if ((i > 0) && (i % 10 == 0))
    {
      out << "<tr style=\"background-color:#E0E0E0;text-align: center;\">" << std::endl;
      out << "<th> 1. </th>" << std::endl;
      out << "<th> 2. Image </th>" << std::endl;
      out << "<th> 3. Time </th>" << std::endl;
      row_count = 4;
      for (it = subsystems.begin(); it != subsystems.end(); ++it, ++row_count)
      {
        out << "<th> " << row_count << ". " << *it << "</th>" << std::endl;
      }
      out << "</tr>" << std::endl;
    }
    out << "<tr>" << std::endl;
    out << "<td> " << col_count << " </td>" << std::endl;
    ProcImageResult& res = *reportData[i].second;
    se::pathname jpgname(res.info.c_str());
//    std::string ref_svg = jpgname.path + ".out/lines.svg";
    std::string ref_svg =  res.info;
    out << "\t<td><a href=\"" << ref_svg << "\">" << std::endl;
    out << res.jpgName << std::endl;
    out << "\t</a></td>\t";
    out << "<td>" << results[i].timeProfile.getValue("Total") << "</td>" << std::endl;
    for (it = subsystems.begin(); it != subsystems.end(); ++it)
    {
      out << "<td> " << res.diffWithIdeal.getValue((*it).c_str()) << " </td>" << std::endl;
    }

    out << "</tr>" << std::endl;
  }
  out << "</table>" << std::endl;
  out << "<br><hr /><br>" << std::endl;

  std::set<std::string> time_subsystems;
  for (int i = 0; i < nResults; ++i)
  {
    results[i].timeProfile.updateSubsystems(time_subsystems, pattern);
  }
  if(time_subsystems.empty())
    return htmfilename;
  std::vector<std::pair<double, std::string> > avg_times(time_subsystems.size());
  int cur = 0;
  for (std::set<std::string>::iterator it = time_subsystems.begin();
    it != time_subsystems.end(); ++it, ++cur)
  {
    avg_times[cur] = std::make_pair(0, *it);
    for (int i = 0; i < nResults; ++i)
    {
      avg_times[cur].first += results[i].timeProfile.getValue((*it).c_str());
    }
    avg_times[cur].first /= nResults;
  }
  std::sort(avg_times.rbegin(), avg_times.rend());

  std::vector<std::pair<double, ProcImageResult*> > timeReport(nResults);
  for (int i = 0; i < nResults; ++i)
  {
    timeReport[i].second = &results[i];
    timeReport[i].first = results[i].timeProfile.getValue("Total");
  }
  std::sort(timeReport.rbegin(), timeReport.rend());

  out << "<table border=\"1\" cellpadding=\"20\">" << std::endl;
  out << "<tr>" << std::endl;
  out << "<th> 1. </th>" << std::endl;
  out << "<th> 2. Image </th>" << std::endl;
  row_count = 3;
  for (std::vector<std::pair<double, std::string> >::iterator it = avg_times.begin();
      it != avg_times.end(); ++it, ++row_count)
  {
    out << "<th> " << row_count << ". " << it->second << "</th>" << std::endl;
  }
  out << "<th> " << row_count << ". Something else" << "</th>" << std::endl;
  out << "</tr>" << std::endl;
  out << "<tr>" << std::endl;
  out << "<th> </th> <th> Average time </th>" << std::endl;
  double cur_counted = 0;
  for (std::vector<std::pair<double, std::string> >::iterator it = avg_times.begin();
      it != avg_times.end(); ++it, ++row_count)
  {
    out << "<th>" << it->first << " (" << it->first * 100 / avg_totalTime << "%) </th>" << std::endl;
    cur_counted += it->first;
  }
  if (cur_counted > avg_totalTime)
    cur_counted -= avg_totalTime;
  cur_counted = avg_totalTime - cur_counted;
  out << "<th> " << cur_counted  << " (" <<
    cur_counted * 100 / avg_totalTime << "%) </th>" << std::endl;
  --row_count;
  out<< "</tr>" << std::endl;
  col_count = 1;

  for (int i = 0; i < nResults; ++i, ++col_count)
  {
    if ((i > 0) && (i % 10 == 0))
    {
      out << "<tr style=\"background-color:#E0E0E0;text-align: center;\">" << std::endl;
      out << "<th> 1. </th>" << std::endl;
      out << "<th> 2. Image </th>" << std::endl;
      row_count = 3;
      for (std::vector<std::pair<double, std::string> >::iterator it = avg_times.begin();
          it != avg_times.end(); ++it, ++row_count)
      {
        out << "<th> " << row_count << ". " << it->second << "</th>" << std::endl;
      }
      out << "<th> " << row_count << ". Something else" << "</th>" << std::endl;
      out << "</tr>" << std::endl;
    }
    out << "<tr>" << std::endl;
    out << "<td> " << col_count << " </td>" << std::endl;
    ProcImageResult& res = *timeReport[i].second;
    se::pathname jpgname(res.info.c_str());
//    std::string ref_svg = jpgname.path + ".out/lines.svg";
    std::string ref_svg =  res.info;
    out << "\t<td><a href=\"" << ref_svg << "\">" << std::endl;
    out << res.jpgName << std::endl;
    out << "\t</a></td>\t";
    double cur_total = res.timeProfile.getValue("Total");
    cur_counted = 0;
    for (std::vector<std::pair<double, std::string> >::iterator it = avg_times.begin();
          it != avg_times.end(); ++it, ++row_count)
    {
      double cur_subtime = res.timeProfile.getValue((it->second).c_str());
      cur_counted += cur_subtime;
      out << "<td> " << cur_subtime << " (" <<
        cur_subtime * 100 / cur_total << "%) </td>" << std::endl;
    }
    if (cur_counted > cur_total)
      cur_counted -= cur_total;
    cur_counted = cur_total - cur_counted;
    out << "<td> " << cur_counted  << " (" <<
      cur_counted * 100 / cur_total << "%) </td>" << std::endl;
    out << "</tr>" << std::endl;
  }
  out << "</table>" << std::endl;
  out << "</body>" << std::endl;
  out << "</html>" << std::endl;

  return htmfilename;
}

} // namespace {

#endif // #ifndef MINUTILS_REPORT_H_INCLUDED
