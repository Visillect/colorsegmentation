#include <cstdlib>
#include <cstring>
#include <algorithm>

#include <minimgio/contrib.h>
#include <minutils/minhelpers.h>

#if defined(WITH_TIFF)
# include <tiffio.h>
#endif // WITH_TIFF

#if defined(WITH_JPEG)
# include <libjpeg/jpeglib.h>
# include <libjpeg/jerror.h>
# include <csetjmp>
#endif // WITH_JPEG

#if defined(WITH_PNG)
# define PNG_SKIP_SETJMP_CHECK
# include <png.h>
#endif // WITH_PNG

#if defined(WITH_WEBP)
# include <webp/decode.h>
# include <webp/encode.h>
#endif // WITH_WEBP

namespace se { namespace image_io {


/// Implementation for FileOutputStream

FileOutputStream::FileOutputStream()
    : file_(NULL) { }

FileOutputStream::~FileOutputStream() {
  Close();
}

int FileOutputStream::Open(const char *file_name) {
  if (!file_name) {
    return -1;
  }

  file_ = ::fopen(file_name, "wb");
  if (!file_) {
    return -1;
  }

  return 0;
}

int FileOutputStream::Close() {
  if (file_) {
    ::fclose(file_);
  }

  return 0;
}

int FileOutputStream::WriteBytes(const uint8_t *bytes, int count) {
  if (!file_) {
    return -1;
  }

  if (!bytes || count <= 0) {
    return -1;
  }

  size_t to_write = static_cast<size_t>(count);
  size_t written = ::fwrite(bytes, 1, count, file_);
  if (written != to_write) {
    return -1;
  }

  return 0;
}


/// Implementation for FixedBufferOutputStream.

FixedBufferOutputStream::FixedBufferOutputStream()
    : buffer_(NULL),
      length_(0),
      written_(0) { }

FixedBufferOutputStream::~FixedBufferOutputStream() { }

int FixedBufferOutputStream::Bind(uint8_t *buffer, int length) {
  if (!buffer || length <= 0) {
    return -1;
  }

  buffer_ = buffer;
  length_ = length;
  written_ = 0;

  return 0;
}

int FixedBufferOutputStream::WrittenBytes() const {
  return written_;
}

int FixedBufferOutputStream::WriteBytes(const uint8_t *bytes, int count) {
  if (!buffer_) {
    return -1;
  }

  // Check if we have rooms for new data.
  if ((length_ - written_) < count) {
    return -1;
  }

  ::memcpy(buffer_ + written_, bytes, count);
  written_ += count;

  return 0;
}


/// Implementation for ExtensibleBufferOutputStream.

ExtensibleBufferOutputStream::ExtensibleBufferOutputStream() { }

ExtensibleBufferOutputStream::~ExtensibleBufferOutputStream() { }

const uint8_t *ExtensibleBufferOutputStream::GetBuffer() const {
  if (buffer_.size() > 0) {
    return &buffer_[0];
  }

  return NULL;
}

int ExtensibleBufferOutputStream::WrittenBytes() const {
  return static_cast<int>(buffer_.size());
}

int ExtensibleBufferOutputStream::WriteBytes(const uint8_t *bytes, int count) {
  buffer_.insert(buffer_.end(), bytes, bytes + count);
  return 0;
}

static int EncodeImageToTiff(
    const MinImg            & /* image */,
    OutputStreamInterface   & /* output */) {
#if !defined(WITH_TIFF)
  return -1;
#else
  return -1;
#endif // WITH_TIFF
}

#if defined(WITH_JPEG)
struct JpegErrorManager {
  jpeg_error_mgr manager;
  jmp_buf jump_buffer;
};

const int JPEG_DEFAULT_QUALITY = 90;
const int JPEG_BUFFER_SIZE = 1024;

static void JpegErrorHelper(j_common_ptr cinfo) {
  JpegErrorManager *error_manager =
      reinterpret_cast<JpegErrorManager *>(cinfo->err);
  ::longjmp(error_manager->jump_buffer, 1);
}

static void JpegOutputMessage(j_common_ptr) {
  ; // Do nothing.
}


struct JpegDestManager {
  jpeg_destination_mgr manager;
  OutputStreamInterface *output;
  std::vector<JOCTET> *buffer;
};


static void JpegDestManagerInitBuffer(jpeg_compress_struct* cinfo) {
  JpegDestManager *manager = reinterpret_cast<JpegDestManager*>(cinfo->dest);
  std::vector<JOCTET> &buf = *manager->buffer;
  manager->manager.next_output_byte = &(buf)[0];
  manager->manager.free_in_buffer = buf.size();
}

static boolean JpegDestManagerEmptyBuffer(jpeg_compress_struct* cinfo) {
  JpegDestManager *manager = reinterpret_cast<JpegDestManager*>(cinfo->dest);
  std::vector<JOCTET> &buf = *manager->buffer;
  if(manager->output->WriteBytes(&buf[0], buf.size())) {
    return FALSE;
  }
  manager->manager.next_output_byte = &buf[0];
  manager->manager.free_in_buffer = buf.size();
  return TRUE;
}

static void JpegDestManagerTermBuffer(jpeg_compress_struct* cinfo) {
  JpegDestManager *manager = reinterpret_cast<JpegDestManager*>(cinfo->dest);
  std::vector<JOCTET> &buf = *manager->buffer;
  int size = manager->manager.next_output_byte - &buf[0];
  if (manager->output->WriteBytes(&buf[0], size)) {
    JpegErrorManager *error_manager =
        reinterpret_cast<JpegErrorManager *>(cinfo->err);
    ::longjmp(error_manager->jump_buffer, 1);
  }
}
#endif // WITH_JPEG

static int EncodeImageToJpeg(
    const MinImg            &image,
    OutputStreamInterface   &output) {
#if !defined(WITH_JPEG)
  SUPPRESS_UNUSED_VARIABLE(image);
  SUPPRESS_UNUSED_VARIABLE(output);
  return -1;
#else
  if (!image.pScan0)
    return -2;
  if (image.channelDepth != 1 || image.format != FMT_UINT)
    return -2;

  jpeg_compress_struct cinfo;
  ::memset(&cinfo, 0, sizeof(cinfo));
  JpegErrorManager error_manager;
  ::memset(&error_manager, 0, sizeof(error_manager));
  cinfo.err = jpeg_std_error(&error_manager.manager);
  error_manager.manager.error_exit = JpegErrorHelper;
  error_manager.manager.output_message = JpegOutputMessage;

  jpeg_create_compress(&cinfo);

  if (setjmp(error_manager.jump_buffer)) {
    jpeg_destroy_compress(&cinfo);
    return -3;
  }

  JpegDestManager dest_manager;
  std::vector<JOCTET> buffer(JPEG_BUFFER_SIZE);
  dest_manager.buffer = &buffer;
  dest_manager.output = &output;
  dest_manager.manager.init_destination = JpegDestManagerInitBuffer;
  dest_manager.manager.empty_output_buffer = JpegDestManagerEmptyBuffer;
  dest_manager.manager.term_destination = JpegDestManagerTermBuffer;
  cinfo.dest = reinterpret_cast<jpeg_destination_mgr *>(&dest_manager);

  cinfo.image_width = image.width;
  cinfo.image_height = image.height;
  cinfo.input_components = image.channels;
  switch (cinfo.input_components) {
  case 1:
    cinfo.in_color_space = JCS_GRAYSCALE;
    break;
  case 3:
    cinfo.in_color_space = JCS_RGB;
    break;
  default:
    cinfo.in_color_space = JCS_UNKNOWN;
  }

  jpeg_set_defaults(&cinfo);
//  if (pProps)
//  {
//    cinfo.density_unit = 1;
//    cinfo.X_density = (short)(pProps->xDPI + .5);
//    cinfo.Y_density = (short)(pProps->yDPI + .5);
//  }
  int quality = JPEG_DEFAULT_QUALITY;
//  if (pProps && pProps->qty)
//    quality = pProps->qty;
  jpeg_set_quality(&cinfo, quality, true);
  jpeg_start_compress(&cinfo, true);
  JSAMPROW row_ptrs[1] = {NULL};
  for (int y = 0; y < image.height; y++) {
    row_ptrs[0] = reinterpret_cast<JSAMPROW>(image.pScan0 + image.stride * y);
    jpeg_write_scanlines(&cinfo, row_ptrs, 1);
  }

  jpeg_finish_compress(&cinfo);
  return 0;
#endif // WITH_JPEG
}


/// Png support.

#if defined(WITH_PNG)
static void PngReadHelper(png_structp png_ptr,
                          png_bytep /* data */,
                          png_size_t /* length */) {
  png_error(png_ptr, "PngReadHelper is not implemented.");
}

void PngWriteHelper(png_structp png_ptr,
                    png_bytep data,
                    png_size_t length) {
  OutputStreamInterface *output =
      reinterpret_cast<OutputStreamInterface *>(
          ::png_get_io_ptr(png_ptr));
  if (output->WriteBytes(data, length)) {
    png_error(png_ptr, "Failed to write with PngWriteHelper.");
  }
}

void PngFlushHelper(png_structp /* png_ptr */) {
  ; // Do nothing.
}
#endif // WITH_PNG

static int EncodeImageToPng(
    const MinImg            &image,
    OutputStreamInterface   &output) {
#if !defined(WITH_PNG)
  SUPPRESS_UNUSED_VARIABLE(image);
  SUPPRESS_UNUSED_VARIABLE(output);
  return -1;
#else
  if (!image.pScan0) {
    return -2;
  }

  if (image.channelDepth != 1 || image.format != FMT_UINT) {
    return -2;
  }
  if (image.channels != 1 && image.channels != 3 && image.channels != 4) {
    return -2;
  }


  png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
  if (!png_ptr) {
    return -3;
  }

  png_infop info_ptr= png_create_info_struct(png_ptr);
  if (!info_ptr) {
    png_destroy_write_struct(&png_ptr, 0);
    return -3;
  }

  if (setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_write_struct(&png_ptr, &info_ptr);
    return -3;
  }

  int depth = std::max(1, image.channelDepth * 8);
  int color_type = PNG_COLOR_TYPE_GRAY;
  if (image.channels == 3) {
    color_type = PNG_COLOR_TYPE_RGB;
  } else if (image.channels == 4) {
    color_type = PNG_COLOR_TYPE_RGBA;
  }
  png_set_IHDR(png_ptr, info_ptr, image.width, image.height,
               depth, color_type, PNG_INTERLACE_NONE, 0, 0);

  uint32_t xDPM = 0, yDPM = 0;
  // const double DPI_FACTOR = 39.37007874015748;
  // if (pProps != 0)
  // {
  //   xDPM = static_cast<uint32_t>(pProps->xDPI * DPI_FACTOR + 0.5);
  //   yDPM = static_cast<uint32_t>(pProps->yDPI * DPI_FACTOR + 0.5);
  // }

  png_set_pHYs(png_ptr, info_ptr, xDPM , yDPM, PNG_OFFSET_PIXEL);
  png_set_write_fn(png_ptr, &output, PngWriteHelper, PngFlushHelper);
  png_write_info(png_ptr, info_ptr);

  std::vector<png_bytep> row_ptrs(image.height);
  for (int y = 0; y < image.height; ++y) {
    row_ptrs[y] = image.pScan0 + y * image.stride;
  }
  png_write_image(png_ptr, &row_ptrs[0]);
  png_write_end(png_ptr, info_ptr);
  png_destroy_write_struct(&png_ptr, &info_ptr);

  return 0;
#endif // WITH_PNG
}

static int EncodeImageToWebp(
    const MinImg            &image,
    OutputStreamInterface   &output) {
#if !defined(WITH_WEBP)
  SUPPRESS_UNUSED_VARIABLE(image);
  SUPPRESS_UNUSED_VARIABLE(output);
  return -1;
#else
  if (!image.pScan0) {
    return -2;
  }

  // Not supporting images like this.
  if (image.channelDepth != 1 || image.format != FMT_UINT ||
      image.channels != 3) {
    return -2;
  }

  uint8_t *buffer = NULL;
  size_t encoded_size = WebPEncodeLosslessRGB(image.pScan0,
                                              image.width,
                                              image.height,
                                              image.stride,
                                              &buffer);
  if (encoded_size == 0) {
    return -3;
  }

  int ret_code = output.WriteBytes(buffer, encoded_size);
  ::free(buffer);
  if (ret_code) {
    return -1;
  }

  return 0;
#endif // WITH_WEBP
}

int EncodeImage(const MinImg            &image,
                ImageFormat             format,
                OutputStreamInterface   &output) {
  switch (format) {
    case FORMAT_TIFF:
      return EncodeImageToTiff(image, output);
    case FORMAT_JPEG:
      return EncodeImageToJpeg(image, output);
    case FORMAT_PNG:
      return EncodeImageToPng(image, output);
    case FORMAT_WEBP:
      return EncodeImageToWebp(image, output);
    default:
      return -1;
  }
}

} // image_io

} // namespace se
