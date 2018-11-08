#include <gtest/gtest.h>
#include <minbase/minresult.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/imgguard.hpp>
#include <minimgapi/minimgapi-helpers.hpp>
#include <minimgio/minimgio.h>
#include <minutils/smartptr.h>
#include <cstdio>  // for sprintf
#include <limits>
#include <fstream>


// LETTERs for image
static const uint8_t LETTER_a[] = { 0x00, 0x00, 0xF8, 0x0C, 0x7C, 0xCC, 0xF6, 0x00 };
static const uint8_t LETTER_B[] = { 0xFC, 0x66, 0x66, 0x7C, 0x66, 0x66, 0xFC, 0x00 };
static const uint8_t LETTER_C[] = { 0x7E, 0xC6, 0xC0, 0xC0, 0xC2, 0xC6, 0x7C, 0x00 };
static const uint8_t LETTER_d[] = { 0x1C, 0x0C, 0x6C, 0xDC, 0xCC, 0xCC, 0x76, 0x00 };
static const uint8_t LETTER_e[] = { 0x00, 0x00, 0x7C, 0xC6, 0xFE, 0xC0, 0x7C, 0x00 };
static const uint8_t LETTER_l[] = { 0x38, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00 };
static const uint8_t LETTER_n[] = { 0x00, 0x00, 0xDC, 0x66, 0x66, 0x66, 0xEE, 0x00 };
static const uint8_t LETTER_o[] = { 0x00, 0x00, 0x7C, 0xC6, 0xC6, 0xC6, 0x7C, 0x00 };
static const uint8_t LETTER_R[] = { 0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0xE6, 0x00 };
static const uint8_t LETTER_u[] = { 0x00, 0x00, 0xEE, 0x66, 0x66, 0x66, 0x3A, 0x00 };
static const uint8_t LETTER_w[] = { 0x00, 0x00, 0xE6, 0xC2, 0xDA, 0x6C, 0x64, 0x00 };
static const uint8_t LETTER_y[] = { 0x00, 0x00, 0xE6, 0x62, 0x34, 0x18, 0x08, 0x70 };
static const uint8_t LETTER_Y[] = { 0xE6, 0xC2, 0x6C, 0x68, 0x30, 0x30, 0x78, 0x00 };

template<typename T>
void draw_letters(const MinImg *img, const uint8_t* const *letters, const size_t n)
{
  for (int y = 0; y < 8; ++y) {
    T* line = GetMinImageLineAs<T>(img, y, BO_IGNORE);
    for (size_t letter = 0; letter < n; ++letter) {
      uint8_t chr = letters[letter][y];
      for (int x = 0; x < 8; ++x)
        if (chr & (0x80 >> x))
          for (int c = 0; c < img->channels; ++c)
            line[x * img->channels + c] = 0;
      line += 8 * img->channels;
    }
  }
}

static bool tmp_is_writeable;
#define SKIP_IF(CHECK) if (CHECK) { \
  std::cout << "[  SKIPPED ] " << #CHECK << "\n"; return;} \

static void libpng_uint16_to_uint8(MinImg const *dst, MinImg const *src)
{
  ASSERT_EQ(TYP_UINT16, GetMinImageType(src));
  ASSERT_EQ(TYP_UINT8, GetMinImageType(dst));
  ASSERT_EQ(NO_ERRORS, CompareMinImage3DSizes(dst, src));
  for (int y = 0; y < src->height; ++y)
  {
    const uint16_t *src_line = GetMinImageLineAs<uint16_t>(src, y);
    uint8_t* dst_line = GetMinImageLineAs<uint8_t>(dst, y);
    for (int x = 0; x < src->width * src->channels; ++x)
      dst_line[x] = static_cast<uint8_t>(src_line[x] >> 8);
  }
}

