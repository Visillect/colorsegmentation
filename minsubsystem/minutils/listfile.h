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
 * @file   listfile.h
 * @brief  listfile (.lst) support and batch processing template
 */

#pragma once
#ifndef MINUTILS_LISTFILE_H_INCLUDED
#define MINUTILS_LISTFILE_H_INCLUDED

#define LISTFILE_VERSION 5

#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <minutils/pathname.h>
#include <minstopwatch/hiresclock.h>
#include <minbase/mintyp.h>
#include <minbase/minresult.h>

#include <iostream>
#include <fstream>
#include <sstream>

namespace se {

inline std::string substrex(
    std::string s,
    std::string::size_type _Off = 0,
    std::string::size_type _Count = std::string::npos)
{
  if (_Off == std::string::npos)
    return std::string();
  return s.substr(_Off, _Count);
}

inline
std::string joinPath(
  const std::string &sPart1,
  const std::string &sPart2)
{
  std::string sRes = sPart1;
  if (!sRes.empty()
    && (sRes[sRes.length() - 1] != '/')
    && (sRes[sRes.length() - 1] != '\\')
    && !sPart2.empty()
  )
    sRes += "/";
  sRes += sPart2;
  return sRes;
}

inline
std::string joinPath(
  const std::string &sPart1,
  const std::string &sPart2,
  const std::string &sPart3)
{
  return joinPath(joinPath(sPart1, sPart2), sPart3);
}

inline
std::string joinPath(
  const std::string &sPart1,
  const std::string &sPart2,
  const std::string &sPart3,
  const std::string &sPart4)
{
  return joinPath(joinPath(sPart1, sPart2, sPart3), sPart4);
}

inline int32_t roundReal(real64_t val) {
  return static_cast<int32_t>((val > 0) ? (val + .5) : (val - .5));
}

class GenUniqueInt
{
public:
  GenUniqueInt(int startFrom = 0)
    : curNum(startFrom - 1)
  {}
  int generate() { return ++curNum; }
  int getLast() { return curNum; }
private:
  int curNum;
};

class CommonValue
{
public:
  enum Type {TP_DEFAULT, TP_INT, TP_REAL, TP_STR, TP_BOOL};

  CommonValue()
    : tp(TP_DEFAULT), vlInt(0), vlReal(0), vlBool(false)
  {}
  CommonValue(int32_t vl)
    : tp(TP_INT), vlInt(vl), vlReal(0), vlBool(false)
  {}
  CommonValue(real64_t vl)
    : tp(TP_REAL), vlInt(0), vlReal(vl), vlBool(false)
  {}
  CommonValue(const char *vl)
    : tp(TP_STR), vlInt(0), vlReal(0), vlStr(vl), vlBool(false)
  {}
  CommonValue(bool vl)
    : tp(TP_BOOL), vlInt(0), vlReal(0), vlBool(vl)
  {}
  CommonValue(const CommonValue &cv)
    : tp(cv.tp), vlInt(cv.vlInt), vlReal(cv.vlReal), vlStr(cv.vlStr), vlBool(cv.vlBool)
  {}
  CommonValue &operator=(const CommonValue &r) {
    tp = r.tp;
    vlInt = r.vlInt;
    vlReal = r.vlReal;
    vlStr = r.vlStr;
    vlBool = r.vlBool;
    return *this;
  }
  friend bool operator==(const CommonValue &l, const CommonValue &r) {
    if (l.tp != r.tp)
      return false;
    if (l.tp == TP_INT)
      return l.vlInt == r.vlInt;
    if (l.tp == TP_REAL)
      return l.vlReal == r.vlReal;
    if (l.tp == TP_BOOL)
      return l.vlBool == r.vlBool;
    if (l.tp == TP_STR)
      return l.vlStr == r.vlStr;
    if (l.tp == TP_DEFAULT)
      return true;
    return false;
  }
  friend bool operator!=(const CommonValue &l, const CommonValue &r) {
    return !(l == r);
  }

