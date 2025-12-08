# RGE256: ARX-Based Pseudorandom Number Generator Suite

RGE256 is a family of ARX-based pseudorandom number generators built around 256-bit and 512-bit internal states. Each variant applies 32-bit add–rotate–xor (ARX) transformations and recursive geometric mixing rules to produce deterministic, uniform, and statistically stable output streams.

The RGE256 suite is intended for:

- numerical simulation  
- Monte Carlo workloads  
- machine learning data generation  
- tensor initialization  
- general-purpose high-quality noncryptographic randomness  

These generators are **not** designed for use in adversarial or cryptographic environments.

The repository contains the following implementations:

- **RGE256Lite**  
- **RGE256LiteSafe**  
- **RGE256ex**  
- **RGE512ex**  
- **RGE256ctr** (recommended default)  
- **C-accelerated RGE256ctr** (`librge256ctr.so`)

All Python variants depend only on NumPy. The counter-mode C implementation includes a Python wrapper for high-throughput generation.

---

## 1. Variants Overview

### **RGE256Lite**
Original 256-bit ARX core using three rounds of updates and simple cross-lane diffusion.  
It passes basic statistical tests but can enter weak states under certain initial seeds.  
Retained for historical and reproducibility purposes.

### **RGE256LiteSafe**
RGE256Lite with an added 64-bit counter.  
This eliminates bad seeds and guarantees a minimum period of **2⁶⁴**.  
Simple, stable, and suitable for deterministic workloads.

### **RGE256ex**
Heavier ARX mixing using multiple rotation constants and tuned additive constants.  
Provides stronger diffusion and stable statistical behavior.  
Does not use a counter.

### **RGE512ex**
A 512-bit state (sixteen 32-bit words) with two ARX layers per round.  
Strongest avalanche behavior and most uniform statistical profile.  
Intended for long-running simulations that benefit from a larger internal state.

### **RGE256ctr**
Counter-mode generator inspired by reduced-round ChaCha quarter-round flow.  
Uses six ARX rounds, a 64-bit counter, and feedforward output.  
Easy to parallelize and suitable for tensor and array generation.  
This is the **recommended default generator**.

---

## 2. Statistical Results

All results were produced using the NumPy implementations in this repository.  
Tests include entropy estimation, chi-square uniformity, lag-1 serial correlation, bit balance, and short cycle detection.

### **RGE256Lite**
- Entropy: ~17.60 bits  
- Chi-square: ~231  
- Serial correlation: 0.00236  
- Bit frequency: 0.49950  
- Cycle detection: none observed in short runs  

### **RGE256LiteSafe**
- Entropy: ~17.60 bits  
- Chi-square: ~312  
- Serial correlation: 0.00138  
- Bit frequency: 0.49978  
- Cycle detection: none observed  

### **RGE256ex**
- Entropy: ~17.60 bits  
- Chi-square: ~240  
- Serial correlation: 0.00145  
- Bit frequency: 0.49990  
- Cycle detection: none observed  

### **RGE512ex**
- Entropy: 18.19 bits  
- Chi-square: 269.46  
- Serial correlation: −0.00110  
- Bit frequency: 0.49979  
- Cycle detection: none observed in 2M steps  

### **RGE256ctr**
- Entropy: ~17.60 bits  
- Chi-square: ~218  
- Serial correlation: 0.000099  
- Bit frequency: 0.50007  
- Cycle detection: none observed  

RGE256ctr shows the lowest serial correlation.  
RGE512ex shows the strongest diffusion and most uniform overall statistics.

---

## 3. External Independent Testing

Independent testing of **RGE256Lite** was performed by **Alexey L. Voskov** using the SmokeRand framework. His findings apply specifically to the Lite variant and not the entire suite.

Verified test results include:

- SmokeRand express, brief, default, and full batteries  
- TestU01 SmallCrush, Crush, and BigCrush  
- PractRand testing to at least **1 TiB**

Voskov also constructed experimental ARX variants (RGE256ex and RGE512ex-style designs) with heavier mixing and improved rotation constants. His feedback led directly to the development of safer and more robust counter-mode versions such as **RGE256LiteSafe** and **RGE256ctr**.

Full credit is given in the **Credits** section.

---

## 4. Benchmark Results

Comprehensive benchmarks compare:

- All RGE256 variants  
- C-accelerated RGE256ctr  
- PCG64  
- Philox  
- Xoshiro256++  
- ChaCha8  
- ChaCha12  
- NumPy array fill  
- PyTorch tensor fill  
- Bulk stream throughput  

### **Benchmark tables will be inserted here once generated.**

---

## 5. Python Usage

```python
from rge256.rge256_ctr import RGE256ctr

g = RGE256ctr(seed=12345)
values = [g.next32() for _ in range(16)]
print(values)
```

### C-accelerated usage:
```python
from rge256.rge256ctr_wrapper import RGE256ctr_C

g = RGE256ctr_C(seed=12345)
print(g.next32())
```

---

## 6. Project Structure

```
c/
    Makefile
    rge256ctr.c
    rge256ctr.h
rge256/
    __init__.py
    librge256ctr.so
    rge256_lite.py
    rge256_safe.py
    rge256_ex.py
    rge512_ex.py
    rge256_ctr.py
    rge256ctr_wrapper.py
    utils.py
tests/
    test_cycle.py
    test_imports.py
    test_small_stats.py
README.md
CREDITS.md
LICENSE
```

---

## 7. PyTorch Integration

RGE256ctr is structured similarly to reduced-round ChaCha and is compatible with PyTorch’s counter-mode RNG paradigm.  
A minimal integration would require:

- A `GeneratorImpl` subclass  
- State serialization  
- CPU kernel registration  
- Optional CUDA kernel extension  
- Integration into `torch.Generator` selection mechanisms  

A full proposal can be provided if required.

---

## 8. License

This project is released under the **MIT License**.  
All code is available for academic and industrial use.

---

## 9. Citation

If you use this work, please cite:

**Reid, S. (2025). RGE-256: ARX-Based Pseudorandom Number Generator With Structured Entropy and Empirical Validation. Zenodo.**  
DOI: https://doi.org/10.5281/zenodo.17713219

---

## 10. Credits

**Primary author:** Steven Reid  
Independent Researcher  

**Independent C testing and analysis:**  
**Alexey L. Voskov**  
- SmokeRand testing  
- TestU01 and PractRand validation  
- Analysis of rotation constants  
- Detection of seed weaknesses  
- Recommendations for counter-mode designs  
- Inspiration for RGE256ex and RGE512ex structures  

His contributions improved the safety, structure, and statistical robustness of the current RGE256 suite.

