#include <gtest/gtest.h>
#include <minbase/minresult.h>
#include <minimgapi/minimgapi.h>
#include <minimgapi/minimgapi-inl.h>
#include <minimgapi/imgguard.hpp>

TEST(TestMinimgapi, TestCompareMinImages) {
  uint8_t data_a[14], data_b[15];
  for (int i = 0; i < 14; ++i)
    data_a[i] = rand() & 0xFFU;
  ::memcpy(data_b, data_a, 14);
  MinImg image_a = {0}, image_b = {0};
  ASSERT_EQ(NO_ERRORS, _WrapAlignedBufferWithMinImage(&image_a, data_a,
                                                      2, 2, 3, TYP_UINT8, 8));
  EXPECT_GT(CompareMinImages(&image_a, &image_b), 0);
  ASSERT_EQ(NO_ERRORS, _WrapAlignedBufferWithMinImage(&image_b, data_b,
                                                      2, 2, 3, TYP_UINT8, 7));
  EXPECT_GT(CompareMinImages(&image_a, &image_b), 0);
  ASSERT_EQ(NO_ERRORS, CopyMinImage(&image_b, &image_a));
  EXPECT_EQ(NO_ERRORS, CompareMinImages(&image_a, &image_b));
  ++data_b[11];
  EXPECT_GT(CompareMinImages(&image_a, &image_b), 0);
}

TEST(TestMinimgapi, TestCopyMinImageFragment) {
  DECLARE_GUARDED_MINIMG(dst_image);
  DECLARE_GUARDED_MINIMG(src_image);
  EXPECT_EQ(NO_ERRORS, NewMinImagePrototype(&dst_image, 100, 70, 3, TYP_UINT8));
  EXPECT_EQ(NO_ERRORS, NewMinImagePrototype(&src_image, 200, 60, 3, TYP_UINT8));
  EXPECT_EQ(NO_ERRORS, CopyMinImageFragment(&dst_image, &src_image,
                                                         7, 23, 55, 14, 3, 18));
}

TEST(TestMinimgapi, TestCheckMinImagesTangle) {
  MinImg src = {0};
  MinImg dst = {0};
  uint32_t result = 0;
  EXPECT_EQ(NO_ERRORS, CheckMinImagesTangle(&result, &src, &dst));
  EXPECT_EQ(TCR_SAME_IMAGE, result);

  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&src, 13, 100, 1, TYP_UINT8, 0, AO_EMPTY));
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&dst, 13, 100, 1, TYP_UINT8, 0, AO_EMPTY));

  EXPECT_EQ(BAD_ARGS, CheckMinImagesTangle(&result, &src, &dst));
  src.stride = dst.stride = dst.width;

  uint8_t *original_pScan0 = reinterpret_cast<uint8_t *>(0x4000000);
  src.pScan0 = original_pScan0;
  dst.pScan0 = original_pScan0 + dst.stride;

  EXPECT_EQ(NO_ERRORS, CheckMinImagesTangle(&result, &src, &dst));
  EXPECT_EQ(static_cast<uint32_t>(TCR_FORWARD_PASS_POSSIBLE |
                                  TCR_INDEPENDENT_LINES), result);

  EXPECT_EQ(NO_ERRORS, CheckMinImagesTangle(&result, &dst, &src));
  EXPECT_EQ(TCR_TANGLED_IMAGES, result);

  src.pScan0 = original_pScan0 + 1;
  src.stride = 14;
  dst.pScan0 = original_pScan0;
  src.stride = 14;

  EXPECT_EQ(NO_ERRORS, CheckMinImagesTangle(&result, &dst, &src));
  EXPECT_EQ(TCR_FORWARD_PASS_POSSIBLE, result);
}

TEST(TestMinimgapi, TestGetMinImgLine) {
  if (sizeof(void*) == 8) {
    MinImg src = { 0 };
    ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&src, 1, 512, 1, TYP_UINT8, 0, AO_EMPTY));
    src.pScan0 = reinterpret_cast<uint8_t *>(0x4000000);
    src.stride = std::numeric_limits<int32_t>::max();
    EXPECT_EQ(src.pScan0, GetMinImageLine(&src, 0));
    EXPECT_EQ(src.pScan0 + static_cast<size_t>(src.stride) * 511, GetMinImageLine(&src, 511));
    EXPECT_EQ(src.pScan0 + static_cast<size_t>(src.stride) * 511, GetMinImageLine(&src, 511, BO_REPEAT));
    EXPECT_EQ(src.pScan0 + static_cast<size_t>(src.stride) * 512, GetMinImageLine(&src, 512, BO_IGNORE));
    src.width = 0;
    EXPECT_EQ(src.pScan0 + static_cast<size_t>(src.stride) * 512, GetMinImageLine(&src, 512, BO_IGNORE));
  }
}

TEST(TestMinimgapi, TestCopyMinImageChannels34) {
  DECLARE_GUARDED_MINIMG(dst);
  DECLARE_GUARDED_MINIMG(src);
  // 1200x589 - Size matters!
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&dst, 1200, 589, 4, TYP_UINT8));
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&src, 1200, 589, 3, TYP_UINT8));
  ASSERT_EQ(NO_ERRORS, ZeroFillMinImage(&dst));
  const uint8_t fill = 0xF0U;
  ASSERT_EQ(NO_ERRORS, FillMinImage(&src, &fill, sizeof(fill)));
  const int channels[] = { 0, 1, 2 };
  ASSERT_EQ(NO_ERRORS, CopyMinImageChannels(&dst, &src, channels, channels, 3));
  for (int y = 0; y < dst.height; ++y) {
    const uint8_t *px = dst.pScan0 + dst.stride * y;
    for (int x = 0; x < dst.width; ++x, px +=4) {
      ASSERT_TRUE(
        px[0] == fill && px[1] == fill && px[2] == fill && px[3] == 0);
    }
  }
}

TEST(TestMinimgapi, TestCopyMinImageChannels43) {
  DECLARE_GUARDED_MINIMG(dst);
  DECLARE_GUARDED_MINIMG(src);
  // 1200x589 - Size matters!
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&dst, 1200, 589, 3, TYP_UINT8));
  ASSERT_EQ(NO_ERRORS, NewMinImagePrototype(&src, 1200, 589, 4, TYP_UINT8));
  ASSERT_EQ(NO_ERRORS, ZeroFillMinImage(&dst));
  const uint8_t fill = 0xF1U;
  ASSERT_EQ(NO_ERRORS, FillMinImage(&src, &fill, sizeof(fill)));
  const int channels[] = { 0, 1, 2 };
  ASSERT_EQ(NO_ERRORS, CopyMinImageChannels(&dst, &src, channels, channels, 3));
  for (int y = 0; y < dst.height; ++y) {
    const uint8_t *px = dst.pScan0 + dst.stride * y;
    for (int x = 0; x < dst.width; ++x, px += 3) {
      ASSERT_TRUE(px[0] == fill && px[1] == fill && px[2] == fill);
    }
  }
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
