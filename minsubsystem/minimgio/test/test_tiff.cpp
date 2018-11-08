#include "common.hpp"
#include "quantile_diff.hpp"

static void test_tiff(MinImg const& original_img)
{
  SKIP_IF(!tmp_is_writeable);
  std::string const fn = std::string(std::tmpnam(NULL)) + ".tiFf";
  FileRemover file_remover(fn);
  ASSERT_EQ(NO_ERRORS, SaveMinImage(fn.c_str(), &original_img));
  DECLARE_GUARDED_MINIMG(loaded_image);
  ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
  ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, &original_img));
  ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
  ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));
  ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image, &original_img));
  if (original_img.channels == 2)  // to not check for all possible channels
  {  // test `page` support
    const int page = 1;
    MinImg original_img_region = {0};
    ASSERT_EQ(NO_ERRORS, GetMinImageRegion(
      &original_img_region, &original_img, 0, 0, 8, 9));
    ASSERT_EQ(NO_ERRORS, SaveMinImage(fn.c_str(), &original_img_region, page));
    DECLARE_GUARDED_MINIMG(loaded_image2);
    ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image2, fn.c_str(), page));
    ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image2, &original_img_region));
    ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image2));
    ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image2, fn.c_str(), page));
    ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image2, &original_img_region));
  }

  if (GetMinImageType(&original_img) == TYP_UINT1)
  { // check that user can convert to uint8_t
    DECLARE_GUARDED_MINIMG(loaded_image);
    ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
    ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, &original_img));
    ASSERT_EQ(NO_ERRORS, SetMinImageType(&loaded_image, TYP_UINT8));
    ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
    ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));

    DECLARE_GUARDED_MINIMG(unpacked_img);
    ASSERT_EQ(NO_ERRORS, CloneRetypifiedMinImagePrototype(
      &unpacked_img, &original_img, TYP_UINT8));
    ASSERT_EQ(NO_ERRORS, UnpackMinImage(&unpacked_img, &original_img));
    ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image, &unpacked_img));
  }
}

template<typename T>
static void test_tiff(int const channels)
{
  DECLARE_GUARDED_MINIMG(original_img);
  create_test_image<T>(&original_img, channels);
  test_tiff(original_img);
}

#define MINIMGIO_TYPE_CHECK(type) \
TEST(TestMinimgio, tiff_##type) { \
  test_tiff<type>(1);        \
  test_tiff<type>(2);        \
  test_tiff<type>(3);        \
  test_tiff<type>(4);        \
  test_tiff<type>(17);       \
}

MINIMGIO_TYPE_CHECK(bool);
MINIMGIO_TYPE_CHECK(uint8_t);
MINIMGIO_TYPE_CHECK(int8_t);
MINIMGIO_TYPE_CHECK(uint16_t);
MINIMGIO_TYPE_CHECK(int16_t);
MINIMGIO_TYPE_CHECK(uint32_t);
MINIMGIO_TYPE_CHECK(int32_t);
MINIMGIO_TYPE_CHECK(real32_t);
MINIMGIO_TYPE_CHECK(uint64_t);
MINIMGIO_TYPE_CHECK(int64_t);
MINIMGIO_TYPE_CHECK(real64_t);

TEST(TestMinimgio, tiff_props) {
  minimgio_test_props(IFF_TIFF, ".tif");
}

static void _tiff_compression(MinImg const* img, ImgFileComp comp)
{
  std::string const fn = std::string(std::tmpnam(NULL)) + ".tiFf";
  ExtImgProps props;
  props.iff = IFF_TIFF;
  props.comp = comp;
  props.qty = 100;
  ASSERT_EQ(NO_ERRORS, SaveMinImageEx(fn.c_str(), img, &props));
  DECLARE_GUARDED_MINIMG(loaded_image);
  ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
  ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, img));
  ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
  ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));
  if (comp == IFC_JPEG) {
    uint8_t the_diff;
    quantile_diff(&the_diff, &loaded_image, img);
    ASSERT_LE(the_diff, 3);
  } else {
    ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image, img));
  }
}

template<typename T>
static void _tiff_compression(ImgFileComp comp)
{
  DECLARE_GUARDED_MINIMG(original_img);
  create_test_image<T>(&original_img, 2);
  _tiff_compression(&original_img, comp);
}

#define MINIMGIO_COMP_TEST(COMP, TYPE)        \
TEST(TestMinimgio, tiff_compression_##COMP) { \
  _tiff_compression<TYPE>(IFC_##COMP);        \
}                                             \

MINIMGIO_COMP_TEST(NONE, uint8_t);
#ifdef WITH_TIFF_LZW
MINIMGIO_COMP_TEST(LZW, uint8_t);
#endif
#ifdef WITH_TIFF_ZIP
MINIMGIO_COMP_TEST(DEFLATE, uint8_t);
#endif
MINIMGIO_COMP_TEST(PACKBITS, uint8_t);
#ifdef WITH_TIFF_JPEG
MINIMGIO_COMP_TEST(JPEG, uint8_t);
#endif
MINIMGIO_COMP_TEST(RLE, bool);
MINIMGIO_COMP_TEST(GROUP3, bool);
MINIMGIO_COMP_TEST(GROUP4, bool);


int main(int argc, char **argv) {
  tmp_is_writeable = check_tmp_is_writeable();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
