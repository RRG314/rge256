# Credits

## Primary Author
Steven Reid  
Creator of the original RGE256 concept, the full Python implementations, the RGE256Suite structure, and all NumPy based variants (Lite, LiteSafe, ex, 512ex, ctr).

## External Testing and Analysis
Alexey L. Voskov 
GitHub: https://github.com/alvoskov  
SmokeRand: https://github.com/alvoskov/SmokeRand

Voskov independently reimplemented the RGE256Lite algorithm in C for integration with the SmokeRand testing framework. His results apply specifically to the RGE256Lite variant and not to the entire suite.

Completed external testing of RGE256Lite included:
- SmokeRand express battery  
- SmokeRand brief battery  
- SmokeRand default battery  
- SmokeRand full battery  
- TestU01 SmallCrush  
- TestU01 Crush  
- TestU01 BigCrush  
- PractRand testing to at least 1 TiB of input  
- Performance measurement of approximately 400 MiB per second in his C implementation

He also generated several experimental variants inspired by RGE256 using modified round counts, rotation constants, cross coupling patterns, and counter based constructions. These included:
- RGE256ex  
- RGE512ex  
- RGE256ex ctr  
- RGE512ex ctr  

Voskov conducted partial statistical exploration of these experimental variants inside SmokeRand and reported that early results were promising. Full TestU01 and PractRand coverage for these variants was still in progress at the time of his review.

Additional contributions from Voskov include:
- rotation constant recommendations  
- cross lane mixing suggestions  
- analysis of possible bad states  
- recommendations for safe seeding  
- advice on counter based structures  
- structural comparisons with ChaCha style ARX mixers  

All C code written by Voskov remains MIT licensed and credited in accordance with the original repositories.

## Additional Acknowledgments
Thanks to the open source PRNG research community for providing tools including TestU01, PractRand, Dieharder, and SmokeRand. These tools made independent validation possible.