template<typename T>
STATIC_SPECIAL void create_test_image(
  MinImg* img, const int channels=3, const int width=600, const int height=300)
{
  const T min_val = std::numeric_limits<T>::min();
  const T max_val = std::numeric_limits<T>::max();
  const real32_t range =
    static_cast<real32_t>(max_val) - static_cast<real32_t>(min_val);

  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(
    img, width, height, channels, GetMinTypByCType<T>()));
  scoped_cpp_array<T> R(new T[img->width]);
  for (int x = 0; x < width; ++x)
    R[x] = static_cast<T>(
      static_cast<real32_t>(x) / static_cast<real32_t>(width - 1) * range);
  for (int y = 0; y < height; ++y) {
    const T G = static_cast<T>((static_cast<real32_t>(y) / static_cast<real32_t>(height - 1)) * range);
    T *dst_line = GetMinImageLineAs<T>(img, y);
    for (int x = 0; x < width; ++x)
    {
      T *pix = dst_line + x * channels;
      for (int c = 0; c < channels; ++c) {
        switch (c) {
        case 0:  pix[0] = R[x]; break;
        case 1:  pix[1] = G; break;
        case 2:  pix[2] = R[img->width - x - 1]; break;
        default: pix[c] = max_val;
        }
      }
    }
  }
  MinImg text_region = {0};
  ASSERT_EQ(NO_ERRORS, GetMinImageRegion(&text_region, img, 3, 3, 0, 8, RO_REUSE_CONTAINER));
  const uint8_t *blue[] = { LETTER_B, LETTER_l, LETTER_u, LETTER_e };
  draw_letters<T>(&text_region, blue, 4);
  ASSERT_EQ(NO_ERRORS, GetMinImageRegion(&text_region, img, img->width - 8 * 3 - 3, 3, 0, 8, RO_REUSE_CONTAINER));
  const uint8_t *red[] = { LETTER_R, LETTER_e, LETTER_d};
  draw_letters<T>(&text_region, red, 3);
  ASSERT_EQ(NO_ERRORS, GetMinImageRegion(&text_region, img, 1, img->height - 9, 0, 8, RO_REUSE_CONTAINER));
  const uint8_t *cyan[] = { LETTER_C, LETTER_y, LETTER_a, LETTER_n };
  draw_letters<T>(&text_region, cyan, 4);
  ASSERT_EQ(NO_ERRORS, GetMinImageRegion(&text_region, img, img->width - 8 * 6 - 3, img->height - 8 - 3, 0, 8, RO_REUSE_CONTAINER));
  const uint8_t *yellow[] = { LETTER_Y, LETTER_e, LETTER_l, LETTER_l, LETTER_o, LETTER_w };
  draw_letters<T>(&text_region, yellow, 6);
}

template<>
STATIC_SPECIAL void create_test_image<bool>(
  MinImg* img, int const channels, int const width, int const height)
{
  DECLARE_GUARDED_MINIMG(original_img);
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(
    &original_img, width, height, channels, TYP_UINT8));
  uint8_t bg = 0xff;
  FillMinImage(&original_img, &bg, sizeof(uint8_t));
  const uint8_t *road[] = { LETTER_R, LETTER_o, LETTER_a, LETTER_d };
  draw_letters<uint8_t>(&original_img, road, 4);
  ASSERT_EQ(NO_ERRORS, CloneRetypifiedMinImagePrototype(
    img, &original_img, TYP_UINT1));
  ASSERT_EQ(NO_ERRORS, PackMinImage(img, &original_img));
}

class FileRemover {
  const std::string &fn;
public:
  FileRemover(const std::string &fn) : fn(fn){}
  ~FileRemover() {
    std::remove(fn.c_str());
  }
};


void minimgio_test_props(
    const ImgFileFormat iff,
    const std::string &extension) {
  SKIP_IF(!tmp_is_writeable);
  DECLARE_GUARDED_MINIMG(img);
  create_test_image<uint8_t>(&img);
  const std::string fn = std::string(std::tmpnam(NULL)) + extension;
  FileRemover file_remover(fn);
  MinImg prototype = { 0 };

  const ExtImgProps save_props = {
    iff, IFC_NONE, 92.1f, 192.455f, 100
  };
  ASSERT_EQ(NO_ERRORS, SaveMinImageEx(fn.c_str(), &img, &save_props));
  ExtImgProps load_props;
  ASSERT_EQ(NO_ERRORS, GetMinImageFilePropsEx(&prototype, &load_props, fn.c_str()));
  EXPECT_EQ(load_props.iff, save_props.iff);
  EXPECT_EQ(load_props.qty, 0);
  EXPECT_EQ(load_props.comp, IFC_NONE);
  EXPECT_NEAR(load_props.xDPI, save_props.xDPI, 0.51f);
  EXPECT_NEAR(load_props.yDPI, save_props.yDPI, 0.51f);

  const ExtImgProps save_props_none = {
    IFF_UNKNOWN, IFC_NONE, .0f, .0f, 0
  };
  ASSERT_EQ(NO_ERRORS, SaveMinImageEx(fn.c_str(), &img, &save_props_none));
  ASSERT_EQ(NO_ERRORS, GetMinImageFilePropsEx(&prototype, &load_props, fn.c_str()));
  EXPECT_EQ(load_props.iff, iff);
  EXPECT_EQ(load_props.qty, 0);
  EXPECT_EQ(load_props.comp, IFC_NONE);
  EXPECT_EQ(load_props.xDPI, save_props_none.xDPI);
  EXPECT_EQ(load_props.yDPI, save_props_none.yDPI);
}

static bool check_tmp_is_writeable() {
  const std::string fn = std::string(std::tmpnam(NULL)) + ".test_minimgio";
  FILE *f = fopen(fn.c_str(), "wb");
  FileRemover file_remover(fn);
  bool ret = false;
  if (f) {
    if (fseek(f, 0xFFFF, SEEK_SET) == 0)  // chosen 0xFFFF for no real reason
      ret = true;
    fclose(f);
  }
  return ret;
}
