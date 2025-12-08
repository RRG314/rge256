# tests/test_cycle.py

import numpy as np
from rge256 import (
    RGE256Lite,
    RGE256Safe,
    RGE256ex,
    RGE512ex,
    RGE256ctr,
)

def detect_cycle(gen, n=500_000):
    """
    Proper cycle detection:
    A generator is only a TRUE cycle if it returns to the *entire state*,
    not if the output repeats.
    """
    seen_states = set()

    for _ in range(n):
        state = tuple(gen.state) if hasattr(gen, "state") else None
        if state in seen_states:
            return True
        seen_states.add(state)
        gen.next32()

    return False


def test_cycles():
    gens = [
        ("RGE256Lite",  RGE256Lite(seed=1)),
        ("RGE256Safe",  RGE256Safe(seed=1)),   # should be NO cycle
        ("RGE256ex",    RGE256ex(seed=1)),
        ("RGE512ex",    RGE512ex(seed=1)),
        ("RGE256ctr",   RGE256ctr(seed=1)),
    ]

    for name, gen in gens:
        c = detect_cycle(gen)
        print(name, "→", c)
        assert c is False, f"{name} incorrectly reports a cycle"
