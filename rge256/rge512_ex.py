/**
 * @file rge512_ex.py
 * 
 *
 * @details
 * This file implements an RGE family generator derived from the original
 * RGE-256 design by Steven Reid. The RGE architecture is based on structured
 * ARX (add-rotate-xor) transformations with recursive cross-lane diffusion
 * intended to maximize entropy propagation and avalanche behavior.
 *
 * This specific variant includes independent reengineering and/or extension
 * work, including counter-mode construction, implementation hardening,
 * and statistical testing.
 *
 * Independent reengineering, C implementation, and statistical testing
 * for SmokeRand, TestU01, and PractRand were performed by Alexey L. Voskov.
 * These efforts include rewriting selected RGE cores into reentrant,
 * streaming-compatible C implementations and validating them using
 * full-scale empirical test batteries.
 *
 *
 * References:
 *
 * 1. Voskov, A. L. (2025). Independent reengineering and validation of
 *    RGE-based generators for SmokeRand and TestU01.
 *    https://github.com/alvoskov/smokerand-generators
 *
 * 2. Reid, S. (2025). RGE-256: A New ARX-Based Pseudorandom Number Generator
 *    with Structured Entropy and Empirical Validation. Zenodo.
 *    https://doi.org/10.5281/zenodo.17713219
 *
 * Original algorithm:
 *   © 2025 Steven Reid
 *
 * Reengineering, counter-mode variants, and C implementations:
 *   © 2025 Alexey L. Voskov
 *   Lomonosov Moscow State University
 *   alvoskov@gmail.com
 *
 * License:
 *   MIT License
 */


# rge512_ex.py
import numpy as np
from .utils import rotl32, add32

class RGE512ex:
    """
    RGE512ex: 512-bit ARX generator with 16-word state.
    Aggressive avalanche behavior; excellent uniformity.
    """

    def __init__(self, seed):
        np.random.seed(seed)
        self.s = np.random.randint(0, 2**32, size=16, dtype=np.uint32)
        self.rounds = 2

    def next32(self):
        s = self.s

        for _ in range(self.rounds):
            # Layer 1
            s[0] = add32(s[0], add32(s[1], 0x9E3779B9));   s[1] = rotl32(s[1] ^ s[0], 11)
            s[2] = add32(s[2], add32(s[3], 0x85EBCA77));   s[3] = rotl32(s[3] ^ s[2], 17)
            s[4] = add32(s[4], add32(s[5], 0xC2B2AE35));   s[5] = rotl32(s[5] ^ s[4], 19)
            s[6] = add32(s[6], add32(s[7], 0x165667B1));   s[7] = rotl32(s[7] ^ s[6], 23)

            # Layer 2
            s[8]  = add32(s[8],  add32(s[9],  0x27D4EB2F)); s[9]  = rotl32(s[9]  ^ s[8], 14)
            s[10] = add32(s[10], add32(s[11], 0xDE5FB9D9)); s[11] = rotl32(s[11] ^ s[10], 21)
            s[12] = add32(s[12], add32(s[13], 0x98EF1E27)); s[13] = rotl32(s[13] ^ s[12], 7)
            s[14] = add32(s[14], add32(s[15], 0xA2659AF1)); s[15] = rotl32(s[15] ^ s[14], 29)

            # Avalanche cross-mix
            for i in range(8):
                s[i] ^= s[i+8]
                s[i+8] = rotl32(s[i+8] ^ s[i], (5 * i + 11) % 32)

        return np.uint32(s[7] ^ s[12])
