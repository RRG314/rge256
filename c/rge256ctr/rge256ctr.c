// rge256ctr.c
// RGE256ctr: 256-bit ARX-based counter PRNG (non-cryptographic).

#include "rge256ctr.h"

#include <string.h>
#include <stdio.h>

static inline uint32_t rotl32(uint32_t x, uint32_t r) {
    return (x << r) | (x >> (32u - r));
}

static inline void qr(uint32_t x[16], int a, int b, int c, int d) {
    x[a] += x[b];  x[d] ^= x[a];  x[d] = rotl32(x[d], 16);
    x[c] += x[d];  x[b] ^= x[c];  x[b] = rotl32(x[b], 12);
    x[a] += x[b];  x[d] ^= x[a];  x[d] = rotl32(x[d], 8);
    x[c] += x[d];  x[b] ^= x[c];  x[b] = rotl32(x[b], 7);
}

static inline void set_constants(uint32_t s[16]) {
    s[0] = 0x61707865u;
    s[1] = 0x3320646Eu;
    s[2] = 0x79622D32u;
    s[3] = 0x6B206574u;
}

static void refill(rge256ctr_state *s) {
    uint32_t st[16];
    uint32_t w[16];

    set_constants(st);

    for (int i = 0; i < 8; i++)
        st[4 + i] = s->key[i];

    uint64_t ctr = s->counter++;
    st[12] = (uint32_t)(ctr & 0xFFFFFFFFu);
    st[13] = (uint32_t)(ctr >> 32);

    uint64_t n = s->nonce;
    st[14] = (uint32_t)(n & 0xFFFFFFFFu);
    st[15] = (uint32_t)(n >> 32);

    for (int i = 0; i < 16; i++)
        w[i] = st[i];

    // 12 ARX rounds (6 double-rounds)
    for (int i = 0; i < 6; i++) {
        qr(w, 0, 4,  8, 12);
        qr(w, 1, 5,  9, 13);
        qr(w, 2, 6, 10, 14);
        qr(w, 3, 7, 11, 15);

        qr(w, 0, 5, 10, 15);
        qr(w, 1, 6, 11, 12);
        qr(w, 2, 7,  8, 13);
        qr(w, 3, 4,  9, 14);
    }

    for (int i = 0; i < 16; i++)
        s->buf[i] = w[i] + st[i];

    s->buf_used = 0;
}

void rge256ctr_init(rge256ctr_state *s, uint32_t seed, uint64_t nonce) {
    memset(s, 0, sizeof(*s));

    // simple seed → 256-bit key expansion
    s->key[0] = seed;
    s->key[1] = 0x9E3779B9u;
    s->key[2] = 0x243F6A88u;
    s->key[3] = 0xB7E15162u;
    s->key[4] = 0xC6EF3720u;
    s->key[5] = 0xDEADBEEFu;
    s->key[6] = 0xA5A5A5A5u;
    s->key[7] = 0x01234567u;

    s->counter  = 0;
    s->nonce    = nonce;
    s->buf_used = 16;  // force refill on first use
}

uint32_t rge256ctr_next_u32(rge256ctr_state *s) {
    if (s->buf_used >= 16)
        refill(s);
    return s->buf[s->buf_used++];
}

void rge256ctr_fill_bytes(rge256ctr_state *s, void *out, size_t nbytes) {
    uint8_t *p = (uint8_t *)out;

    while (nbytes >= 4) {
        uint32_t w = rge256ctr_next_u32(s);
        p[0] = (uint8_t)(w & 0xFFu);
        p[1] = (uint8_t)((w >> 8) & 0xFFu);
        p[2] = (uint8_t)((w >> 16) & 0xFFu);
        p[3] = (uint8_t)((w >> 24) & 0xFFu);
        p += 4;
        nbytes -= 4;
    }

    if (nbytes > 0) {
        uint32_t w = rge256ctr_next_u32(s);
        for (size_t i = 0; i < nbytes; i++) {
            p[i] = (uint8_t)(w & 0xFFu);
            w >>= 8;
        }
    }
}

// Standalone stream used for Dieharder / NIST / other tools.
//   gcc -std=c99 -O3 rge256ctr.c -o rge256ctr
//   ./rge256ctr | dieharder -a -g 200 -f /dev/stdin
int main(void) {
    rge256ctr_state g;
    rge256ctr_init(&g, 123456789u, 0ULL);

    for (;;) {
        uint32_t x = rge256ctr_next_u32(&g);
        if (fwrite(&x, sizeof x, 1, stdout) != 1)
            break;
    }
    return 0;
}
