"""
RGE256 — Recursive Geometric Entropy PRNG Family
Provides multiple ARX-based pseudorandom number generators,
including both pure Python and C-accelerated implementations.
"""

# Expose Python implementations
from .rge256_ctr import RGE256ctr
from .rge256_lite import RGE256Lite
from .rge256_safe import RGE256Safe
from .rge256_ex import RGE256ex
from .rge512_ex import RGE512ex

# Expose optional C backend (fallback to Python if native module fails to load)
try:
    from .rge256ctr_wrapper import RGE256ctr_C
    HAS_NATIVE_CTR = True
except Exception:  # pragma: no cover - platform-dependent native loading
    class RGE256ctr_C(RGE256ctr):  # type: ignore[misc]
        """Compatibility fallback when native C backend is unavailable."""

    HAS_NATIVE_CTR = False

__all__ = [
    "RGE256ctr_C",
    "RGE256ctr",
    "RGE256Lite",
    "RGE256Safe",
    "RGE256ex",
    "RGE512ex",
    "HAS_NATIVE_CTR",
]
