#ifndef RGE256CTR_H
#define RGE256CTR_H

#include <stdint.h>
#include <stddef.h>

/* RGE256ctr: 256-bit ARX-based counter PRNG (non-cryptographic). */

typedef struct {
    uint32_t key[8];    /* 256-bit key */
    uint64_t counter;   /* 64-bit block counter */
    uint64_t nonce;     /* 64-bit nonce (stream/thread id) */
    uint32_t buf[16];   /* 16 words = 64 bytes per block */
    int      buf_used;  /* next word index in buf (0..16)  */
} rge256ctr_state;

void     rge256ctr_init(rge256ctr_state *s, uint32_t seed, uint64_t nonce);
uint32_t rge256ctr_next_u32(rge256ctr_state *s);
void     rge256ctr_fill_bytes(rge256ctr_state *s, void *out, size_t nbytes);

#endif /* RGE256CTR_H */
