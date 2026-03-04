import ctypes
import os

# Path to shared library
_lib_path = os.path.join(os.path.dirname(__file__), "librge256ctr.so")
try:
    _lib = ctypes.CDLL(_lib_path)
except OSError as exc:  # pragma: no cover - platform-dependent native loading
    _lib = None
    _load_error = exc
else:
    _load_error = None

# C struct mapped to Python
class RGE256CTRState(ctypes.Structure):
    _fields_ = [
        ("c", ctypes.c_uint32 * 4),
        ("key", ctypes.c_uint32 * 8),
        ("counter", ctypes.c_uint64),
    ]

if _lib is not None:
    _lib.rge256ctr_init.argtypes = [ctypes.POINTER(RGE256CTRState), ctypes.c_uint32]
    _lib.rge256ctr_init.restype = None

    _lib.rge256ctr_next32.argtypes = [ctypes.POINTER(RGE256CTRState)]
    _lib.rge256ctr_next32.restype = ctypes.c_uint32

class RGE256ctr_C:
    def __init__(self, seed=1):
        if _lib is None:
            raise RuntimeError(
                f"Failed to load native backend at {_lib_path}: {_load_error}. "
                "Use rge256.RGE256ctr (pure Python) on this platform."
            )
        self.state = RGE256CTRState()
        _lib.rge256ctr_init(ctypes.byref(self.state), ctypes.c_uint32(seed))

    def next32(self):
        return int(_lib.rge256ctr_next32(ctypes.byref(self.state)))
