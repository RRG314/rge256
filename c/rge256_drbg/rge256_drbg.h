// rge256_drbg.h
// RGE256-DRBG (C99) — ChaCha20-based DRBG wrapper
//
// - ChaCha20 core (20 rounds), IETF layout
// - Buffered output (64-byte blocks)
// - Rekey after generate() (backtracking-resistance shape)
// - Reseed supported (44 bytes: 32 key + 12 nonce)
// - next32() convenience API

#ifndef RGE256_DRBG_H
#define RGE256_DRBG_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RGE256_DRBG_KEY_BYTES    32u
#define RGE256_DRBG_NONCE_BYTES  12u
#define RGE256_DRBG_SEED_BYTES   (RGE256_DRBG_KEY_BYTES + RGE256_DRBG_NONCE_BYTES) // 44

typedef struct {
    uint32_t key[8];     // 256-bit key
    uint32_t nonce[3];   // 96-bit nonce (IETF)
    uint32_t counter;    // 32-bit block counter

    uint8_t  buf[64];    // buffered keystream block
    uint32_t idx;        // next unread byte in buf (0..64), 64 = empty
} rge256_drbg_state;

// Initialize with 32-byte key and 12-byte nonce.
// Note: init performs an internal rekey so raw seed bytes are not directly output.
void rge256_drbg_init(rge256_drbg_state *st, const uint8_t key[32], const uint8_t nonce[12]);

// Reseed with 44 bytes: 32 key bytes + 12 nonce bytes.
// This XOR-mixes into state and rekeys.
void rge256_drbg_reseed(rge256_drbg_state *st, const uint8_t seed44[RGE256_DRBG_SEED_BYTES]);

// Generate out_len bytes. If additional != NULL and additional_len > 0,
// additional input is XOR-mixed into (key||nonce) before generation.
// After generation, the state is rekeyed.
void rge256_drbg_generate(
    rge256_drbg_state *st,
    uint8_t *out,
    size_t out_len,
    const uint8_t *additional,
    size_t additional_len
);

// Convenience: returns one 32-bit word from buffered stream (little-endian).
uint32_t rge256_drbg_next32(rge256_drbg_state *st);

#ifdef __cplusplus
}
#endif

#endif
