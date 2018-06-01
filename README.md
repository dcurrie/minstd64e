# minstd64e
Minimal Standard 64-bit pseudo random number generator

This code is inspired by M.E. O'Neill's blog post
http://www.pcg-random.org/posts/does-it-beat-the-minimal-standard.html

This blog post describes a minimal-standard LCG with 128-bit state, 128-bit multiplier, 
and 64-bit output (the high 64 bits of the state). Some of the features of this type of
generator are
* simple code
* small state
* full 2^128 period (as long as multiplier and addend are chosen carefully)

Based on that work, I searched for a new multiplier that passes the LLL spectral test, and 
also passes PractRand and BigCrush (TestU01). I decided to use addend of 1. This constrains 
the multiplier to be 1 modulo 4 (Hull-Dobell Theorem). This search resulted in the multiplier

    44629693204054986313095191502463994785 == #x21935dc2e417e6615a3a5a07fd848ba1

All tests pass!
