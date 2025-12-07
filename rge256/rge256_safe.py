# rge256_safe.py
import numpy as np
from .utils import rotl32, add32

class RGE256Safe:
    """
    RGE256-Lite-SAFE:
    Same ARX core as Lite, but includes a 64-bit counter to eliminate cycles
    and guarantee a minimum period of 2^64. No bad seeds.
    """

    def __init__(self, seed):
        np.random.seed(seed)
        self.s = np.random.randint(0, 2**32, size=8, dtype=np.uint32)
        self.rounds = 3
        self.counter = np.uint64(0)

    def next32(self):
        s = self.s

        for _ in range(self.rounds):
            s[0] = add32(s[0], s[1]);  s[1] = rotl32(s[1] ^ s[0], 7)
            s[2] = add32(s[2], s[3]);  s[3] = rotl32(s[3] ^ s[2], 9)
            s[4] = add32(s[4], s[5]);  s[5] = rotl32(s[5] ^ s[4], 13)
            s[6] = add32(s[6], s[7]);  s[7] = rotl32(s[7] ^ s[6], 18)

            s[0] ^= s[4];  s[1] ^= s[5]
            s[2] ^= s[6];  s[3] ^= s[7]

        # Counter ensures period >= 2^64
        self.counter += 1
        c0 = np.uint32(self.counter & 0xFFFFFFFF)
        c1 = np.uint32((self.counter >> 32) & 0xFFFFFFFF)

        return np.uint32((s[0] ^ rotl32(s[4], 13)) ^ c0 ^ rotl32(c1, 7))
