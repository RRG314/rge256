// rge256_drbg.c
// RGE256-DRBG (C99) — ChaCha20-based DRBG wrapper

#include "rge256_drbg.h"
#include <string.h> // memcpy, memset

static inline uint32_t rotl32(uint32_t x, uint32_t r) {
    return (x << r) | (x >> (32u - r));
}

static inline uint32_t load32_le(const uint8_t b[4]) {
    return (uint32_t)b[0]
        | ((uint32_t)b[1] << 8)
        | ((uint32_t)b[2] << 16)
        | ((uint32_t)b[3] << 24);
}

static inline void store32_le(uint8_t out[4], uint32_t x) {
    out[0] = (uint8_t)(x);
    out[1] = (uint8_t)(x >> 8);
    out[2] = (uint8_t)(x >> 16);
    out[3] = (uint8_t)(x >> 24);
}

static inline void qr(uint32_t x[16], int a, int b, int c, int d) {
    x[a] += x[b];  x[d] ^= x[a];  x[d] = rotl32(x[d], 16);
    x[c] += x[d];  x[b] ^= x[c];  x[b] = rotl32(x[b], 12);
    x[a] += x[b];  x[d] ^= x[a];  x[d] = rotl32(x[d], 8);
    x[c] += x[d];  x[b] ^= x[c];  x[b] = rotl32(x[b], 7);
}

// ChaCha20 block (IETF): constants, key[8], counter, nonce[3]
static void chacha20_block(
    uint8_t out[64],
    const uint32_t key[8],
    uint32_t counter,
    const uint32_t nonce[3]
) {
    uint32_t x[16];
    uint32_t w[16];

    x[0] = 0x61707865u; // "expa"
    x[1] = 0x3320646eu; // "nd 3"
    x[2] = 0x79622d32u; // "2-by"
    x[3] = 0x6b206574u; // "te k"

    x[4]  = key[0];
    x[5]  = key[1];
    x[6]  = key[2];
    x[7]  = key[3];
    x[8]  = key[4];
    x[9]  = key[5];
    x[10] = key[6];
    x[11] = key[7];

    x[12] = counter;
    x[13] = nonce[0];
    x[14] = nonce[1];
    x[15] = nonce[2];

    for (int i = 0; i < 16; i++) w[i] = x[i];

    // 20 rounds = 10 double-rounds
    for (int i = 0; i < 10; i++) {
        // column rounds
        qr(w, 0, 4,  8, 12);
        qr(w, 1, 5,  9, 13);
        qr(w, 2, 6, 10, 14);
        qr(w, 3, 7, 11, 15);
        // diagonal rounds
        qr(w, 0, 5, 10, 15);
        qr(w, 1, 6, 11, 12);
        qr(w, 2, 7,  8, 13);
        qr(w, 3, 4,  9, 14);
    }

    for (int i = 0; i < 16; i++) {
        uint32_t y = w[i] + x[i];
        store32_le(&out[4u * (uint32_t)i], y);
    }
}

static void refill(rge256_drbg_state *st) {
    chacha20_block(st->buf, st->key, st->counter, st->nonce);
    st->counter += 1u;
    st->idx = 0u;
}

static void stream_bytes(rge256_drbg_state *st, uint8_t *out, size_t n) {
    while (n > 0) {
        if (st->idx >= 64u) {
            refill(st);
        }
        size_t avail = 64u - (size_t)st->idx;
        size_t take = (n < avail) ? n : avail;
        memcpy(out, st->buf + st->idx, take);
        st->idx += (uint32_t)take;
        out += take;
        n -= take;
    }
}

static void mix_additional(rge256_drbg_state *st, const uint8_t *add, size_t add_len) {
    if (!add || add_len == 0) return;

    // XOR-mix into key||nonce (44 bytes) cyclically
    uint8_t tmp[RGE256_DRBG_SEED_BYTES];

    for (int i = 0; i < 8; i++) store32_le(&tmp[4u * (uint32_t)i], st->key[i]);
    store32_le(&tmp[32], st->nonce[0]);
    store32_le(&tmp[36], st->nonce[1]);
    store32_le(&tmp[40], st->nonce[2]);

    for (size_t i = 0; i < (size_t)RGE256_DRBG_SEED_BYTES; i++) {
        tmp[i] ^= add[i % add_len];
    }

    for (int i = 0; i < 8; i++) st->key[i] = load32_le(&tmp[4u * (uint32_t)i]);
    st->nonce[0] = load32_le(&tmp[32]);
    st->nonce[1] = load32_le(&tmp[36]);
    st->nonce[2] = load32_le(&tmp[40]);

    memset(tmp, 0, sizeof(tmp));
    st->idx = 64u; // force new block next time
}

// Rekey: derive new key+nonce from stream, then reset counter/buffer.
static void drbg_rekey(rge256_drbg_state *st) {
    uint8_t seed[RGE256_DRBG_SEED_BYTES];
    stream_bytes(st, seed, sizeof(seed));

    for (int i = 0; i < 8; i++) st->key[i] = load32_le(&seed[4u * (uint32_t)i]);
    st->nonce[0] = load32_le(&seed[32]);
    st->nonce[1] = load32_le(&seed[36]);
    st->nonce[2] = load32_le(&seed[40]);

    st->counter = 0u;
    st->idx = 64u; // discard buffered bytes

    memset(seed, 0, sizeof(seed));
}

void rge256_drbg_init(rge256_drbg_state *st, const uint8_t key[32], const uint8_t nonce[12]) {
    // load key/nonce
    for (int i = 0; i < 8; i++) st->key[i] = load32_le(&key[4u * (uint32_t)i]);
    st->nonce[0] = load32_le(&nonce[0]);
    st->nonce[1] = load32_le(&nonce[4]);
    st->nonce[2] = load32_le(&nonce[8]);

    st->counter = 0u;
    st->idx = 64u;

    memset(st->buf, 0, sizeof(st->buf));

    // Internal rekey so raw seed isn't directly output
    drbg_rekey(st);
}

void rge256_drbg_reseed(rge256_drbg_state *st, const uint8_t seed44[RGE256_DRBG_SEED_BYTES]) {
    // XOR mix seed into key||nonce
    for (int i = 0; i < 8; i++) {
        uint32_t s = load32_le(&seed44[4u * (uint32_t)i]);
        st->key[i] ^= s;
    }
    st->nonce[0] ^= load32_le(&seed44[32]);
    st->nonce[1] ^= load32_le(&seed44[36]);
    st->nonce[2] ^= load32_le(&seed44[40]);

    st->idx = 64u;
    st->counter = 0u;

    drbg_rekey(st);
}

void rge256_drbg_generate(
    rge256_drbg_state *st,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional,
    size_t additional_len
) {
    mix_additional(st, additional, additional_len);

    // produce output
    stream_bytes(st, out, out_len);

    // rekey after generate for backtracking-resistance shape
    drbg_rekey(st);
}

uint32_t rge256_drbg_next32(rge256_drbg_state *st) {
    uint8_t b[4];
    stream_bytes(st, b, 4);
    return load32_le(b);
}