  friend CommonValue from_str(const char *vl);
  int32_t to_int() const {
    switch(tp) {
      case TP_INT:
        return vlInt;
      case TP_REAL:
        return roundReal(vlReal);
      case TP_BOOL:
        return static_cast<int32_t>(vlBool);
      default:
        return 0;
    }
  }
  real64_t to_real() const {
    switch(tp) {
      case TP_INT:
        return vlInt;
      case TP_REAL:
        return vlReal;
      case TP_BOOL:
        return vlBool ? 1. : 0;
      default:
        return 0;
    }
    return 0;
  }
  const char *to_str() const {
    std::ostringstream oss;
    switch(tp) {
      case TP_INT:
        oss << vlInt;
        const_cast<CommonValue*>(this)->vlStr = oss.str();
        return vlStr.c_str();
      case TP_REAL:
        oss << vlReal;
        const_cast<CommonValue*>(this)->vlStr = oss.str();
        return vlStr.c_str();
      case TP_STR:
        return vlStr.c_str();
      default:
        const_cast<CommonValue*>(this)->vlStr = std::string();
        return vlStr.c_str();
    }
    return vlStr.c_str();
  }
  bool to_bool() const {
    switch(tp) {
      case TP_INT:
        return vlInt != 0;
      case TP_REAL:
        return vlReal != 0;
      case TP_STR:
        return false;
      case TP_BOOL:
        return vlBool;
      default:
        return false;
    }
    return false;
  }
  Type valType() { return tp; }
  bool empty() { return tp == TP_DEFAULT; }
protected:
  Type tp;
  int32_t vlInt;
  real64_t vlReal;
  std::string vlStr;
  bool vlBool;
};

inline
CommonValue from_str(const char *vl) {
  CommonValue cv;
  if (!vl || !*vl)
    return cv;
  using std::string;
  if (!strcmp(vl, "false")) {
    cv.tp = CommonValue::TP_BOOL;
    cv.vlBool = false;
  } else if (!strcmp(vl, "true")) {
    cv.tp = CommonValue::TP_BOOL;
    cv.vlBool = true;
  } else if (vl[0] == '"')
  {
    cv.tp = CommonValue::TP_STR;
    cv.vlStr = vl + 1;
    if (cv.vlStr[cv.vlStr.length() - 1] == '"')
      cv.vlStr.resize(cv.vlStr.length() - 1);
  } else
  {
    string sVal(vl);
    if (sVal.find_first_not_of("0123456789+-.") == string::npos)
    {
      std::istringstream iss(sVal);
      if (sVal.find_first_of('.') != string::npos)
      {
        cv.tp = CommonValue::TP_REAL;
        iss >> cv.vlReal;
      } else
      {
        cv.tp = CommonValue::TP_INT;
        iss >> cv.vlInt;
      }
    } else
    {
      cv.tp = CommonValue::TP_STR;
      cv.vlStr = sVal;
    }
  }
  return cv;
}

typedef std::map<std::string, CommonValue> VarSetT;

inline
int extractIntVariable(const VarSetT &varset, const char *pszName, int32_t defaultValue)
{
  VarSetT::const_iterator it = varset.find(pszName);
  if (it != varset.end())
    return it->second.to_int();
  else
    return defaultValue;
}

inline
real64_t extractRealVariable(const VarSetT &varset, const char *pszName, real64_t defaultValue)
{
  VarSetT::const_iterator it = varset.find(pszName);
  if (it != varset.end())
    return it->second.to_real();
  else
    return defaultValue;
}

inline
int extractBoolVariable(const VarSetT &varset, const char *pszName, bool defaultValue)
{
  VarSetT::const_iterator it = varset.find(pszName);
  if (it != varset.end())
    return it->second.to_bool();
  else
    return defaultValue;
}

inline
const char *extractStrVariable(const VarSetT &varset, const char *pszName, const char *pszDefaultValue)
{
  VarSetT::const_iterator it = varset.find(pszName);
  if (it != varset.end())
    return it->second.to_str();
  else
    return pszDefaultValue;
}

class proc_list_data
{
public:
  int files_ok;
  int files_failed;
  double last_file_seconds;
  double list_seconds;

