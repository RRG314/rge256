# Credits

## Primary Author
Steven Reid  
Creator of the original RGE256 concept, the full Python implementations, the RGE256Suite structure, and all NumPy based variants (Lite, LiteSafe, ex, 512ex, ctr).

## External Testing and Analysis
Alexey L. Voskov 
GitHub: https://github.com/alvoskov  
SmokeRand: https://github.com/alvoskov/SmokeRand

# Credits

## Primary author

Steven Reid  
Designer of the original RGE256 algorithm and author of the Python implementations in this repository, including the Lite, LiteSafe, ex, 512ex, and ctr variants, along with the internal statistical test scripts.

## External feedback and testing

An independent reviewer and author of the SmokeRand framework (https://github.com/alvoskov/SmokeRand) implemented the RGE256-Lite variant in C as `rge256lite.c` and integrated it into SmokeRand.

Based on the comment header and discussion, this C implementation of RGE256-Lite was reported to have passed:

- SmokeRand express, brief, default, and full batteries  
- TestU01 SmallCrush, Crush, and BigCrush  
- PractRand testing to at least 1 TiB of output  

The reviewer also identified two important drawbacks of the RGE256-Lite design:

- It is relatively slow compared to other non-cryptographic generators  
- It has no guaranteed minimal period and can have bad seeds

In addition, the reviewer experimented with several related designs, including RGE256ex, RGE512ex, RGE256ex-ctr, and RGE512ex-ctr, and reported that early statistical tests for these were promising, with full testing still in progress at the time of the comments.

This project acknowledges that feedback, incorporates the bad-seed warning into the documentation for the Lite variant, and introduces new variants (such as RGE256-Lite-Safe and RGE256ctr) specifically to address the period and seed-safety concerns.

## AI Assistance Acknowledgment

The implementations in this repository were developed with the assistance of 
AI tools, which were used to translate algorithmic designs, specifications, and 
variant structures into working source code. All generator architectures, 
variant definitions, mixing strategies, design choices, and testing methodology 
were created, directed, and validated by Steven Reid. AI assistance was used in 
a supportive role for code generation and documentation, not for conceptual or 
algorithmic creation.


## Additional Acknowledgments
Thanks to the open source PRNG research community for providing tools including TestU01, PractRand, Dieharder, and SmokeRand. These tools made independent validation possible.
