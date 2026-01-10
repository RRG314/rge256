// rge256ctr.h
// Header for the RGE256ctr C99 PRNG

#ifndef RGE256CTR_H
#define RGE256CTR_H

#include <stdint.h>

typedef struct {
    uint32_t c[4];      // constants
    uint32_t key[8];    // 256 bit key
    uint64_t counter;   // 64 bit block counter
} rge256ctr_state;

// Initialize PRNG with 32 bit seed
void rge256ctr_init(rge256ctr_state *s, uint32_t seed);

// Generate one 32 bit unsigned integer
uint32_t rge256ctr_next32(rge256ctr_state *s);

#endif