  int total_files_ok;     //
  int total_files_failed; //  with included
  double total_seconds;   //

  int cur_list_done; // [done/total] to show progress on current list
  int cur_list_total; // [1/156]...[156/156] == [done/total]

  std::string margin; // for nested list output formatting

  std::string sImgRootFolder;
  std::string sListFilePathName;
  std::string sReportRootFolder;
  std::string sSubfolder;   // relatively to root list file
  std::string sIdealRootFolder;

  bool bStopped, bBreak;

  std::vector<VarSetT> variables;

  GenUniqueInt *pClipNumGenerator;
  bool bOwnClipNumGenerator;

  std::ostream *pOsFileListReport;
  std::vector<std::string> *pFileListContainer;
  bool bDoNotRecog;

  // todo: extend - time of processing; first and last negative retcodes etc.
//  int argc;
//  char** argv;
  int flags;

  proc_list_data(int _flags=0):
      flags(_flags), files_ok(0), files_failed(0),
      total_files_ok(0), total_files_failed(0),
      last_file_seconds(0), list_seconds(0), total_seconds(0),
      cur_list_done(0), cur_list_total(0), bStopped(false), bBreak(false), variables(1),
      pOsFileListReport(NULL),
      pFileListContainer(NULL),
      bDoNotRecog(false),
      pClipNumGenerator(new GenUniqueInt(1)),
      bOwnClipNumGenerator(true)
    {}
  proc_list_data(
    const proc_list_data &parentPLD,
    const char *pszImgFolder,
    const char *pszLstPathName) :
      flags(parentPLD.flags), files_ok(0), files_failed(0),
      total_files_ok(0), total_files_failed(0),
      last_file_seconds(0), list_seconds(0), total_seconds(0),
      cur_list_done(0), cur_list_total(0), bStopped(false), bBreak(false),
      variables(parentPLD.variables),
      bDoNotRecog(parentPLD.bDoNotRecog),
      pClipNumGenerator(parentPLD.pClipNumGenerator),
      bOwnClipNumGenerator(false),
      sImgRootFolder(parentPLD.sImgRootFolder + "/" + pszImgFolder),
      sReportRootFolder(parentPLD.sReportRootFolder + "/" + pszImgFolder),
      sListFilePathName(pszLstPathName), margin(parentPLD.margin + "\t"),
      pOsFileListReport(parentPLD.pOsFileListReport),
      pFileListContainer(parentPLD.pFileListContainer),
      sSubfolder(parentPLD.sSubfolder.empty() ? pszImgFolder : parentPLD.sSubfolder + "/" + pszImgFolder),
      sIdealRootFolder(parentPLD.sIdealRootFolder)
    {}
  ~proc_list_data() {
    if (bOwnClipNumGenerator && pClipNumGenerator)
      delete pClipNumGenerator;
  }
};

class ProcFile
{
public:
  class RunParams
  {
  public:
    RunParams(const VarSetT &v)
      : pszFilePathName(NULL), pszSymmetricPathFromLst(NULL),
      pszLstFolder(NULL), pszLstPathName(NULL), pszRelativePath(NULL), flags(0), vars(v) {}
    const char *pszFilePathName;
    const char *pszSymmetricPathFromLst;
    const char *pszIdealFolder;
    const char *pszReportRoot;
    const char *pszLstFolder;
    const char *pszLstPathName;
    const char *pszRelativePath;
    int flags;
    const VarSetT &vars;
  };

