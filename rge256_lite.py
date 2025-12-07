# rge256_lite.py
import numpy as np
from .utils import rotl32, add32

class RGE256Lite:
    """
    RGE256-Lite: Minimal 256-bit ARX mixer.
    Vulnerable to bad seeds. Included for research and reproducibility.
    """

    def __init__(self, seed):
        np.random.seed(seed)
        self.s = np.random.randint(0, 2**32, size=8, dtype=np.uint32)
        self.s[7] = np.uint32(0x243F6A88)   # Avoid absorbing states
        self.rounds = 3

        # Warm up
        for _ in range(10):
            self.next32()

    def next32(self):
        s = self.s

        for _ in range(self.rounds):
            s[0] = add32(s[0], s[1]);  s[1] = rotl32(s[1] ^ s[0], 7)
            s[2] = add32(s[2], s[3]);  s[3] = rotl32(s[3] ^ s[2], 9)
            s[4] = add32(s[4], s[5]);  s[5] = rotl32(s[5] ^ s[4], 13)
            s[6] = add32(s[6], s[7]);  s[7] = rotl32(s[7] ^ s[6], 18)

            # Cross-lane diffusion
            s[0] ^= s[4];  s[1] ^= s[5]
            s[2] ^= s[6];  s[3] ^= s[7]

        return np.uint32(s[0] ^ s[4])
