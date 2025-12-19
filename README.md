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
- **RGE256ctr**   
- **C-accelerated RGE256ctr** 

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
Includes 64-bit counter for guaranteed minimum period of 2⁶⁴.  
**Algorithm design by Alexey L. Voskov** based on the original RGE256 concept.

### **RGE512ex**
A 512-bit state (eight 64-bit words) with counter injection and cross-lane mixing.  
Strongest avalanche behavior and most uniform statistical profile.  
Intended for long-running simulations that benefit from a larger internal state.  
**Algorithm design by Alexey L. Voskov** based on the original RGE256 concept.

### **RGE256ctr**
Counter-mode generator inspired by reduced-round ChaCha quarter-round flow.  
Uses six ARX rounds, a 64-bit counter, and feedforward output.  
Easy to parallelize and suitable for tensor and array generation.  
This is the **recommended default generator**.  
**Counter-mode architecture by Alexey L. Voskov**.

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

## 3. Streaming Validation Results

Comprehensive validation was performed using streaming input mode with Dieharder (114 tests) and SmokeRand (42 tests). Streaming mode eliminates file-rewind artifacts that can cause false failures in corpus-based testing.

**All variants achieved zero Dieharder failures and Quality 4.0 ratings on SmokeRand.**

### Dieharder Results (Streaming Input)

Results shown as Passed/Weak/Failed:

| Variant | Diehard Core (17) | STS Serial (32) | RGB Bitdist (12) | RGB Lagged Sum (33) | DAB Tests (6) | Total (114) |
|---------|-------------------|-----------------|------------------|---------------------|---------------|-------------|
| RGE256Lite | 17/0/0 | 32/0/0 | 12/0/0 | 33/0/0 | 5/1/0 | 113/1/0 |
| RGE256LiteSafe | 17/0/0 | 31/1/0 | 11/1/0 | 33/0/0 | 6/0/0 | 112/2/0 |
| RGE256ctr | 17/0/0 | 32/0/0 | 12/0/0 | 32/1/0 | 5/1/0 | 112/2/0 |
| RGE256ex | 17/0/0 | 32/0/0 | 11/1/0 | 31/2/0 | 6/0/0 | 111/3/0 |
| RGE512ex | 16/1/0 | 31/1/0 | 12/0/0 | 32/1/0 | 6/0/0 | 110/4/0 |

### SmokeRand Results

| Variant | Express (7) | Default (42) | Quality | Data Volume |
|---------|-------------|--------------|---------|-------------|
| RGE256Lite | 7/7 pass | 42/42 pass | 4.00 | 145 GB |
| RGE256LiteSafe | 7/7 pass | 42/42 pass | 4.00 | 145 GB |
| RGE256ctr | 7/7 pass | 42/42 pass | 4.00 | 145 GB |
| RGE256ex | 7/7 pass | 42/42 pass | 4.00 | 145 GB |
| RGE512ex | 7/7 pass | 42/42 pass | 4.00 | 145 GB |

All variants achieve the maximum Quality 4.0 rating, processing approximately 145 GB (2³⁷ bytes) per variant.

---

## 4. External Review and Improvements

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
  structure, constants, or round functions from Voskov's experimental EX variants.
  These variants show strong statistical properties and improved diffusion.

### Voskov's Contributions

Alexey L. Voskov provided critical contributions to this project:

- **Algorithm Design**: RGE512ex and RGE256ctr counter-mode constructions are his designs, included here with attribution
- **Test-Safe CTR Architecture**: The counter-mode formulation for SmokeRand/TestU01/PractRand compatibility
- **Independent Validation**: Confirmed RGE256Lite passes TestU01 BigCrush, PractRand (≥1 TiB), and all SmokeRand batteries
- **Streaming Validation**: All variants tested via streaming input achieving zero Dieharder failures

His feedback directly influenced the improved counter-based and extended-state variants now included in this repository.

