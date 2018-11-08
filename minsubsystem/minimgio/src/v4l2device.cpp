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

#include <minimgio/minimgio.h>
#include <minbase/minresult.h>

#include <minimgio/src/minimgiojpeg.h>
#include <minimgio/device.h>

#include "v4l2device.h"

#include <utility>
#include <string>
#include <map>

#include <chrono>
#include <thread>

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <cstring>

#include <libv4l2.h>

///
/// TODO: Look opencv-3.1.0/modules/videoio/src/cap_libv4l.cpp
///       from line 1095


static int ExtractRangedValue(int& value, const char* p_str, int mn, int mx) {
  char* p_str_end = 0;
  double tmp = ::strtod(p_str, &p_str_end);
  if (*p_str_end)
    return BAD_ARGS;
  value = static_cast<int>(mn + (mx - mn) * tmp / 100. + 0.5);
  return NO_ERRORS;
}

static int ExtractBooleanValue(int& value, const char* p_str) {
  if (!::strcmp(p_str, "true"))
    value = 1;
  else if (!::strcmp(p_str, "false"))
    value = 0;
  else
    return BAD_ARGS;
  return NO_ERRORS;
}

static int ExtractFrameSizeValues(
    uint32_t&   width,
    uint32_t&   height,
    const char* p_str) {
  char* p_str_end = nullptr;
  uint32_t t_width = ::strtoul(p_str, &p_str_end, 10);
  if ('x' != *p_str_end)
    return BAD_ARGS;
  uint32_t t_height = ::strtoul(p_str_end + 1, &p_str_end, 10);
  if (*p_str_end)
    return BAD_ARGS;
  if (0 == t_width || 0 == t_height)
    return BAD_ARGS;
  width = t_width;
  height = t_height;
  return NO_ERRORS;
}

static void SetTimePerFrameApproximation(
    uint32_t& numerator,
    uint32_t& denominator,
    real64_t  fps) {
  /// TODO: Probably make better fraction approximation
  numerator = 1;
  denominator = static_cast<uint32_t>(fps + 0.5);
}

V4L2Device* V4L2Device::Instance(
    const std::string& device_path,
    const std::string& frame_size,
    const std::string& frame_rate) {
  uint32_t width = 0;
  uint32_t height = 0;
  real64_t fps = 0.;
  uint32_t numerator = 0;
  uint32_t denominator = 0;
  if (!frame_size.empty())
    if (NO_ERRORS != ExtractFrameSizeValues(width, height, frame_size.c_str()))
      return nullptr;
  if (!frame_rate.empty()) {
    char* p_str_end = nullptr;
    fps = ::strtod(frame_rate.c_str(), &p_str_end);
    if (*p_str_end)
      return nullptr;
  }
  if (!frame_rate.empty())
    SetTimePerFrameApproximation(numerator, denominator, fps);

  static std::map<std::string, V4L2Device*> devices;
  V4L2Device* p_device = nullptr;
  const auto it = devices.find(device_path);
  if (devices.end() == it) {
    p_device = new V4L2Device(device_path); /// And still, WHO WILL DELETE THIS?
    if (!p_device->dev_file_.IsFileProper()) {
      delete p_device;
      return nullptr;
    }
    devices[device_path] = p_device;
  } else
    p_device = it->second;

  if (p_device->counter_) {
    if (!frame_size.empty() &&
        (p_device->pix_.width != width || p_device->pix_.height != height))
      return nullptr;
    if (!frame_rate.empty() &&
        (p_device->timeperframe_.numerator != numerator ||
         p_device->timeperframe_.denominator != denominator))
      return nullptr;

    return p_device;
  }

  if (!frame_size.empty()) {
    p_device->pix_.width = width;
    p_device->pix_.height = height;
  }

  if (!frame_rate.empty()) {
    p_device->timeperframe_.numerator = numerator;
    p_device->timeperframe_.denominator = denominator;
  }

  return p_device;
}

