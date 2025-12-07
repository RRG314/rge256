"""
RGE256 — Recursive Geometric Entropy PRNG Family
Provides multiple ARX-based pseudorandom number generators,
including both pure Python and C-accelerated implementations.
"""

# Expose C backend (RGE256ctr in C)
from .rge256ctr_wrapper import RGE256ctr_C

# Expose Python implementations
from .rge256_ctr import RGE256ctr
from .rge256_lite import RGE256Lite
from .rge256_safe import RGE256LiteSAFE
from .rge256_ex import RGE256ex
from .rge512_ex import RGE512ex

__all__ = [
    "RGE256ctr_C",
    "RGE256ctr",
    "RGE256Lite",
    "RGE256LiteSAFE",
    "RGE256ex",
    "RGE512ex",
]
