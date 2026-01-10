// rge256ctr.c
// C99 implementation of RGE256ctr:
// 256 bit key, ChaCha-style ARX core, 64 bit counter.
// Not intended for cryptographic use.

#include <stdint.h>
#include <stdio.h>

// Rotate left (32 bit)
static inline uint32_t rotl32(uint32_t x, uint32_t r) {
    return (x << r) | (x >> (32 - r));
}

// Quarter round
static inline void qr(uint32_t x[16], int a, int b, int c, int d) {
    x[a] += x[b];  x[d] ^= x[a];  x[d] = rotl32(x[d], 16);
    x[c] += x[d];  x[b] ^= x[c];  x[b] = rotl32(x[b], 12);
    x[a] += x[b];  x[d] ^= x[a];  x[d] = rotl32(x[d], 8);
    x[c] += x[d];  x[b] ^= x[c];  x[b] = rotl32(x[b], 7);
}

typedef struct {
    uint32_t c[4];     // constants
    uint32_t key[8];   // 256 bit key
    uint64_t counter;  // 64 bit counter
} rge256ctr_state;

// Seed expansion (simple LCG, deterministic)
void rge256ctr_init(rge256ctr_state *s, uint32_t seed) {
    s->c[0] = 0x61707865;
    s->c[1] = 0x3320646E;
    s->c[2] = 0x79622D32;
    s->c[3] = 0x6B206574;

    uint32_t x = seed;
    for (int i = 0; i < 8; i++) {
        x = x * 1664525 + 1013904223;
        s->key[i] = x;
    }

    s->counter = 0;
}

// Generate one 32 bit value
uint32_t rge256ctr_next32(rge256ctr_state *s) {
    uint32_t state[16];

    state[0] = s->c[0];
    state[1] = s->c[1];
    state[2] = s->c[2];
    state[3] = s->c[3];

    for (int i = 0; i < 8; i++)
        state[4 + i] = s->key[i];

    uint64_t ctr = s->counter++;
    state[12] = (uint32_t)(ctr & 0xFFFFFFFFu);
    state[13] = (uint32_t)(ctr >> 32);
    state[14] = s->key[0] ^ state[12];
    state[15] = s->key[1] ^ state[13];

    uint32_t w[16];
    for (int i = 0; i < 16; i++)
        w[i] = state[i];

    // Six ARX rounds
    for (int i = 0; i < 6; i++) {
        qr(w, 0,4,8,12);
        qr(w, 1,5,9,13);
        qr(w, 2,6,10,14);
        qr(w, 3,7,11,15);

        qr(w, 0,5,10,15);
        qr(w, 1,6,11,12);
        qr(w, 2,7,8,13);
        qr(w, 3,4,9,14);
    }

    return (w[0] + state[0]);
}

// Write an infinite stream of random bytes to stdout
int main() {
    rge256ctr_state g;
    rge256ctr_init(&g, 123456789u);

    for (;;) {
        uint32_t x = rge256ctr_next32(&g);
        fwrite(&x, sizeof(uint32_t), 1, stdout);
    }
    return 0;
}
