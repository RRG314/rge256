# tests/test_cycle.py

import os
import numpy as np
from rge256 import (
    RGE256Lite,
    RGE256Safe,
    RGE256ex,
    RGE512ex,
    RGE256ctr,
)

def detect_cycle(gen, n=None):
    """
    Proper cycle detection:
    A generator is only a TRUE cycle if it returns to the *entire state*,
    not if the output repeats.
    """
    if n is None:
        # Keep default runtime practical for CI and local checks.
        n = int(os.environ.get("RGE_CYCLE_ITERS", "50000"))

    seen_states = set()

    for _ in range(n):
        state_raw = getattr(gen, "state", None)
        if state_raw is None:
            raise AssertionError("Generator must expose a `state` attribute for cycle testing")
        state = tuple(np.asarray(state_raw, dtype=np.uint32).tolist())
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