  int processFile(proc_list_data& pld, const pathname &fileName)
  {
    using std::string;
    string ifile = fileName.is_relative() ? pld.sImgRootFolder + '/' + fileName.path : fileName.path;
    pathname pathListFile = pld.sListFilePathName.c_str();
    string file_folder = (fileName.folder.empty() ? "" : "/" + fileName.folder);
    string sSymmetricPathFromLst = pathListFile.folder + file_folder;
    RunParams params(pld.variables.back());
    params.pszFilePathName = ifile.c_str();
    params.pszSymmetricPathFromLst = sSymmetricPathFromLst.c_str();
    params.pszLstFolder = pathListFile.folder.c_str();
    params.pszLstPathName = pathListFile.path.c_str();
    params.pszReportRoot = pld.sReportRootFolder.c_str();
    string sRelativePath = pld.sSubfolder.empty() ? fileName.path : pld.sSubfolder + "/" + fileName.path;
    params.pszRelativePath = sRelativePath.c_str();
    string sIdealFolder = pld.sIdealRootFolder.empty() ?
      sSymmetricPathFromLst
      : joinPath(pld.sIdealRootFolder, pld.sSubfolder + file_folder);
    params.pszIdealFolder = sIdealFolder.c_str();
    params.flags = pld.flags;
    long long t_start_item = highResolutionClock();
    int res = runAndReport(
      params,
      pld.pOsFileListReport,
      pld.pFileListContainer,
      pld.bDoNotRecog);
    pld.last_file_seconds = (highResolutionClock()-t_start_item)/double( highResolutionClocksPerSecond() );
    pld.list_seconds += pld.last_file_seconds;
    pld.total_seconds += pld.last_file_seconds;
    if (res < 0) {
      pld.files_failed++;
      pld.total_files_failed++;
    } else {
      pld.files_ok++;
      pld.total_files_ok++;
    }
    return res;
  }

  virtual int run(const RunParams &params)
  {
    return NO_ERRORS;
  }
  virtual int finish(const VarSetT &vars, const char *pszLstPath) {
    return NO_ERRORS;
  }
  int runAndReport(const RunParams &params,
    std::ostream *pOs,
    std::vector<std::string> *pListFileContainer,
    bool bDoNotRecog)
  {
    if (pOs)
      (*pOs) << params.pszFilePathName << std::endl;
    if (pListFileContainer)
      pListFileContainer->push_back(params.pszFilePathName);
    if (!bDoNotRecog)
      return run(params);
    else
      return NO_ERRORS;
  }
};

inline
std::string preprocCmdString(const char *pszCmdString) {
  std::string sTmp = substrex(pszCmdString, 0, std::string(pszCmdString).find_first_of(';'));
  sTmp = substrex(sTmp, 0, sTmp.find_last_not_of("\n\r") + 1);
  return substrex(sTmp, 0, sTmp.find_last_not_of(' ') + 1);
}

class ListFileCommand
{
public:
  ListFileCommand() {}
  virtual ~ListFileCommand() {}
  virtual int execute(ProcFile &procFile, proc_list_data& pld) const = 0;
  virtual void writeToConsole(proc_list_data& pld) const {}
  virtual const char *statement() const =0;
  virtual ListFileCommand *construct() const =0;
  void preprocAndParseCmdString(const char *pszCmdString) {
    parseCmdString(preprocCmdString(pszCmdString).c_str());
  }
  virtual void parseCmdString(const char *pszCmdString) {
    std::string sParams(pszCmdString);
    if (statement())
      sParams = substrex(sParams, sParams.find_first_not_of(' ', 1 + strlen(statement())));
    parseCmdParams(sParams.c_str());
  }
  ListFileCommand *constructFromString(const char *pszCmdString) const {
    ListFileCommand *pCmd = construct();
    pCmd->preprocAndParseCmdString(pszCmdString);
    return pCmd;
  }
  virtual void parseCmdParams(const char *pszCmdParams) {}
};

class LFCProcessFile : public ListFileCommand
{
public:
  LFCProcessFile() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    return procFile.processFile(pld, fileName);
  }
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << fileName.path << "...\n";
  }
  LFCProcessFile *construct() const { return new LFCProcessFile; }
  const char *statement() const { return NULL; }
  void parseCmdParams(const char *pszCmdParams) {
    fileName.parse(pszCmdParams);
  }
