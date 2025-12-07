# rge256_ex.py
import numpy as np
from .utils import rotl32, add32

class RGE256ex:
    """
    RGE256ex: 256-bit heavy-mixing ARX generator.
    Uses large rotates and avalanche constants.
    """

    def __init__(self, seed):
        np.random.seed(seed)
        self.s = np.random.randint(0, 2**32, size=8, dtype=np.uint32)
        self.rounds = 2

    def next32(self):
        s = self.s

        for _ in range(self.rounds):
            # First half
            s[0] = add32(s[0], add32(s[1], 0x9E3779B9));  s[1] = rotl32(s[1] ^ s[0], 11)
            s[2] = add32(s[2], add32(s[3], 0x85EBCA77));  s[3] = rotl32(s[3] ^ s[2], 17)

            # Second half
            s[4] = add32(s[4], add32(s[5], 0xC2B2AE35));  s[5] = rotl32(s[5] ^ s[4], 19)
            s[6] = add32(s[6], add32(s[7], 0x165667B1));  s[7] = rotl32(s[7] ^ s[6], 23)

            # Cross-coupling
            s[0] ^= s[4];  s[1] ^= s[5]
            s[2] ^= s[6];  s[3] ^= s[7]

        return np.uint32(s[0] ^ s[4])
