#include <minimgapi/minimgapi.h>
#include "bitcpy.h"

void bitcpy(
    uint8_t       *p_dst,
    int            dst_shift,
    const uint8_t *p_src,
    int            src_shift, 
    int            size) {
  for (int i = 0; i < size; ++i)
    if (GET_IMAGE_LINE_BIT(p_src, src_shift + i))
      SET_IMAGE_LINE_BIT(p_dst, dst_shift + i);
    else
      CLEAR_IMAGE_LINE_BIT(p_dst, dst_shift + i);
}