V4L2Device::V4L2Device(const std::string& device_path)
  : ImageShipper_(nullptr),
    counter_(0),
    dev_file_(device_path),
    fmt_{0},
    streamparams_{0},
    buffers_{{0}},
    n_buffers_(0),
    cur_buf_index_(0),
#ifdef MIN_V4L2_DEVICE_USE_BUFFER_PROXY
    p_buffer_proxy_(nullptr),
#endif
    pix_(fmt_.fmt.pix),
    timeperframe_(streamparams_.parm.capture.timeperframe) {
  fmt_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  streamparams_.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
}

V4L2Device::~V4L2Device() {
  // FIXME: Need to backup realease here
}


template<uint32_t> static int ShipImage(
    const MinImg*  /*p_image*/,
    const uint8_t* /*p_buffer*/,
    size_t         /*buffer_size*/) {
  return NOT_IMPLEMENTED;
}

template<> STATIC_SPECIAL int ShipImage<V4L2_PIX_FMT_RGB24>(
    const MinImg*  p_image,
    const uint8_t* p_buffer,
    size_t         /*buffer_size*/) {
  uint8_t* p_line = p_image->pScan0;
  const uint8_t* p_src = p_buffer;
  const int unf_width = p_image->width * 3;
  for (int y = 0; y < p_image->height; ++y) {
    for (int unf_x = 0; unf_x < unf_width; ++unf_x)
      p_line[unf_x] = *p_src++;
    p_line += p_image->stride;
  }

  return NO_ERRORS;
}

template<> STATIC_SPECIAL int ShipImage<V4L2_PIX_FMT_MJPEG>(
    const MinImg*  p_image,
    const uint8_t* p_buffer,
    size_t         buffer_size) {
  char jpg_memfilename[32];
  ::sprintf(jpg_memfilename, "mem://%p.%lu", p_buffer, buffer_size);
  return LoadJpeg(p_image, jpg_memfilename);
}


int V4L2Device::NegotiateFormat() {
  static const std::pair<uint32_t, ImageShipperType> supported_formats[] = {
    { V4L2_PIX_FMT_RGB24, ShipImage<V4L2_PIX_FMT_RGB24> },
    { V4L2_PIX_FMT_MJPEG, ShipImage<V4L2_PIX_FMT_MJPEG> },
    { V4L2_PIX_FMT_BGR24, ShipImage<V4L2_PIX_FMT_BGR24> },
    { V4L2_PIX_FMT_YUYV,  ShipImage<V4L2_PIX_FMT_YUYV> },
  };
  const size_t nfmts = sizeof(supported_formats) / sizeof(supported_formats[0]);

  const uint32_t width = pix_.width;
  const uint32_t height = pix_.height;

  for (size_t f = 0; f < nfmts; ++f) {
    pix_.pixelformat = supported_formats[f].first;
    pix_.width = width;
    pix_.height = height;
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_S_FMT, &fmt_))
      return INTERNAL_ERROR;
    if (supported_formats[f].first == pix_.pixelformat) {
      ImageShipper_ = supported_formats[f].second;
      return NO_ERRORS;
    }
  }

  return NOT_SUPPORTED;
}

int V4L2Device::GetCtrls() {
  static const std::pair<std::string, int> prop_cid_map[] = {
    { SP_GAIN,          V4L2_CID_GAIN },
    { SP_GAINAUTO,      V4L2_CID_AUTOGAIN },
    { SP_EXPOSURE,      V4L2_CID_EXPOSURE_ABSOLUTE },
    { SP_EXPOSUREAUTO,  V4L2_CID_EXPOSURE_AUTO },
    { SP_FOCUS,         V4L2_CID_FOCUS_ABSOLUTE },
    { SP_FOCUSAUTO,     V4L2_CID_FOCUS_AUTO },
  };

  for (const auto& prop_cid_pair : prop_cid_map) {
    v4l2_queryctrl& query = ctrl_queries[prop_cid_pair.first];
    ::memset(&query, 0, sizeof(v4l2_queryctrl));
    query.id = prop_cid_pair.second;
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_QUERYCTRL, &query)) {
      if (EINVAL != errno)
        return INTERNAL_ERROR;
      query.flags |= V4L2_CTRL_FLAG_DISABLED;
    }
    if (query.type == V4L2_CTRL_TYPE_MENU)
      query.flags |= V4L2_CTRL_FLAG_DISABLED;
    if (!(query.flags & V4L2_CTRL_FLAG_DISABLED)) {
      v4l2_control& ctrl = cur_ctrls[prop_cid_pair.first];
      ctrl.id = query.id;
      if (-1 == InsistentIoctl(dev_file_, VIDIOC_G_CTRL, &ctrl))
        return INTERNAL_ERROR;
    }
  }

  int streamparm_request = timeperframe_.denominator ? VIDIOC_S_PARM
                                                     : VIDIOC_G_PARM;
  if (-1 == InsistentIoctl(dev_file_, streamparm_request, &streamparams_))
    return INTERNAL_ERROR;

  return NO_ERRORS;
}

