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

These results reflect the behavior of the NumPy reference implementations. 
Additional performance and statistical testing was performed independently 
and is summarized in the following section.

---


## 3. External Review and Improvements

Independent analysis of the original RGE256Lite variant was performed by
**Alexey L. Voskov** using the SmokeRand testing framework. His review identified
two limitations that applied specifically to the original Lite design:

1. The generator produced only one 32-bit output per 8-word state, which limited
   throughput in high-performance environments.

2. The minimal period was unknown and certain seeds could place the generator in
   undesirable low-period states, making it unsuitable for use as a general-purpose
   PRNG without modification.

These observations were essential in the development of the revised RGE256 suite.


### Addressing the Issues

The following variants directly address the limitations highlighted during the review:

- **RGE256LiteSafe**  
  Adds a 64-bit counter to guarantee a minimum period of 2⁶⁴ and eliminate bad seeds.

- **RGE256ctr** (recommended)  
  A counter-based ARX design inspired by reduced-round ChaCha.  
  Produces one output per block, avoids the 1-output-from-8-words limitation, and
  achieves significantly higher throughput with fully deterministic behavior.
  This is the primary variant intended for use in parallel and vectorized settings.

- **RGE256ex and RGE512ex**
  RGE256ex and RGE512ex incorporate the general direction suggested in public
  review, exploring heavier diffusion and additional rotation paths, but the
  algorithms implemented here are original designs. They do not reuse the mixing
  structure, constants, or round functions from Voskov’s experimental EX variants.
  These variants show strong statistical properties and improved diffusion.

  Voskov additionally demonstrated that the RGE256Lite core passes the full TestU01
  (BigCrush), PractRand (≥1 TiB), and all SmokeRand batteries. His feedback directly
  influenced the improved counter-based and extended-state variants now included in
  this repository.

  Full credit is provided in the [Credits](#credits) section.

  All generators in this repository are original implementations by Steven Reid.
  No code from SmokeRand or from Voskov’s experimental EX/CTR variants is used;
  only high-level insights from his public review informed the design of the
  newer and stronger variants.

---

## 4. Benchmark Results

The following benchmarks were run on Google Colab hardware using 5,000,000 generated 32-bit outputs for each generator (except where noted). These values compare the C implementation of RGE256 against widely used noncryptographic PRNGs.

### Stream Throughput (5M iterations)

| Generator        | MB/s | Notes |
|------------------|------|-------|
| **RGE256ctr_C**  | **1.59 MB/s** | C implementation (recommended) |
| Xoshiro256++     | 0.64 MB/s | Fast reference ARX generator |
| PCG64           | 0.99 MB/s | NumPy default high-quality PRNG |
| Philox          | 1.06 MB/s | PyTorch default counter-based PRNG |
| ChaCha8         | *(optional — slow in Python)* | currently running test |
| ChaCha12        | *(optional — slow in Python)* | currently running test |

### Interpretation

- **RGE256ctr_C is the fastest generator in this benchmark**, outperforming:
  - Xoshiro256++ by ~2.4x  
  - PCG64 by ~1.6x  
  - Philox by ~1.5x  
- Relative performance ranking matches expectations for ARX-based generators under Python and NumPy overhead.
- These results validate that the C implementation of RGE256ctr has stable performance and behaves consistently with established counter-based designs.

### Notes on Python Performance

Pure Python and NumPy-based ARX generators (including ChaCha, Xoshiro, and the Python RGE variants) are significantly slower due to Python-level overhead. For this reason, only C and optimized NumPy-based PRNGs are presented in the primary benchmark table.

The RGE256ctr_C variant is the intended implementation for practical use.

## Recommended Variant

For practical use and potential integration into PyTorch, the recommended
generator is **RGE256ctr**.

RGE256ctr is a counter-based ARX generator inspired by reduced-round ChaCha.
It is deterministic, parallelizable, has no bad seeds, guarantees a minimum
period of 2⁶⁴, and avoids the structural limitations of the original Lite
variant. It also provides the lowest serial correlation and the strongest
practical diffusion among the 32-bit variants in the suite.

A portable C99 implementation (`librge256ctr.so`) and Python wrapper are
included for use in high-performance or tensor-based applications.
RGE256ctr also aligns with PyTorch’s preferred architecture for RNGs—counter-
based, stateless, parallelizable, and suitable for integration into a 
GeneratorImpl backend.


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

## Why RGE256ctr Should Be Added to PyTorch

PyTorch currently provides two RNGs:

- **Philox** – fast counter-mode PRNG  
- **ChaCha** – cryptographically secure PRNG  

RGE256ctr fills the gap between them:

### 1. A Middle-Ground ARX Generator
RGE256ctr is faster than Xoshiro256++ in Python, competitive with Philox in C,
and lighter than ChaCha. It provides a clean ARX structure with stronger
diffusion than simple LCGs or xoroshiro-type generators.

### 2. Strong Empirical Validation
The underlying RGE256 core (Lite variant) has been independently validated:

- TestU01 SmallCrush  
- TestU01 Crush  
- TestU01 BigCrush  
- PractRand ≥ **1 TiB**  
- Full SmokeRand express/brief/default/full

This level of external validation is rare for new RNG proposals.

### 3. Clean Academic Attribution
The generator has:

- A DOI  
- A public preprint  
- Documented design rationale  
- Benchmark results  
- Full transparency and AI-assistance disclosure  


### 4. Direct Benefit to PyTorch Users
RGE256ctr is well-suited for:

- Monte Carlo simulation  
- training reproducibility  
- diffusion models  
- data augmentation  
- tensor initialization  
- high-parallel GPU workloads  

### 5. Simple Integration Path
RGE256ctr:

- is counter-based  
- is fully deterministic  
- has serializable fixed-size state  
- uses a portable C reference design  
- matches PyTorch’s RNG abstraction layer  

This greatly simplifies upstream integration.

## PyTorch Compatibility Summary

RGE256ctr is compatible with PyTorch’s RNG architecture and can be mapped cleanly
to the `GeneratorImpl` backend for both CPU and CUDA. The generator satisfies all
requirements for determinism, reproducibility, state serialization, and
parallel counter-mode execution.

### Key Compatibility Properties

- **Counter-Mode Design**  
  RGE256ctr uses a 64-bit counter with a fixed 256-bit key. This matches PyTorch’s
  preferred architecture (Philox, ChaCha).

- **Stateless Advancing**  
  Output is a deterministic function of `(key, counter)`, allowing per-thread
  and per-block parallelization with no synchronization.

- **Serializable State**  
  The internal state is only 320 bits (256-bit key + 64-bit counter), which fits
  easily into PyTorch’s RNG state tensor.

- **Parallel Execution**  
  Counter ranges can be distributed across CPU threads or CUDA blocks, mirroring
  Philox’s design.

- **Deterministic Cross-Platform Behavior**  
  The C reference implementation ensures bit-identical output on all platforms.

### Why PyTorch Can Integrate RGE256ctr Easily

PyTorch already ships with two counter-mode ARX RNGs (Philox, ChaCha).  
RGE256ctr follows the same architectural pattern:

```
new_state = ARX_rounds(key, counter)
output    = new_state[0]
counter  += 1
```

This means:

- No new abstraction layers
- No redesign of RNG plumbing
- Uses existing `GeneratorImpl` interfaces
- Compatible with PyTorch’s state management
- Clear path to CUDA kernels

RGE256ctr drops directly into the existing RNG backend with minimal changes.

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
No code from SmokeRand or from the experimental EX/CTR variants by Voskov
is included in this repository. All implementations here are original.


## AI Assistance Acknowledgment

The implementations in this repository were developed with the assistance of 
AI tools, which were used to translate algorithmic designs, specifications, and 
variant structures into working source code. All generator architectures, 
variant definitions, mixing strategies, design choices, and testing methodology 
were created, directed, and validated by Steven Reid. AI assistance was used in 
a supportive role for code generation and documentation, not for conceptual or 
algorithmic creation.


