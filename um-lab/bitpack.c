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

bool Bitpack_fitsu(uint64_t n, unsigned width)
{
    assert(width <= 64);

    unsigned c = 0;
    while (n != 0) {
        n >>= 1;
        c++;
    }
    return c <= width;
}

bool Bitpack_fitss(int64_t n, unsigned width)
{
    assert(width <= 64);

    if (n >= 0) {
        return Bitpack_fitsu((uint64_t)n, width - 1);
    }

    return Bitpack_fitsu((uint64_t)(~n), width - 1);
}

uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);

    return word << (64 - width - lsb) >> (64 - width);
}

int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb)
{
    assert(width + lsb <= 64);

    return ((int64_t)word) << (64 - width - lsb) >> (64 - width);
}

uint64_t Bitpack_newu(uint64_t word, unsigned width, unsigned lsb,
                      uint64_t value)
{
    assert(width + lsb <= 64);
    if (!Bitpack_fitsu(value, width)) {
        RAISE(Bitpack_Overflow);
    }

    uint64_t mask = (~0u) << (64u - width) >> (64u - width - lsb);
    return (word & ~mask) | (value << lsb);
}

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb,
                      int64_t value)
{
    assert(width + lsb <= 64);
    if (!Bitpack_fitss(value, width)) {
        RAISE(Bitpack_Overflow);
    }

    uint64_t mask = (~0u) << (64u - width) >> (64u - width - lsb);
    value <<= 64u - width;
    return (word & ~mask) | (((uint64_t)value) >> (64u - width - lsb));
}

extern Except_T Bitpack_Overflow;
#endif
