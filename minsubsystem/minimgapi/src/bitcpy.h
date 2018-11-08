#pragma once
#ifndef MINIMGAPI_SRC_BITCPY_H_INCLUDED
#define MINIMGAPI_SRC_BITCPY_H_INCLUDED

void bitcpy(
    uint8_t       *p_dst,
    int            dst_shift,
    const uint8_t *p_src,
    int            src_shift,
    int            size);

#endif // #ifndef MINIMGAPI_SRC_BITCPY_H_INCLUDED
