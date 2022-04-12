/*
 * IMPLEMENTATION OF BITPACK INTERFACE (bitpack.h)
 */
#ifndef BITPACK_INCLUDED
#define BITPACK_INCLUDED
#include "bitpack.h"
#include "assert.h"
#include "except.h"
#include <stdbool.h>
#include <stdint.h>

Except_T Bitpack_Overflow = {"Overflow packing bits"};

bool Bitpack_fitsu(uint32_t n, unsigned width)
{
    assert(width <= 32);

    unsigned c = 0;
    while (n != 0) {
        n >>= 1;
        c++;
    }
    return c <= width;
}

uint32_t Bitpack_getu(uint32_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 32);

    return word << (32 - width - lsb) >> (32 - width);
}

uint32_t Bitpack_newu(uint32_t word, unsigned width, unsigned lsb,
                      uint32_t value)
{
    assert(width + lsb <= 32);
    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
    }

    uint32_t mask = (~0u) << (32u - width) >> (32u - width - lsb);
    return (word & ~mask) | (value << lsb);
}

extern Except_T Bitpack_Overflow;
#endif
