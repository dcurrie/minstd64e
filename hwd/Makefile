
# Configuration

DEFS = -DUSE_MINSTD64e -DHWD_BITS=64 -DHWD_PRNG_BITS=64 -DHWD_DIM=8

hwd: hwd.c prngs_hwd.c
	cc -O3 -std=c99 -Wall $(DEFS) hwd.c -o hwd 