protected:
  pathname fileName;
};

class LFCStop : public ListFileCommand
{
public:
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    pld.bStopped = true;
    return NO_ERRORS;
  }
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << "Processing stopped\n";
  }
  ListFileCommand *construct() const { return new LFCStop; }
  const char *statement() const { return "stop"; }
};

class LFCBreak : public ListFileCommand
{
public:
  LFCBreak() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    pld.bBreak = true;
    return NO_ERRORS;
  }
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << "Breaking lst file\n";
  }
  ListFileCommand *construct() const { return new LFCBreak; }
  const char *statement() const { return "break"; }
};

const char CLIPNUMBER_VAR_NAME[] = "clipNumber";

#if 0
class LFCStartClip : public ListFileCommand
{
public:
  LFCStartClip() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    VarSetT::iterator it = pld.variables.back().find(CLIPNUMBER_VAR_NAME);
    if (it != pld.variables.back().end())
      it->second = CommonValue(pld.pClipNumGenerator->generate());
    else
      pld.variables.back()[CLIPNUMBER_VAR_NAME] = pld.pClipNumGenerator->generate();  // zero value of clipNumber means there is no clip
    return NO_ERRORS;
  }
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << "Starting new clip\n";
  }
  ListFileCommand *construct() const { return new LFCStartClip; }
  const char *statement() const { return "start_clip"; }
};

class LFCFinishClip : public ListFileCommand
{
public:
  LFCFinishClip() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    pld.variables.back()[CLIPNUMBER_VAR_NAME] = CommonValue(static_cast<int32_t>(0));
    return NO_ERRORS;
  }
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << "Clip finished\n";
  }
  ListFileCommand *construct() const { return new LFCFinishClip; }
  const char *statement() const { return "finish_clip"; }
};
#endif

class LFCInclude : public ListFileCommand
{
public:
  LFCInclude() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const;
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << "Including " << fileName.path << "\n";
  }
  ListFileCommand *construct() const { return new LFCInclude; }
  const char *statement() const { return "include"; }
  virtual void parseCmdString(const char *pszCmdString) {
    std::string sParams(pszCmdString);
    if ((pszCmdString[0] == '#') && statement())
      sParams = substrex(sParams, sParams.find_first_not_of(' ', 1 + strlen(statement())));
    parseCmdParams(sParams.c_str());
  }
  void parseCmdParams(const char *pszCmdParams) {
    fileName.parse(pszCmdParams);
  }
protected:
  pathname fileName;
};

class LFCClip : public ListFileCommand
{
public:
  LFCClip() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const;
  virtual void writeToConsole(proc_list_data& pld) const {
    std::cout << pld.margin << "Including clip " << fileName.path << "\n";
  }
  ListFileCommand *construct() const { return new LFCClip; }
  const char *statement() const { return "clip"; }
  void parseCmdParams(const char *pszCmdParams) {
    fileName.parse(pszCmdParams);
  }
protected:
  pathname fileName;
};

class LFCEcho : public ListFileCommand
{
public:
  LFCEcho() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    std::cerr << pld.margin << sText << '\n';
    return NO_ERRORS;
  }
  ListFileCommand *construct() const { return new LFCEcho; }
  const char *statement() const { return "echo"; }
  void parseCmdParams(const char *pszCmdParams) {
    sText = pszCmdParams;
  }
protected:
  std::string sText;
};

