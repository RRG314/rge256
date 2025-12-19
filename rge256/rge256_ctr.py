/**
 * @file rge256_ctr.py
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

# rge256_ctr.py
import numpy as np
from .utils import rotl32, add32

class RGE256ctr:
    """
    RGE256ctr: 6-round ChaCha-like counter-mode generator.
    Fast, parallelizable, stable, and no bad seeds.
    """

    def __init__(self, seed):
        # ChaCha constants
        self.const = np.array([
            0x61707865, 0x3320646E,
            0x79622D32, 0x6B206574
        ], dtype=np.uint32)

        # Key derived from seed
        np.random.seed(seed)
        self.key = np.random.randint(0, 2**32, size=8, dtype=np.uint32)

        # 64-bit counter
        self.counter = np.uint64(0)

    def quarter_round(self, x, a, b, c, d):
        x[a] = add32(x[a], x[b]);  x[d] ^= x[a];  x[d] = rotl32(x[d], 16)
        x[c] = add32(x[c], x[d]);  x[b] ^= x[c];  x[b] = rotl32(x[b], 12)
        x[a] = add32(x[a], x[b]);  x[d] ^= x[a];  x[d] = rotl32(x[d], 8)
        x[c] = add32(x[c], x[d]);  x[b] ^= x[c];  x[b] = rotl32(x[b], 7)

    def next32(self):
        x = np.zeros(16, dtype=np.uint32)
        x[0:4] = self.const
        x[4:12] = self.key

        ctr = self.counter
        x[12] = np.uint32(ctr & 0xFFFFFFFF)
        x[13] = np.uint32((ctr >> 32) & 0xFFFFFFFF)
        x[14] = self.key[0] ^ x[12]
        x[15] = self.key[1] ^ x[13]

        self.counter += 1

        w = x.copy()

        # 6 ARX rounds (reduced ChaCha)
        for _ in range(6):
            self.quarter_round(w, 0,4,8,12)
            self.quarter_round(w, 1,5,9,13)
            self.quarter_round(w, 2,6,10,14)
            self.quarter_round(w, 3,7,11,15)

            self.quarter_round(w, 0,5,10,15)
            self.quarter_round(w, 1,6,11,12)
            self.quarter_round(w, 2,7,8,13)
            self.quarter_round(w, 3,4,9,14)

        # Feedforward
        out = (w[0] + x[0]) & 0xFFFFFFFF
        return np.uint32(out)