int V4L2Device::GetBuffersAndStartStreaming() {
  if (n_buffers_)
    return BAD_STATE;
  for (size_t b = 0; b < MAX_V4L2_BUFFERS; ++b)
    if (buffers_[b].size)
      return BAD_STATE;

  v4l2_requestbuffers buf_request = {0};
  buf_request.count = MAX_V4L2_BUFFERS;
  buf_request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  buf_request.memory = V4L2_MEMORY_MMAP;
  if (-1 == InsistentIoctl(dev_file_, VIDIOC_REQBUFS, &buf_request))
    return INTERNAL_ERROR;

  if (buf_request.count > MAX_V4L2_BUFFERS) {
    buf_request.count = 0;
    IGNORE_ERRORS(InsistentIoctl(dev_file_, VIDIOC_REQBUFS, &buf_request));
  }
  if (!buf_request.count)
    return INTERNAL_ERROR;
  n_buffers_ = buf_request.count;

  v4l2_buffer v4l2_buf = {0};
  v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  for (size_t b = 0; b < n_buffers_; ++b) {
    v4l2_buf.index = b;
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_QUERYBUF, &v4l2_buf))
      return INTERNAL_ERROR;
    buffers_[b].p = v4l2_mmap(nullptr, v4l2_buf.length, PROT_READ | PROT_WRITE,
                              MAP_SHARED, dev_file_, v4l2_buf.m.offset);
    if (MAP_FAILED == buffers_[b].p || !buffers_[b].p) {
      buffers_[b].p = nullptr;
      IGNORE_ERRORS(StopStreamingAndFreeBuffers());
      return INTERNAL_ERROR;
    }
    buffers_[b].size = v4l2_buf.length;

    if (-1 == InsistentIoctl(dev_file_, VIDIOC_QBUF, &v4l2_buf)) {
      IGNORE_ERRORS(StopStreamingAndFreeBuffers());
      return INTERNAL_ERROR;
    }
  }

#ifdef MIN_V4L2_DEVICE_USE_BUFFER_PROXY
  p_buffer_proxy_ = alignedmalloc(buffers_[0].size, 16);
  if (!p_buffer_proxy_) {
    IGNORE_ERRORS(StopStreamingAndFreeBuffers());
    return NO_MEMORY;
  }
#endif

  if (-1 == InsistentIoctl(dev_file_, VIDIOC_STREAMON, &v4l2_buf.type)) {
    IGNORE_ERRORS(StopStreamingAndFreeBuffers());
    return INTERNAL_ERROR;
  }

  return NO_ERRORS;
}

int V4L2Device::StopStreamingAndFreeBuffers() {
  int ret = NO_ERRORS;

  v4l2_requestbuffers buf_request = {0};
  buf_request.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == InsistentIoctl(dev_file_, VIDIOC_STREAMOFF, &buf_request.type))
    ret = INTERNAL_ERROR;

  for (size_t b = 0; b < n_buffers_; ++b) {
    if (!buffers_[b].size)
      continue;
    if (-1 == v4l2_munmap(buffers_[b].p, buffers_[b].size))
      ret = INTERNAL_ERROR;
    else
      ::memset(&buffers_[b], 0, sizeof(Buffer));
  }

  if (n_buffers_) {
    buf_request.memory = V4L2_MEMORY_MMAP;
    buf_request.count = 0;
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_REQBUFS, &buf_request))
      ret = INTERNAL_ERROR;
    else
      n_buffers_ = 0;
  }

