import ctypes
import os

# Path to shared library
_lib_path = os.path.join(os.path.dirname(__file__), "librge256ctr.so")
_lib = ctypes.CDLL(_lib_path)

# C struct mapped to Python
class RGE256CTRState(ctypes.Structure):
    _fields_ = [
        ("c", ctypes.c_uint32 * 4),
        ("key", ctypes.c_uint32 * 8),
        ("counter", ctypes.c_uint64),
    ]

_lib.rge256ctr_init.argtypes = [ctypes.POINTER(RGE256CTRState), ctypes.c_uint32]
_lib.rge256ctr_init.restype = None

_lib.rge256ctr_next32.argtypes = [ctypes.POINTER(RGE256CTRState)]
_lib.rge256ctr_next32.restype = ctypes.c_uint32

class RGE256ctr_C:
    def __init__(self, seed=1):
        self.state = RGE256CTRState()
        _lib.rge256ctr_init(ctypes.byref(self.state), ctypes.c_uint32(seed))

    def next32(self):
        return int(_lib.rge256ctr_next32(ctypes.byref(self.state)))
