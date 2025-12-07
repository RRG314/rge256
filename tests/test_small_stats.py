# tests/test_small_stats.py

import numpy as np
import pytest

from rge256 import (
    RGE256Lite,
    RGE256Safe,
    RGE256ex,
    RGE512ex,
    RGE256ctr
)

def entropy(values):
    vals, counts = np.unique(values, return_counts=True)
    p = counts / counts.sum()
    return float(-(p * np.log2(p)).sum())

def chi_square(values, bins=256):
    hist, _ = np.histogram(values, bins=bins, range=(0, 2**32))
    expected = len(values) / bins
    return float(np.sum((hist - expected)**2 / expected))

def serial_corr(values):
    avg = values.mean()
    num = float(np.sum((values[:-1] - avg) * (values[1:] - avg)))
    den = float(np.sum((values - avg)**2))
    if den == 0:
        return 0.0
    return num / den

@pytest.mark.parametrize("GenClass", [
    RGE256Lite,
    RGE256Safe,
    RGE256ex,
    RGE512ex,
    RGE256ctr
])
def test_small_statistics(GenClass):
    gen = GenClass(seed=12345)
    data = np.array([gen.next32() for _ in range(200000)], dtype=np.uint32)

    ent = entropy(data)
    chi = chi_square(data)
    corr = serial_corr(data)
    bits = float(np.unpackbits(data.view(np.uint8)).mean())

    # Basic sanity constraints
    assert 16.0 <= ent <= 20.0
    assert 50.0 <= chi <= 500.0
    assert abs(corr) < 0.01
    assert 0.45 <= bits <= 0.55
