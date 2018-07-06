
#ifdef USE_MINSTD64e

#include "../minstd_64.h"

#if HAVE___UINT128_T

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

#define UINT128_C(h,l) (uint128_v ){ .hi = h, .lo = l }

static inline uint64_t lo32(const uint64_t x)
{
    return (x & 0xffffffffu);
}

static inline uint64_t hi32(const uint64_t x)
{
    return (x >> 32);
}

// https://stackoverflow.com/questions/25095741/how-can-i-multiply-64-bit-operands-and-get-128-bit-result-portably
// http://www.hackersdelight.org/hdcodetxt/muldwu.c.txt

/* 64 bit unsigned multiply with 128 bit result
*/
static inline uint128_v longmult64(const uint64_t u, const uint64_t v)
{
    uint128_v result;

#if defined(_MSC_VER) && defined(_M_AMD64)
    result.lo = _umul128(u, v, &result.hi);
#elif !defined(HAVE___UINT128_T) && ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __clang_major__ >= 9) && defined(__x86_64__))
    unsigned __int128 p = (unsigned __int128 )u * (unsigned __int128 )v;
    result.hi = (uint64_t )(p >> 64);
    result.lo = (uint64_t )p;
#else
    const uint64_t ul = lo32(u);
    const uint64_t uh = hi32(u);
    const uint64_t vl = lo32(v);
    const uint64_t vh = hi32(v);

    uint64_t t  = (ul * vl);

    const uint64_t ll = lo32(t);

    t = (uh * vl) + hi32(t);

    const uint64_t lh = lo32(t) + (ul * vh);
    const uint64_t hl = hi32(t);

    result.hi = ((uh * vh) + hl) + hi32(lh);
    result.lo = (lh << 32) + ll; // or v * v
#endif
    return result;
}

/* 128 bit unsigned multiply with 128 bit result
*/
static inline uint128_v mult128(const uint128_v x, const uint128_v y)
{
    uint128_v p = longmult64(x.lo, y.lo);
    p.hi += (x.hi * y.lo);
    p.hi += (y.hi * x.lo);
    return p;
}

static const uint128_v A = { .hi = 0x21935dc2e417e661ULL, .lo = 0x5a3a5a07fd848ba1ULL };

static uint128_v rng_state = { .hi = 0u, .lo = 1u };

uint64_t next (void)
{
    uint128_v nxt = mult128(rng_state, A);
    nxt.lo += 1u;
    nxt.hi += (nxt.lo == 0u); // carry
    rng_state = nxt;
    return nxt.hi;
}

#endif

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
