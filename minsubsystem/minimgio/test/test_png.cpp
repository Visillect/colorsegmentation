#include "common.hpp"


static void test_png(MinImg const& original_img)
{
  SKIP_IF(!tmp_is_writeable);
  std::string const fn = std::string(std::tmpnam(NULL)) + ".pnG";
  FileRemover file_remover(fn);
  ASSERT_EQ(NO_ERRORS, SaveMinImage(fn.c_str(), &original_img));
  {
    DECLARE_GUARDED_MINIMG(loaded_image);
    ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
    ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, &original_img));
    ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
    ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));
    ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image, &original_img));
  }
  if (original_img.channels == 2 || original_img.channels == 4)
  {  // check that user can strip alpha channel
    DECLARE_GUARDED_MINIMG(loaded_image);
    ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
    ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, &original_img));
    loaded_image.channels -= 1;
    ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
    ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));

    DECLARE_GUARDED_MINIMG(stripped_img);
    ASSERT_EQ(NO_ERRORS, CloneDimensionedMinImagePrototype(
      &stripped_img, &original_img, original_img.channels - 1));
    const int channel_to_copy[] = { 0, 1, 2 };
    ASSERT_EQ(NO_ERRORS, CopyMinImageChannels(
      &stripped_img, &original_img, channel_to_copy, channel_to_copy,
      original_img.channels - 1));

    ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image, &stripped_img));
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
  if (GetMinImageType(&original_img) == TYP_UINT16)
  {  // check that user can convert to uint8_t
    DECLARE_GUARDED_MINIMG(loaded_image);
    ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&loaded_image, fn.c_str()));
    ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&loaded_image, &original_img));
    loaded_image.channelDepth -= 1;
    ASSERT_EQ(NO_ERRORS, AllocMinImage(&loaded_image));
    ASSERT_EQ(NO_ERRORS, LoadMinImage(&loaded_image, fn.c_str()));

    DECLARE_GUARDED_MINIMG(typified_img);
    ASSERT_EQ(NO_ERRORS, CloneRetypifiedMinImagePrototype(
      &typified_img, &original_img, TYP_UINT8));
    libpng_uint16_to_uint8(&typified_img, &original_img);
    ASSERT_EQ(NO_ERRORS, CompareMinImages(&loaded_image, &typified_img));
  }
}

template<typename T>
static void test_png(int const channels)
{
  DECLARE_GUARDED_MINIMG(original_img);
  create_test_image<T>(&original_img, channels);
  test_png(original_img);
}

TEST(TestMinimgio, png_uint1_1ch) {
  test_png<bool>(1);
}

TEST(TestMinimgio, png_uint8_1ch) {
  test_png<uint8_t>(1);
}

TEST(TestMinimgio, png_uint8_2ch) {
  test_png<uint8_t>(2);
}

TEST(TestMinimgio, png_uint8_3ch) {
  test_png<uint8_t>(3);
}

TEST(TestMinimgio, png_uint8_4ch) {
  test_png<uint8_t>(4);
}

TEST(TestMinimgio, png_uint16_1ch) {
  test_png<uint16_t>(1);
}

TEST(TestMinimgio, png_uint16_2ch) {
  test_png<uint16_t>(2);
}

TEST(TestMinimgio, png_uint16_3ch) {
  test_png<uint16_t>(3);
}

TEST(TestMinimgio, png_uint16_4ch) {
  test_png<uint16_t>(4);
}

TEST(TestMinimgio, png_memory) {
  SKIP_IF(!tmp_is_writeable);
  DECLARE_GUARDED_MINIMG(original_img);
  create_test_image<uint8_t>(&original_img);

  std::string const fn = std::string(std::tmpnam(NULL)) + ".png";
  FileRemover file_remover(fn);
  ASSERT_EQ(NO_ERRORS, SaveMinImage(fn.c_str(), &original_img));
  std::string file_contents;
  {
    std::ifstream t(fn.c_str(), std::ios_base::binary);
    std::stringstream buffer;
    buffer << t.rdbuf();
    file_contents = buffer.str();
  }

  DECLARE_GUARDED_MINIMG(in_memory_img);
  char mem_path[64];
  sprintf(mem_path, "mem://%p.%lu.png", file_contents.c_str(),
    static_cast<unsigned long>(file_contents.size()));
  ASSERT_EQ(NO_ERRORS, GetMinImageFileProps(&in_memory_img, mem_path));
  ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(&original_img, &in_memory_img));
  ASSERT_EQ(NO_ERRORS, AllocMinImage(&in_memory_img));
  ASSERT_EQ(NO_ERRORS, LoadMinImage(&in_memory_img, mem_path));
  ASSERT_EQ(NO_ERRORS, CompareMinImages(&in_memory_img, &original_img));
}

TEST(TestMinimgio, png_props) {
  minimgio_test_props(IFF_PNG, ".png");
}


int main(int argc, char **argv) {
  tmp_is_writeable = check_tmp_is_writeable();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
