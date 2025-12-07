# utils.py
import numpy as np

def rotl32(x, r):
    """32-bit rotate left"""
    return np.uint32(((x << r) & 0xFFFFFFFF) | (x >> (32 - r)))

def add32(a, b):
    """32-bit modular addition without overflow warnings"""
    return np.uint32((np.uint64(a) + np.uint64(b)) & 0xFFFFFFFF)
