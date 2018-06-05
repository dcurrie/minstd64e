
#ifdef USE_MINSTD64e

#include "../minstd_64.h"

#define UINT128_C(h,l) (((uint128_v )h << 64) + ((uint128_v )l))

/*
** > (number->string 44629693204054986313095191502463994785 16)
** "21935dc2e417e6615a3a5a07fd848ba1"
** > #x21935dc2e417e6615a3a5a07fd848ba1
** 44629693204054986313095191502463994785
*/

static const uint128_v A = UINT128_C(0x21935dc2e417e661ULL, 0x5a3a5a07fd848ba1ULL);
static const uint128_v C = 1;

static uint128_v rng_state = 1u;

// minstd_64_next

static inline uint64_t next (void)
{
    rng_state = (rng_state * A) + C;
    return (uint64_t )(rng_state >> 64);
}

#else

/* Sample file for hwd.c (xoroshiro128+). */

static inline uint64_t rotl(const uint64_t x, int k) {
    return (x << k) | (x >> (64 - k));
}

static uint64_t s[2] = { 1, -1ULL };

static uint64_t inline next() {
    const uint64_t s0 = s[0];
    uint64_t s1 = s[1];

    const uint64_t result_plus = s0 + s1;

    s1 ^= s0;
    s[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16);
    s[1] = rotl(s1, 37);

    return result_plus;
}

#endif