#ifdef MIN_V4L2_DEVICE_USE_BUFFER_PROXY
  if (p_buffer_proxy_) {
    alignedfree(p_buffer_proxy_);
    p_buffer_proxy_ = nullptr;
  }
#endif

  return ret;
}

int V4L2Device::Take() {
  if (!counter_++) {
    if (0 > dev_file_.Open())
      return FILE_ERROR;

    PROPAGATE_ERROR(NegotiateFormat());

    PROPAGATE_ERROR(GetCtrls());

    PROPAGATE_ERROR(GetBuffersAndStartStreaming());
  }

  return NO_ERRORS;
}

int V4L2Device::Release() {
  int ret = NO_ERRORS;

  if (!--counter_) {
    ret = StopStreamingAndFreeBuffers();
    dev_file_.Close();
  }

  return ret;
}

int V4L2Device::PushImage(const MinImg*) {
  return NO_SENSE;
}

int V4L2Device::QueryImage(const MinImg* p_image) {
  if (!ImageShipper_ || !counter_)
    return BAD_STATE;

  if (!p_image)
    return BAD_ARGS;
  /// TODO: Add other p_image checks (with respect to format conversion)

  /// TODO: Add exit by timeout (like in opencv) ?

  v4l2_buffer v4l2_buf = {0};
  v4l2_buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  v4l2_buf.memory = V4L2_MEMORY_MMAP;
  v4l2_buf.index = cur_buf_index_;
  if (-1 == InsistentIoctl(dev_file_, VIDIOC_DQBUF, &v4l2_buf))
    return INTERNAL_ERROR;
  while (v4l2_buf.flags & V4L2_BUF_FLAG_ERROR) {
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_QBUF, &v4l2_buf))
      return INTERNAL_ERROR;
    ++v4l2_buf.index;
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_DQBUF, &v4l2_buf))
      return INTERNAL_ERROR;
  }

  const uint8_t* p_ship_buffer = reinterpret_cast<uint8_t*>(
                                   buffers_[v4l2_buf.index].p);

#ifdef MIN_V4L2_DEVICE_USE_BUFFER_PROXY
  ::memcpy(p_buffer_proxy_, buffers_[v4l2_buf.index].p, buffers_[0].size);
  p_ship_buffer = reinterpret_cast<uint8_t*>(p_buffer_proxy_);
#endif

  cur_buf_index_ = (v4l2_buf.index + 1) % n_buffers_;

  PROPAGATE_ERROR(ImageShipper_(p_image, p_ship_buffer, buffers_[0].size));

  if (-1 == InsistentIoctl(dev_file_, VIDIOC_QBUF, &v4l2_buf))
    return INTERNAL_ERROR;

  return NO_ERRORS;
}

int V4L2Device::QueryImagePropsEx(MinImg* p_image, ExtImgProps* p_props) {
  if (!counter_)
    return BAD_STATE;

  if (!p_image)
    return BAD_ARGS;



  // temporary:
  p_image->channelDepth = 1;
  p_image->channels = 3;
  p_image->width = pix_.width;
  p_image->height = pix_.height;
  return NO_ERRORS;


  // TODO: Needs proper implementation yet
  return NOT_IMPLEMENTED;
}

