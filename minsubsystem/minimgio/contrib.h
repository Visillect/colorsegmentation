#pragma once
#ifndef MINIMGIO_CONTRIB_H_INCLUDED
#define MINIMGIO_CONTRIB_H_INCLUDED

#include <cstdio>
#include <vector>

#include <minbase/minimg.h>
#include <minimgio/define.h>

namespace se { namespace image_io {

enum ImageFormat {
  FORMAT_JPEG,
  FORMAT_TIFF,
  FORMAT_PNG,
  FORMAT_WEBP,
  FORMAT_UNKNOWN
};

struct MINIMGIO_API OutputStreamInterface {
  virtual ~OutputStreamInterface() { }
  virtual int WriteBytes(const uint8_t *bytes, int count) = 0;
};

class MINIMGIO_API FileOutputStream : public OutputStreamInterface {
 public:
  FileOutputStream();
  ~FileOutputStream();

  virtual int WriteBytes(const uint8_t *bytes, int count);

  int Open(const char *file_name);
  int Close();
 private:
  FILE *file_;
};

class MINIMGIO_API FixedBufferOutputStream : public OutputStreamInterface {
 public:
  FixedBufferOutputStream();
  ~FixedBufferOutputStream();

  virtual int WriteBytes(const uint8_t *bytes, int count);

  int Bind(uint8_t *buffer, int length);
  int WrittenBytes() const;
 private:
  uint8_t *buffer_;
  int length_;
  int written_;
};

class MINIMGIO_API ExtensibleBufferOutputStream : public OutputStreamInterface {
 public:
  ExtensibleBufferOutputStream();
  ~ExtensibleBufferOutputStream();

  virtual int WriteBytes(const uint8_t *bytes, int count);

  const uint8_t *GetBuffer() const;
  int WrittenBytes() const;
 private:
  std::vector<uint8_t> buffer_;
};

struct MINIMGIO_API InputStreamInterface {
  virtual ~InputStreamInterface() { }
  virtual int ReadBytes(uint8_t *bytes, int count) = 0;
};



MINIMGIO_API int EncodeImage(const MinImg            &image,
                             ImageFormat             format,
                             OutputStreamInterface   &output);

} // image_io

} // namespace se

#endif // #ifndef MINIMGIO_CONTRIB_H_INCLUDED
