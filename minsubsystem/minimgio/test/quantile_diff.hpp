static void quantile_diff(uint8_t *the_diff, const MinImg *p_image_a, const MinImg *p_image_b)
{
  ASSERT_EQ(NO_ERRORS, CompareMinImagePrototypes(p_image_a, p_image_b));
  ASSERT_EQ(TYP_UINT8, GetMinImageType(p_image_a));
  const int len = p_image_a->width * p_image_a->channels;
  *the_diff = 0;
  const uint8_t *p_line_a = p_image_a->pScan0;
  const uint8_t *p_line_b = p_image_b->pScan0;
  int hist[256] = { 0 };
  for (int y = 0; y < p_image_a->height; ++y)
  {
    for (int x = 0; x < len; ++x)
    {
      const uint8_t diff = p_line_a[x] > p_line_b[x] ? p_line_a[x] - p_line_b[x] : p_line_b[x] - p_line_a[x];
      ++hist[diff];
    }
    p_line_a += p_image_a->stride;
    p_line_b += p_image_b->stride;
  }
  const int threshold = p_image_a->height * len * 9 / 10;  // 90%
  int cnt = 0;
  int i = 0;
  for (; i < 256 && cnt < threshold; ++i) {
    cnt += hist[i];
  }
  *the_diff = i;
}
