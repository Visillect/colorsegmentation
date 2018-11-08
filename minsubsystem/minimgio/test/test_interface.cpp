#include <gtest/gtest.h>
#include <minbase/minresult.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/imgguard.hpp>
#include <minimgio/minimgio.h>


static void check_interface(
  bool const is_supported,
  bool const has_pages,
  std::string const extension)
{
  MinImg tmp = { 0 };
  DECLARE_GUARDED_MINIMG(tmp2);
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&tmp2, 1, 1, 3, TYP_UINT8));

  std::string const filename_ = "non_existing_file." + extension;
  const char* filename = filename_.c_str();

  ASSERT_EQ(BAD_ARGS, GetMinImageFileProps(NULL, filename));
  if (is_supported) {
    ASSERT_EQ(FILE_ERROR, GetMinImageFileProps(&tmp, filename));
    ASSERT_EQ(BAD_ARGS, LoadMinImage(NULL, filename));
    ASSERT_EQ(FILE_ERROR, LoadMinImage(&tmp2, filename));
    if (has_pages)
      ASSERT_EQ(FILE_ERROR, GetMinImageFilePages(filename));
    else
      ASSERT_EQ(1, GetMinImageFilePages(filename));
  } else {
    ASSERT_EQ(NOT_SUPPORTED, GetMinImageFileProps(&tmp, filename));
    ASSERT_EQ(NOT_SUPPORTED, LoadMinImage(NULL, filename));
    ASSERT_EQ(NOT_SUPPORTED, LoadMinImage(&tmp2, filename));
    ASSERT_EQ(NOT_SUPPORTED, SaveMinImage(filename, &tmp2));
  }
}

TEST(TestMinimgio, test_interface_common) {
  MinImg tmp = { 0 };
  ASSERT_EQ(BAD_ARGS, GetMinImageFileProps(NULL, NULL));
  ASSERT_EQ(FILE_ERROR, GetMinImageFileProps(&tmp, "non_existing_file.mp3"));
  ASSERT_EQ(BAD_ARGS, LoadMinImage(NULL, NULL));
  ASSERT_EQ(FILE_ERROR, LoadMinImage(&tmp, "non_existing_file.mp3"));
  ASSERT_EQ(BAD_ARGS, GetMinImageFilePages(NULL));
}

TEST(TestMinimgio, test_interface_jpeg) {
#ifdef WITH_JPEG
  check_interface(true, false, "jpg");
#else
  check_interface(false, false, "jpg");
#endif
}

TEST(TestMinimgio, test_interface_png) {
#ifdef WITH_PNG
  check_interface(true, false, "png");
#else
  check_interface(false, false, "png");
#endif
}

TEST(TestMinimgio, test_interface_tiff) {
#ifdef WITH_TIFF 
  check_interface(true, true, "tif");
#else
  check_interface(false, true, "tif");
#endif
}

TEST(TestMinimgio, test_interface_webp) {
#ifdef WITH_WEBP
  check_interface(true, false, "webp");
#else
  check_interface(false, false, "webp");
#endif
}

TEST(TestMinimgio, test_pack_and_unpack) {
  DECLARE_GUARDED_MINIMG(src);
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&src, 16, 16, 1, TYP_UINT8));
  ASSERT_EQ(NO_ERRORS, ZeroFillMinImage(&src));
  for (int idx = 0; idx < 16; ++idx)
    GetMinImageLine(&src, idx)[idx] = 0xFFU;
  DECLARE_GUARDED_MINIMG(dst_1bit);
  ASSERT_EQ(NO_ERRORS, CloneRetypifiedMinImagePrototype(&dst_1bit, &src, TYP_UINT1));
  ASSERT_EQ(NO_ERRORS, PackMinImage(&dst_1bit, &src, 10));
  for (int y = 0; y < src.height; ++y)
  {
    uint8_t const* line = GetMinImageLine(&dst_1bit, y);
    for (int x = 0; x < src.width; ++x)
    {
      uint8_t const the_bit = 0x80 >> x % 8;
      bool const img_has_bit = (line[x / 8] & the_bit) != 0;
      ASSERT_EQ(x == y, img_has_bit);
    }
  }
  DECLARE_GUARDED_MINIMG(dst_8bit);
  ASSERT_EQ(NO_ERRORS, CloneMinImagePrototype(&dst_8bit, &src));
  ASSERT_EQ(NO_ERRORS, UnpackMinImage(&dst_8bit, &dst_1bit));
  ASSERT_EQ(NO_ERRORS, CompareMinImages(&dst_8bit, &src));
}



int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
