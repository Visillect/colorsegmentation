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

#include <minbase/minresult.h>
#include <minutils/smartptr.h>

#include "v4l2utils.h"

#include <linux/videodev2.h>

#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <vector>
#include <cstring>

#include <libv4l2.h>

int InsistentIoctl(int dev_file, int request, const void* p_arg) {
  int res = -1;
  do {
    res = v4l2_ioctl(dev_file, request, p_arg);
  } while (-1 == res && (EINTR == errno || EAGAIN == errno ||
                         EIO == errno /*|| EBUSY == errno*/));
  if (-1 == res) {
    int err_value = errno;
    int for_break = err_value;
  }
  return res;
}


typedef DIR* DirPtr;

static MUSTINLINE void CloseDir(DirPtr* p_dirptr) {
  if (p_dirptr)
    closedir(*p_dirptr);
}

DEFINE_SCOPED_HANDLE(ScopedDIR, DirPtr, CloseDir)

static int ListDirectory(
    std::vector<std::string>& addr_list,
    const std::string&        dir_path) {
  ScopedDIR dir_handle;
  dir_handle.reset(opendir(dir_path.c_str()));
  if (!dir_handle)
    return FILE_ERROR;

  addr_list.clear();
  dirent* p_entry;
  while ((p_entry = readdir(dir_handle))) {
    if (!::strcmp(p_entry->d_name, ".") || !::strcmp(p_entry->d_name, ".."))
      continue;
    addr_list.push_back(dir_path);
    addr_list.back() += p_entry->d_name;
  }

  return NO_ERRORS;
}


ScopedV4L2File::ScopedV4L2File(const std::string& file_path)
  : file_path_(file_path),
    file_is_proper_(false),
    fd_(-1) {
  struct stat st;
  file_is_proper_ = (-1 < stat(file_path.c_str(), &st) && S_ISCHR(st.st_mode));
}

ScopedV4L2File::~ScopedV4L2File() {
  Close();
}

bool ScopedV4L2File::IsFileProper() const {
  return file_is_proper_;
}

int ScopedV4L2File::Open() {
  if (!IsFileProper())
    return -1;
  if (fd_ < 0)
    fd_ = v4l2_open(file_path_.c_str(), O_RDWR | O_NONBLOCK, 0);
  return fd_;
}

void ScopedV4L2File::Close() {
  if (-1 < fd_)
    IGNORE_ERRORS(v4l2_close(fd_));
  fd_ = -1;
}

ScopedV4L2File::operator const int&() const {
  return fd_;
}


static int FillStreamingCaptureDeviceNameAddrMapByAddresses(
    std::map<std::string, std::string>& mp,
    const std::vector<std::string>&     addr_list) {
  for (const auto& addr : addr_list) {
    ScopedV4L2File device_file(addr);
    if (-1 == device_file.Open())
      return FILE_ERROR;

    struct v4l2_capability cap;
    if (-1 == InsistentIoctl(device_file, VIDIOC_QUERYCAP, &cap))
      return INTERNAL_ERROR;

    if (cap.capabilities & (V4L2_CAP_VIDEO_CAPTURE | V4L2_CAP_STREAMING))
      mp[reinterpret_cast<char*>(cap.card)] = addr;
  }

  return NO_ERRORS;
}

int GetStreamingCaptureDeviceNameAddrMap(
    std::map<std::string, std::string>& name_addr_map) {
  constexpr char dev_addr_prefix[] = "/dev/v4l/by-id/";

  std::vector<std::string> addr_list;
  PROPAGATE_ERROR(ListDirectory(addr_list, dev_addr_prefix));
  std::map<std::string, std::string> tmp_map;
  PROPAGATE_ERROR(FillStreamingCaptureDeviceNameAddrMapByAddresses(tmp_map,
                                                                   addr_list));
  std::swap(name_addr_map, tmp_map);

  return NO_ERRORS;
}
