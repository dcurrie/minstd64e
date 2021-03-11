# minstd64e
Minimal Standard 64-bit pseudo random number generator

This code is inspired by M.E. O'Neill's blog post
http://www.pcg-random.org/posts/does-it-beat-the-minimal-standard.html

That blog post describes a minimal-standard LCG with 128-bit state, 128-bit multiplier,
and 64-bit output (the high 64 bits of the state). Some of the features of this type of
generator are
* simple code
* small state
* full 2^128 period (as long as multiplier and addend are chosen carefully)

Based on that work, I searched for a new multiplier that passes the LLL spectral test, and
also passes PractRand and BigCrush (TestU01). I decided to use addend of 1. This constrains
the multiplier to be 1 modulo 4 (Hull-Dobell Theorem). This search resulted in the multiplier

    44629693204054986313095191502463994785 == #x21935dc2e417e6615a3a5a07fd848ba1

All tests pass! By this I mean:
* Spectral Test LLL: M8 M16 M24 all greater than 0.62 and M32 and M48 greater than 0.599
* PractRand up to 8 terabytes (2^43 bytes) -- see `spectraltest/output/output-128e61.txt`
* BigCrush "All tests were passed" for 32 LSBs -- see `output/full-crush-minstd_64-e_lolo.txt`
* BigCrush "All tests were passed" for 32 LSBs reversed  -- see `output/full-crush-minstd_64-e_lorev.txt`
* BigCrush "All tests were passed" for 32 MSBs  -- see `output/full-crush-minstd_64-e_hihi.txt`
* BigCrush "All tests were passed" for 32 MSBs reversed  -- see `output/full-crush-minstd_64-e_hirev.txt`
* Hamming-weight dependency test: no failures up to 5 x 10^14 bytes (process stopped) -- see `hwd/hwd_out.txt`

## Improvements to Karl Entacher's spectral test code

With help from Melissa, the spectral test code was extended to 48 dimensions using the L'Ecuyer recommended values based on work by Leech and Rogers. An alternate set of constants with tighter bounds found by Cohn & Elkies was also added. Experimentation with the NTL-based LLL options led to the use of a variant using Block Korkin-Zolotarev reduction. The block size used in that algorithm can be controlled from the command line, along with the selection of constants set. The new command is

`lll_spect [-c|--Cohn_Elkies [0|1]] [-b|--blocksize <2..48>] [-h|--help] <output-file-name> <dim> <mult> <modul>`

The default block size is 10. It can be varied between 2 and the `<dim>` to trade speed and accuracy. Using a value of 30 was sufficient to obtain accuracy equivalent to Karl Entacher's Mathematica implementation in our tests.

The set of constants defaults to the original Entacher constants extended with L'Ecuyer, Leech, Rogers values up to `<dim>` 48. Specifying `-c` switches to the Cohn & Elkies values, which is limited to `<dim>` 36.
