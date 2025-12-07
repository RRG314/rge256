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