class LFCSet : public ListFileCommand
{
public:
  LFCSet() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    for (VarSetT::const_iterator it = variables.begin(); it != variables.end(); it++)
      pld.variables.back()[it->first] = it->second;
    return NO_ERRORS;
  }
  ListFileCommand *construct() const { return new LFCSet; }
  const char *statement() const { return "set"; }
  void parseCmdParams(const char *pszCmdParams) {
    using std::string;
    string sParams(pszCmdParams);
    int startPos = 0;
    while(startPos != string::npos) {
      int endPos = 0;
      string sVarName = substrex(sParams, startPos, (endPos = sParams.find_first_of(" =", startPos)) - startPos);
      startPos = endPos;
      startPos = sParams.find_first_not_of(' ', startPos);
      string sVarValue;
      startPos++;
      if( (startPos != string::npos) && (sParams[startPos] == '"'))
        sVarValue = substrex(sParams, startPos, (endPos = sParams.find_first_of('"', startPos + 1) + 1) - startPos);
      else
        sVarValue = substrex(sParams, startPos, (endPos = sParams.find_first_of(" \t", startPos)) - startPos);
      variables[sVarName] = from_str(sVarValue.c_str());
      startPos = endPos;
      startPos = sParams.find_first_not_of(" \t", startPos);
    }
  }
protected:
  VarSetT variables;
};

class LFCReset : public ListFileCommand
{
public:
  LFCReset() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    pld.variables.clear();
    return NO_ERRORS;
  }
  ListFileCommand *construct() const { return new LFCReset; }
  const char *statement() const { return "reset"; }
};

class LFCPush : public ListFileCommand
{
public:
  LFCPush() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    pld.variables.push_back(VarSetT());
    std::vector<VarSetT>::iterator last = pld.variables.end();
    last--;
    std::vector<VarSetT>::iterator lastButOne = last;
    lastButOne--;
    *last = *lastButOne;
    return NO_ERRORS;
  }
  ListFileCommand *construct() const { return new LFCPush; }
  const char *statement() const { return "push"; }
};

class LFCPop : public ListFileCommand
{
public:
  LFCPop() {}
  int execute(ProcFile &procFile, proc_list_data& pld) const {
    pld.variables.pop_back();
    return NO_ERRORS;
  }
  ListFileCommand *construct() const { return new LFCPop; }
  const char *statement() const { return "pop"; }
};

class listfile // list of terminal filenames of nested listfiles names
               // NOTE: filenames may be relative or absolute
               // in case of relative filename it's path computed from the listfile location
{
//  std::vector< std::string > filenames;
  std::vector<ListFileCommand*> commandList;
  typedef std::map<std::string, ListFileCommand*> CommandRegistry;
  CommandRegistry commandRegistry;
public:
  listfile( const char* listfilename )
  {
    using std::string;

    string line;
//    filenames.clear();
    registerCommands();
    std::ifstream infile(listfilename, std::ios_base::in);
    if (!infile)
      std::cout << "Error when open listfile:" << listfilename << std::endl;
    while (getline(infile, line, '\n'))
    {
      string sLine = preprocCmdString(line.c_str());
      if (sLine.empty()) // skip empty and commented lines
        continue;
//      if (line.[line.length() - 1] == '\r')
//        line.erase(line.length() - 1);

      string sCmdName;
      if (line[0] == '#')
        sCmdName = line.substr(1, line.find_first_of(' ') - 1);
      ListFileCommand *pCmd = NULL;
      if (sCmdName.empty())
      {
        pathname filePathName = line.c_str();
        if (filePathName.ext == "lst")
          pCmd = new LFCInclude;
        else
          pCmd = new LFCProcessFile;
      } else
      {
        CommandRegistry::iterator itCmd = commandRegistry.find(sCmdName);
        if (itCmd != commandRegistry.end())
          pCmd = itCmd->second->construct();
      }
      if (pCmd)
      {
        pCmd->preprocAndParseCmdString(line.c_str());
        commandList.push_back(pCmd);
      }
//      if (line == "#stop") // ignore rest of list
//        break;
//      filenames.push_back (line);
    }
  }
  ~listfile() {
    using std::vector;
    using std::map;
    for (vector<ListFileCommand*>::iterator it = commandList.begin(); it != commandList.end(); it++)
      if (*it)
        delete *it;
    for (CommandRegistry::iterator it = commandRegistry.begin(); it != commandRegistry.end(); it++)
      if (it->second)
        delete it->second;
  }
  void registerCommands() {
    using std::vector;
    vector<ListFileCommand*> cmds;
    cmds.push_back(new LFCProcessFile);
    cmds.push_back(new LFCStop);
    cmds.push_back(new LFCBreak);
    cmds.push_back(new LFCInclude);
    cmds.push_back(new LFCEcho);
    cmds.push_back(new LFCSet);
    cmds.push_back(new LFCReset);
    cmds.push_back(new LFCPush);
    cmds.push_back(new LFCPop);
    cmds.push_back(new LFCClip);
    for (vector<ListFileCommand*>::iterator it = cmds.begin(); it != cmds.end(); it++)
      if ((*it)->statement())
        commandRegistry[(*it)->statement()] = *it;
      else
        commandRegistry[""] = *it;
  }
  int size() { return commandList.size(); }
  const ListFileCommand *operator [] ( int i ) { return commandList[i]; }

}; // class listfile

