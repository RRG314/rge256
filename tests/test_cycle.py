# tests/test_cycle.py

import pytest

from rge256 import (
    RGE256Lite,
    RGE256Safe,
    RGE256ex,
    RGE512ex,
    RGE256ctr
)

def floyd(gen, limit=200000):
    tortoise = gen.next32()
    hare = gen.next32()
    hare = gen.next32()

    steps = 0
    while steps < limit:
        tortoise = gen.next32()
        hare = gen.next32()
        hare = gen.next32()
        if tortoise == hare:
            return True
        steps += 1
    return False

@pytest.mark.parametrize("GenClass", [
    RGE256Lite,
    RGE256Safe,
    RGE256ex,
    RGE512ex,
    RGE256ctr
])
def test_no_short_cycles(GenClass):
    gen = GenClass(seed=9999)
    has_cycle = floyd(gen)
    assert has_cycle is False
