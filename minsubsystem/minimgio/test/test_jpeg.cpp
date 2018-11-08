#include "common.hpp"
#include "quantile_diff.hpp"

static void test_jpeg_quality(const MinImg &original_img, const int quality, const int err)
{
  std::string const fn = std::string(std::tmpnam(NULL)) + ".Jpeg";
  FileRemover file_remover(fn);
  const ExtImgProps props = {
    IFF_JPEG, IFC_NONE, 0.f, 0.f, quality
  };
  ASSERT_EQ(NO_ERRORS, SaveMinImageEx(fn.c_str(), &original_img, &props));
  DECLARE_GUARDED_MINIMG(loaded_image);
  ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
  ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, &original_img));
  ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
  ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));
  uint8_t the_diff = 0;
  quantile_diff(&the_diff, &loaded_image, &original_img);
  ASSERT_LE(the_diff, err);
}

static void test_jpeg(const MinImg &original_img)
{
  SKIP_IF(!tmp_is_writeable);
  test_jpeg_quality(original_img, 100, 2);
  test_jpeg_quality(original_img, 75, 3);
  test_jpeg_quality(original_img, 50, 5);
  test_jpeg_quality(original_img, 25, 7);
  test_jpeg_quality(original_img, 1, 30);
}

template<typename T>
static void test_jpeg(int const channels)
{
  DECLARE_GUARDED_MINIMG(original_img);
  create_test_image<T>(&original_img, channels);
  test_jpeg(original_img);
}

TEST(TestMinimgio, jpeg_grayscale) {
  test_jpeg<uint8_t>(1);
}

TEST(TestMinimgio, jpeg_rgb) {
  test_jpeg<uint8_t>(3);
}

TEST(TestMinimgio, tiff_props) {
  minimgio_test_props(IFF_JPEG, ".jpeg");
}

int main(int argc, char **argv) {
  tmp_is_writeable = check_tmp_is_writeable();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