// proc_list_file flags
const int plf_verbose = 0x001; // if set uses std::cout for output
const int plf_stop_if_result_less_zero = 0x002; // stop processing if returned code less zero


//template <class procfile>  // custom p_procfile->run() called for terminal filenames
inline
int proc_list_file( // recursively parses listfile and nested lists, calls procfile.run()
  const char* listfile_or_terminal_file_name, // .lst or other (say .jpg)
  proc_list_data& pld,
  ProcFile* p_procfile )  // p_procfile->run()
{
//  long long t_start = highResolutionClock();

  pathname fn( listfile_or_terminal_file_name ); // "C:\\ququ\\file.ext" => "C:\\ququ" "file.ext" ...
  pld.sListFilePathName = fn.path;
  if (pld.sImgRootFolder.empty())
    pld.sImgRootFolder = fn.folder;
  if (pld.sReportRootFolder.empty())
    pld.sReportRootFolder = pld.sImgRootFolder;

  int res=0;
  if (fn.ext == "lst")
  {
    listfile lf( fn.path.c_str() );
    if (pld.flags & plf_verbose)
    {
      std::cout << pld.margin << "=========================================\n";
      std::cout << pld.margin << "== run list file '" << fn.name << "' of " << lf.size() << " items: \n";
      // full name... std::cout << listfile_or_terminal_file_name << "\n";
    }

//    pld.margin.push_back('\t');

    for (int i=0; i<lf.size(); i++)
    {
      if (pld.bStopped || pld.bBreak)
        break;
      pld.cur_list_done = i+1;
      pld.cur_list_total = lf.size();
//      pathname fn_i( lf[i].c_str() );
      const ListFileCommand *pCmd = lf[i];
      if (!pCmd)
        continue;
//      std::string ifile = fn_i.is_relative() ? fn.folder + '/' + lf[i] : lf[i];
      if (pld.flags & plf_verbose)
      {
        std::cout << pld.margin << "{{{ list item " << pld.cur_list_done << " of " << pld.cur_list_total << "\n";
        pCmd->writeToConsole(pld);
//        std::cout << pld.margin << ((fn_i.ext == "lst") ? ifile : lf[i]) << "...\n";
      }
//      long long t_start_item = highResolutionClock();
      if (pCmd->execute(*p_procfile, pld) < 0)
        std::cout << "Command execution failed\n";
//      res = proc_list_file<procfile>( ifile.c_str(), pld, p_procfile );


      if (pld.flags & plf_verbose)
      {
//        double sec = (highResolutionClock()-t_start_item)/double( highResolutionClocksPerSecond() );
        double ave = pld.files_ok+pld.files_failed > 0 ? pld.list_seconds / (pld.files_ok+pld.files_failed) : pld.list_seconds;
        std::cout << pld.margin << "...finished in "<< pld.last_file_seconds  <<" sec; retcode=" << res << "\n";
        std::cout << pld.margin << "subtotal: files_ok=" << pld.files_ok << " files_failed=" << pld.files_failed << "\n";
        std::cout << pld.margin << "average time: " << ave << " sec\n";
        std::cout << pld.margin << "}}}\n";
      }

      if (res < 0 && ( pld.flags & plf_stop_if_result_less_zero))
        break;
    } //for (int i=0; i<lf.size(); i++)
    p_procfile->finish(pld.variables.back(), fn.folder.c_str());

    if (pld.margin.length() > 0) // pop_back()
      pld.margin.resize( pld.margin.length()-1 );
      //pld.margin=pld.margin.substr( 0, pld.margin.length()-1 );

    if (pld.flags & plf_verbose)
    {
  //    double sec = (highResolutionClock()-t_start)/double( highResolutionClocksPerSecond() );
      double ave = pld.files_ok+pld.files_failed > 0 ? pld.list_seconds / (pld.files_ok+pld.files_failed) : pld.list_seconds;
      std::cout << pld.margin << "== end of list file '" << fn.name << "'\n";
      std::cout << pld.margin << "== files_ok=" << pld.files_ok << " files_failed=" << pld.files_failed << "\n";
      std::cout << pld.margin << "== average time=" << ave << " sec\n";
      std::cout << pld.margin << "=========================================\n";
    }
  }
  else // not nested .lst file - let process item
  {

    return INTERNAL_ERROR;
 }

  return res;
}