int V4L2Device::GetProperty(const char* p_key, char* p_value, int size) {
  if (0 > dev_file_)
    return BAD_STATE;

  if (!p_key || !p_value)
    return BAD_ARGS;

  const auto prop_query_it = ctrl_queries.find(p_key);
  if (prop_query_it != ctrl_queries.end()) {
    v4l2_queryctrl& query = prop_query_it->second;
    if (query.flags & V4L2_CTRL_FLAG_DISABLED)
      return NOT_SUPPORTED;
    int32_t& val = cur_ctrls[p_key].value;
    v4l2_ctrl_type ctrl_type = static_cast<v4l2_ctrl_type>(query.type);
    switch (ctrl_type) {
    case V4L2_CTRL_TYPE_INTEGER:
      if (::snprintf(p_value, size, "%d", val) > size)
        return NO_MEMORY;
      return NO_ERRORS;
    case V4L2_CTRL_TYPE_BOOLEAN:
      if (::snprintf(p_value, size, "%s", val ? "true" : "false") > size)
        return NO_MEMORY;
      return NO_ERRORS;
    default:
      return NOT_SUPPORTED;
    }
  }

  if (!::strcmp(p_key, SP_FRAMESIZE)) {
    if (::snprintf(p_value, size, "%dx%d", pix_.width, pix_.height) > size)
      return NO_MEMORY;

    return NO_ERRORS;
  }

  if (!::strcmp(p_key, SP_FRAMERATE)) {
    if (::snprintf(p_value, size, "%lf", timeperframe_.denominator /
                   (timeperframe_.numerator + 0.)) > size)
      return NO_MEMORY;

    return NO_ERRORS;
  }

  return NOT_IMPLEMENTED;
}

int V4L2Device::SetProperty(const char* p_key, const char* p_value) {
  if (0 > dev_file_)
    return BAD_STATE;

  if (!p_key || !p_value)
    return BAD_ARGS;

  const auto prop_query_it = ctrl_queries.find(p_key);
  if (prop_query_it != ctrl_queries.end()) {
    v4l2_queryctrl& query = prop_query_it->second;
    if (query.flags & V4L2_CTRL_FLAG_DISABLED)
      return NOT_SUPPORTED;

    size_t len = ::strlen(p_key);
    if (len < 5 || ::strcmp(p_key + len - 4, "Auto")) {
      std::string key_auto = p_key;
      key_auto += "Auto";
      const auto prop_query_it_auto = ctrl_queries.find(key_auto);
      if (prop_query_it_auto != ctrl_queries.end() &&
          !(prop_query_it_auto->second.flags & V4L2_CTRL_FLAG_DISABLED)) {
        v4l2_control& control_auto = cur_ctrls[key_auto];
        if (control_auto.value) {
          control_auto.value = 0;
          if (-1 == InsistentIoctl(dev_file_, VIDIOC_S_CTRL, &control_auto))
            return INTERNAL_ERROR;
          if (control_auto.value)
            return INTERNAL_ERROR;
        }
      }
    }

    v4l2_control& control = cur_ctrls[p_key];
    int value = 0;
    v4l2_ctrl_type ctrl_type = static_cast<v4l2_ctrl_type>(query.type);
    switch (ctrl_type) {
    case V4L2_CTRL_TYPE_INTEGER:
      PROPAGATE_ERROR(ExtractRangedValue(value, p_value,
                                         query.minimum, query.maximum));
      break;
    case V4L2_CTRL_TYPE_BOOLEAN:
      PROPAGATE_ERROR(ExtractBooleanValue(value, p_value));
      break;
    default:
      return NOT_SUPPORTED;
    }

    if (control.value == value)
      return NO_ERRORS;

    control.value = value;
    if (-1 == InsistentIoctl(dev_file_, VIDIOC_S_CTRL, &control))
      return INTERNAL_ERROR;
    if (control.value != value)
      return INTERNAL_ERROR;

    return NO_ERRORS;
  }

  if (!::strcmp(p_key, SP_FRAMESIZE)) {
    if (1 != counter_)
      return BAD_STATE;

    uint32_t width = 0;
    uint32_t height = 0;
    PROPAGATE_ERROR(ExtractFrameSizeValues(width, height, p_value));

    PROPAGATE_ERROR(StopStreamingAndFreeBuffers());

    PROPAGATE_ERROR(SetFrameSize(width, height));

    return GetBuffersAndStartStreaming();
  }

  if (!::strcmp(p_key, SP_FRAMERATE)) {
    char* p_str_end = 0;
    double fps = ::strtod(p_value, &p_str_end);
    if (*p_str_end)
      return BAD_ARGS;

    PROPAGATE_ERROR(StopStreamingAndFreeBuffers());

    PROPAGATE_ERROR(SetFrameRate(fps));

    return GetBuffersAndStartStreaming();
  }

  return NOT_IMPLEMENTED;
}

