# tests/test_imports.py

def test_imports():
    import rge256
    from rge256 import (
        RGE256Lite,
        RGE256Safe,
        RGE256ex,
        RGE512ex,
        RGE256ctr
    )

    RGE256Lite(seed=1)
    RGE256Safe(seed=1)
    RGE256ex(seed=1)
    RGE512ex(seed=1)
    RGE256ctr(seed=1)