inline
int LFCInclude::execute(ProcFile &procFile, proc_list_data& pld) const {
  pathname pathListFile = pld.sListFilePathName.c_str();
  pathname ifile = (fileName.is_relative() ? pathListFile.folder + '/' + fileName.path : fileName.path).c_str();
  proc_list_data newPld(pld, fileName.folder.c_str(), ifile.path.c_str());
/*  newPld.variables = pld.variables;
  newPld.sImgRootFolder = pld.sImgRootFolder + "/" + fileName.folder;
  newPld.sListFileFolder = ifile.folder;
  newPld.margin = pld.margin + "\t";
  newPld.bDoNotRecog = pld.bDoNotRecog;
  newPld.pOsFileListReport = pld.pOsFileListReport;*/
  int res = proc_list_file(ifile.path.c_str(), newPld, &procFile);
  if (newPld.bStopped)
    pld.bStopped = true;
  pld.variables = newPld.variables;
  pld.total_files_ok += newPld.total_files_ok;
  pld.total_files_failed += newPld.total_files_failed;
  pld.total_seconds += newPld.total_seconds;
  return res;
}

inline
int LFCClip::execute(ProcFile &procFile, proc_list_data& pld) const {
  if (fileName.ext == "lst")
  {
    pathname pathListFile = pld.sListFilePathName.c_str();
    pathname ifile((fileName.is_relative() ? pathListFile.folder + '/' + fileName.path : fileName.path).c_str());
    CommonValue &clipNumber = pld.variables.back()[CLIPNUMBER_VAR_NAME];
    bool bClipStarted = true;
    if (!clipNumber.empty()
      && (clipNumber != CommonValue(static_cast<int32_t>(0))))
    {
      std::cerr << "Warning: nested #clip - iterpreting as #include\n";
      bClipStarted = false;
    } else
      clipNumber = pld.pClipNumGenerator->generate();
    proc_list_data newPld(pld, fileName.folder.c_str(), ifile.path.c_str());
    int res = proc_list_file(ifile.path.c_str(), newPld, &procFile);
    if (newPld.bStopped)
      pld.bStopped = true;
    pld.variables = newPld.variables;
    pld.total_files_ok += newPld.total_files_ok;
    pld.total_files_failed += newPld.total_files_failed;
    pld.total_seconds += newPld.total_seconds;
    if (bClipStarted)
      pld.variables.back()[CLIPNUMBER_VAR_NAME] = CommonValue(static_cast<int32_t>(0));
    return res;
  } else // not a list considered as picture
  {
    return procFile.processFile(pld, fileName);
  }
  return INTERNAL_ERROR;
}

}; // namespace se::

#endif // #ifndef MINUTILS_LISTFILE_H_INCLUDED
