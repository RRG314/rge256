# **README.md (Final Version)**

````
# RGE256: ARX-Based Pseudorandom Number Generator Suite

RGE256 is a family of ARX-based pseudorandom number generators. The suite contains multiple variants built around a 256-bit or 512-bit internal state, each designed with different mixing strengths, safety properties, and performance characteristics. The generators use 32 bit add-rotate-xor operations and recursive geometric mixing rules.

The RGE256 suite is intended for simulation, machine learning pipelines, Monte Carlo workloads, data generation, and general purpose high quality noncryptographic randomness. These generators are not designed for cryptographic security and are not intended for adversarial settings.

This repository contains the following implementations:

- RGE256Lite  
- RGE256LiteSafe  
- RGE256ex  
- RGE512ex  
- RGE256ctr

The RGE256ctr variant is the recommended default generator. It uses a counter based structure similar to reduced round ChaCha and is suitable for parallel use and tensor generation.

All variants are implemented in pure Python with NumPy and share a common utility module.

---

## 1. Variants Overview

### RGE256Lite
This is the original 256 bit ARX core. It uses three update rounds and simple cross lane diffusion. It passes statistical tests but can enter undesirable states under certain seeds. It is provided for reproducibility of the original design.

### RGE256LiteSafe
This variant adds a 64 bit counter to the RGE256Lite core. The counter ensures that no initial seed produces a trapped state and guarantees a minimum period of 2^64. This version is simple, stable, and suitable for most applications requiring predictable behavior.

### RGE256ex
This version uses two rounds of heavy ARX mixing with large rotation constants and avalanche tuned addition constants. It provides stronger diffusion than the Lite variants but does not use a counter. It shows good uniformity and very stable output statistics.

### RGE512ex
This is a 512 bit state version composed of sixteen 32 bit words and two ARX layers per round. It has strong avalanche behavior and very uniform output. It is the strongest diffuser in the suite and is intended for long running simulations that benefit from a larger internal state.

### RGE256ctr
This is a counter based generator inspired by the ChaCha quarter round structure. It uses six ARX rounds, a 64 bit counter, and feedforward. This variant is easy to parallelize and produces output with very low serial correlation. It is the recommended default generator for new applications.

---

## 2. Statistical Results

All results below were produced using the NumPy versions included in this repository. Tests include entropy estimation, chi square uniformity, lag 1 serial correlation, bit frequency analysis, and short cycle detection.

### RGE256Lite
- Entropy: approximately 17.60 bits  
- Chi square: approximately 231  
- Serial correlation: 0.00236  
- Bit frequency: 0.49950  
- Cycle detection: no cycles detected in short runs  

### RGE256LiteSafe
- Entropy: approximately 17.60 bits  
- Chi square: approximately 312  
- Serial correlation: 0.00138  
- Bit frequency: 0.49978  
- Cycle detection: no cycles detected  

### RGE256ex
- Entropy: approximately 17.60 bits  
- Chi square: approximately 240  
- Serial correlation: 0.00145  
- Bit frequency: 0.49990  
- Cycle detection: no cycles detected  

### RGE512ex
- Entropy: 18.19 bits  
- Chi square: 269.46  
- Serial correlation: approximately   0.00110 (negative)  
- Bit frequency: 0.49979  
- Cycle detection: no cycles detected in 2 million steps  

### RGE256ctr
- Entropy: approximately 17.60 bits  
- Chi square: approximately 218  
- Serial correlation: 0.000099  
- Bit frequency: 0.50007  
- Cycle detection: no cycles detected  

These results indicate that all variants produce stable and uniform distributions with no detected structural failures. The RGE256ctr variant shows the lowest serial correlation. RGE512ex shows the strongest avalanche behavior and the most uniform statistical profile.

---

## 3. External Independent Testing

Alexey L. Voskov independently reimplemented the original RGE256Lite variant in C for use within the SmokeRand testing framework. His results apply specifically to the Lite variant and not to the entire suite.

His testing reported the following:

- SmokeRand express, brief, default, and full batteries passed  
- TestU01 SmallCrush, Crush, and BigCrush passed  
- PractRand testing passed to at least 1 TiB of input  

Voskov also created experimental variants based on RGE256 with modified rotation schemes and heavier mixing. Statistical testing of those versions was reported as ongoing at the time of his comments.

---

## 4. Python Usage

```python
from rge256 import RGE256ctr

g = RGE256ctr(seed=12345)
values = [g.next32() for _ in range(16)]
print(values)
````

---

## 5. Project Structure

```
rge256/
    __init__.py
    rge256_lite.py
    rge256_safe.py
    rge256_ex.py
    rge512_ex.py
    rge256_ctr.py
    utils.py
tests/
examples/
README.md
CREDITS.md
LICENSE
setup.py
pyproject.toml
```

---

## 6. License

This project is released under the MIT License.
All code in this repository is available for academic and industrial use under the terms of the license.

---

## 7. Citation

If you use RGE256 in academic or applied work, please cite:

Reid, S. (2025). RGE-256: ARX Based Pseudorandom Number Generator With Structured Entropy and Empirical Validation. Zenodo.
DOI: [https://doi.org/10.5281/zenodo.17713219](https://doi.org/10.5281/zenodo.17713219)

---

## 8. Credits

Primary author: Steven Reid
Independent C testing and analysis of RGE256Lite: Alexey L. Voskov
Rotation constants, mixing suggestions, and state safety insights were contributed through his open source review.

```