int V4L2Device::SetFrameSize(uint32_t width, uint32_t height) {
  if (n_buffers_)
    return BAD_STATE;
  if (pix_.width == width && pix_.height == height)
    return NO_ERRORS;
  pix_.width = width;
  pix_.height = height;
  if (-1 == InsistentIoctl(dev_file_, VIDIOC_S_FMT, &fmt_))
    return INTERNAL_ERROR;

  return NO_ERRORS;
}

int V4L2Device::SetFrameRate(real64_t fps) {
  if (n_buffers_)
    return BAD_STATE;
  uint32_t numerator = 0;
  uint32_t denominator = 0;
  SetTimePerFrameApproximation(numerator, denominator, fps);
  if (timeperframe_.numerator == numerator &&
      timeperframe_.denominator == denominator)
    return NO_ERRORS;
  timeperframe_.numerator = numerator;
  timeperframe_.denominator = denominator;

  if (-1 == InsistentIoctl(dev_file_, VIDIOC_S_PARM, &streamparams_))
    return INTERNAL_ERROR;

  return NO_ERRORS;
}


///*

//    constexpr uint32_t formats[] = {
//      V4L2_PIX_FMT_RGB332, V4L2_PIX_FMT_RGB444, V4L2_PIX_FMT_ARGB444,
//      V4L2_PIX_FMT_XRGB444, V4L2_PIX_FMT_RGB555, V4L2_PIX_FMT_ARGB555,
//      V4L2_PIX_FMT_XRGB555, V4L2_PIX_FMT_RGB565, V4L2_PIX_FMT_RGB555X,
//      V4L2_PIX_FMT_ARGB555X, V4L2_PIX_FMT_XRGB555X, V4L2_PIX_FMT_RGB565X,
//      V4L2_PIX_FMT_BGR666, V4L2_PIX_FMT_BGR24, V4L2_PIX_FMT_RGB24,
//      V4L2_PIX_FMT_BGR32, V4L2_PIX_FMT_ABGR32, V4L2_PIX_FMT_XBGR32,
//      V4L2_PIX_FMT_RGB32, V4L2_PIX_FMT_ARGB32, V4L2_PIX_FMT_XRGB32,

//      V4L2_PIX_FMT_GREY, V4L2_PIX_FMT_Y4, V4L2_PIX_FMT_Y6, V4L2_PIX_FMT_Y10,
//      V4L2_PIX_FMT_Y12, V4L2_PIX_FMT_Y16 ,

//      V4L2_PIX_FMT_Y10BPACK,

//      V4L2_PIX_FMT_PAL8 ,

//      V4L2_PIX_FMT_UV8,

//      V4L2_PIX_FMT_YVU410, V4L2_PIX_FMT_YVU420, V4L2_PIX_FMT_YUYV,
//      V4L2_PIX_FMT_YYUV, V4L2_PIX_FMT_YVYU, V4L2_PIX_FMT_UYVY,
//      V4L2_PIX_FMT_VYUY, V4L2_PIX_FMT_YUV422P, V4L2_PIX_FMT_YUV411P,
//      V4L2_PIX_FMT_Y41P, V4L2_PIX_FMT_YUV444, V4L2_PIX_FMT_YUV555,
//      V4L2_PIX_FMT_YUV565, V4L2_PIX_FMT_YUV32, V4L2_PIX_FMT_YUV410,
//      V4L2_PIX_FMT_YUV420, V4L2_PIX_FMT_HI240, V4L2_PIX_FMT_HM12,
//      V4L2_PIX_FMT_M420,

//      V4L2_PIX_FMT_NV12, V4L2_PIX_FMT_NV21, V4L2_PIX_FMT_NV16,
//      V4L2_PIX_FMT_NV61, V4L2_PIX_FMT_NV24, V4L2_PIX_FMT_NV42,