Full credit is provided in the [Credits](#12-credits) section.

All generators in this repository are original implementations by Steven Reid,
except where explicitly attributed to Voskov (RGE512ex, RGE256ctr C reference).

---

## 5. Benchmark Results

The following benchmarks were run on Google Colab hardware using 5,000,000 generated 32-bit outputs for each generator (except where noted). These values compare the C implementation of RGE256 against widely used noncryptographic PRNGs.

### Stream Throughput (5M iterations)

| Generator        | MB/s | Notes |
|------------------|------|-------|
| **RGE256ctr_C**  | **1.59 MB/s** | C implementation (recommended) |
| Xoshiro256++     | 0.64 MB/s | Fast reference ARX generator |
| PCG64           | 0.99 MB/s | NumPy default high-quality PRNG |
| Philox          | 1.06 MB/s | PyTorch default counter-based PRNG |

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

---

## 6. Recommended Variant

For practical use and potential integration into PyTorch, the recommended
generator is **RGE256ctr**.

RGE256ctr is a counter-based ARX generator inspired by reduced-round ChaCha.
It is deterministic, parallelizable, has no bad seeds, guarantees a minimum
period of 2⁶⁴, and avoids the structural limitations of the original Lite
variant. It also provides the lowest serial correlation and the strongest
practical diffusion among the 32-bit variants in the suite.

A portable C99 implementation (`librge256ctr.so`) and Python wrapper are
included for use in high-performance or tensor-based applications.
RGE256ctr also aligns with PyTorch's preferred architecture for RNGs—counter-
based, stateless, parallelizable, and suitable for integration into a 
GeneratorImpl backend.

---

## 7. Python Usage

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

## 8. Project Structure

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

## 9. License

This project is released under the **MIT License**.  
All code is available for academic and industrial use.

---

## 10. Citation

If you use this work, please cite:

**Reid, S. (2025). RGE-256: ARX-Based Pseudorandom Number Generator With Structured Entropy and Empirical Validation. Zenodo.**  
DOI: [https://doi.org/10.5281/zenodo.17713219](https://zenodo.org/records/17861488) 


---

## 11. Credits

**Primary author:** Steven Reid  
Independent Researcher  
ORCID: 0009-0003-9132-3410

---

### Algorithm Attribution

#### RGE256ex, RGE512ex, RGE256ctr

The following algorithms are modifications/extensions of the original RGE256 design:

**RGE256ex** and **RGE512ex** are improved modifications of the RGE256 nonlinear generator. The author of these modifications is **Alexey L. Voskov**:

- A linear part with 64-bit counter was added (so the minimal period is at least 2⁶⁴)
- Extra rotations were added to the ARX nonlinear transformation that allowed reducing the number of rounds and eliminating the need for an output function

**RGE256ctr** counter-mode architecture and its C reference implementation were developed by **Alexey L. Voskov** for test-safe PRNG construction compatible with SmokeRand, TestU01, and PractRand streaming harnesses.

These contributions addressed key limitations in the original RGE256Lite design (output efficiency, period guarantees) and enabled compatibility with streaming test harnesses.

**References:**

1. Reid, S. (2025). RGE-256: A New ARX-Based Pseudorandom Number Generator With Structured Entropy and Empirical Validation. Zenodo. https://doi.org/10.5281/zenodo.17713219

2. Voskov, A. L. (2025). SmokeRand: A Set of Tests for Pseudorandom Number Generators. GitHub. https://github.com/alvoskov/SmokeRand

**Copyright:**

The original RGE256 algorithm was created by Steven Reid.

Reengineering to RGE256ex, RGE512ex, RGE256ctr and reentrant C versions for SmokeRand:

(c) 2025 Alexey L. Voskov, Lomonosov Moscow State University.  

---

### Acknowledgement (SmokeRand)

Statistical testing was performed using the [SmokeRand](https://github.com/alvoskov/SmokeRand) test suite by GitHub user **alvoskov** (Alexey L. Voskov). The RGE256 author is solely responsible for interpreting these results; this use and mention do not imply endorsement or validation of RGE256 by the SmokeRand author.

---

### Independent Validation

Alexey L. Voskov independently validated the RGE256lite using:

- **TestU01**: SmallCrush, Crush, and BigCrush (full pass)
- **PractRand**: ≥1 TiB (no failures)
- **SmokeRand**: Express, brief, default, and full batteries (Quality 4.0)
- **Dieharder**: Streaming validation (zero failures across all variants)

His analysis of rotation constants and seed weaknesses directly informed the development of RGE256LiteSafe and the counter-mode variants.

---

### Summary

All generators in this repository are original implementations by Steven Reid, except where explicitly attributed to Voskov above. The RGE256ex, RGE512ex, and RGE256ctr algorithm designs are due to Alexey L. Voskov based on the original RGE256 concept. No code from SmokeRand is included in this repository.

---

### Attribution Notice

This project is maintained in good faith with the intent to properly credit all contributors. If any reference, acknowledgement, or attribution is missing or incomplete, this is an unintentional oversight on my part, not an attempt to claim credit for others' work. If you believe any contribution has not been properly acknowledged, please open an issue or contact me directly, and I will promptly update this section.

---

## AI Assistance Acknowledgment

The implementations in this repository were developed with the assistance of 
AI tools, which were used to translate algorithmic designs, specifications, and 
variant structures into working source code. All generator architectures, 
variant definitions, mixing strategies, design choices, and testing methodology 
were created, directed, and validated by Steven Reid. AI assistance was used in 
a supportive role for code generation and documentation, not for conceptual or 
algorithmic creation.
