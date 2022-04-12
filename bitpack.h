#ifndef BITPACK_INCLUDED
#define BITPACK_INCLUDED

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

bool Bitpack_fitsu(uint32_t n, unsigned width);
uint32_t Bitpack_getu(uint32_t word, unsigned width, unsigned lsb);
uint32_t Bitpack_newu(uint32_t word, unsigned width, unsigned lsb,
                      uint32_t value);

#endif