//      V4L2_PIX_FMT_NV12M, V4L2_PIX_FMT_NV21M, V4L2_PIX_FMT_NV16M,
//      V4L2_PIX_FMT_NV61M, V4L2_PIX_FMT_NV12MT, V4L2_PIX_FMT_NV12MT_16X16,

//      V4L2_PIX_FMT_YUV420M, V4L2_PIX_FMT_YVU420M,

//      V4L2_PIX_FMT_SBGGR8, V4L2_PIX_FMT_SGBRG8, V4L2_PIX_FMT_SGRBG8,
//      V4L2_PIX_FMT_SRGGB8, V4L2_PIX_FMT_SBGGR10, V4L2_PIX_FMT_SGBRG10,
//      V4L2_PIX_FMT_SGRBG10, V4L2_PIX_FMT_SRGGB10,

//      V4L2_PIX_FMT_SBGGR10P, V4L2_PIX_FMT_SGBRG10P, V4L2_PIX_FMT_SGRBG10P,
//      V4L2_PIX_FMT_SRGGB10P,

//      V4L2_PIX_FMT_SBGGR10ALAW8, V4L2_PIX_FMT_SGBRG10ALAW8,
//      V4L2_PIX_FMT_SGRBG10ALAW8, V4L2_PIX_FMT_SRGGB10ALAW8,

//      V4L2_PIX_FMT_SBGGR10DPCM8, V4L2_PIX_FMT_SGBRG10DPCM8,
//      V4L2_PIX_FMT_SGRBG10DPCM8, V4L2_PIX_FMT_SRGGB10DPCM8,
//      V4L2_PIX_FMT_SBGGR12, V4L2_PIX_FMT_SGBRG12,
//      V4L2_PIX_FMT_SGRBG12, V4L2_PIX_FMT_SRGGB12,
//      V4L2_PIX_FMT_SBGGR16,

//      V4L2_PIX_FMT_MJPEG, V4L2_PIX_FMT_JPEG, V4L2_PIX_FMT_DV,
//      V4L2_PIX_FMT_MPEG, V4L2_PIX_FMT_H264, V4L2_PIX_FMT_H264_NO_SC,
//      V4L2_PIX_FMT_H264_MVC, V4L2_PIX_FMT_H263, V4L2_PIX_FMT_MPEG1,
//      V4L2_PIX_FMT_MPEG2, V4L2_PIX_FMT_MPEG4, V4L2_PIX_FMT_XVID,
//      V4L2_PIX_FMT_VC1_ANNEX_G, V4L2_PIX_FMT_VC1_ANNEX_L, V4L2_PIX_FMT_VP8,

//      V4L2_PIX_FMT_CPIA1, V4L2_PIX_FMT_WNVA, V4L2_PIX_FMT_SN9C10X,
//      V4L2_PIX_FMT_SN9C20X_I420, V4L2_PIX_FMT_PWC1, V4L2_PIX_FMT_PWC2,
//      V4L2_PIX_FMT_ET61X251, V4L2_PIX_FMT_SPCA501, V4L2_PIX_FMT_SPCA505,
//      V4L2_PIX_FMT_SPCA508, V4L2_PIX_FMT_SPCA561, V4L2_PIX_FMT_PAC207,
//      V4L2_PIX_FMT_MR97310A, V4L2_PIX_FMT_JL2005BCD, V4L2_PIX_FMT_SN9C2028,
//      V4L2_PIX_FMT_SQ905C, V4L2_PIX_FMT_PJPG, V4L2_PIX_FMT_OV511,
//      V4L2_PIX_FMT_OV518, V4L2_PIX_FMT_STV0680, V4L2_PIX_FMT_TM6000,
//      V4L2_PIX_FMT_CIT_YYVYUY, V4L2_PIX_FMT_KONICA420, V4L2_PIX_FMT_JPGL,
//      V4L2_PIX_FMT_SE401, V4L2_PIX_FMT_S5C_UYVY_JPG
//    };

//*/
