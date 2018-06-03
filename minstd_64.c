/** @file minstd_64.c
 *
 * @brief 64-bit random number generator
 *
 * @par
 * @copyright Copyright © 2018 Doug Currie, Londonderry, NH, USA
 * 
 * @par License
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * @par
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * @par
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

#include "minstd_64.h"

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

uint64_t minstd_64_next (uint128_v *rng_state_p)
{
    *rng_state_p = (*rng_state_p * A) + C;
    return (uint64_t )(*rng_state_p >> 64);
}

/* skip ahead N steps implemented using the algorithm described in
** "Random Number Generation with Arbitrary Strides" 
** by Forrest B. Brown of Argonne National Lab, 1994
*/
uint64_t minstd_64_skip (uint128_v *rng_state_p, const uint128_v skip)
{
    // merged algorithms G and C
    uint128_v g = 1u;
    uint128_v c = 0u;
    uint128_v h = A;
    uint128_v f = C;
    uint128_v i = skip;

    while (0 != i)
    {
        if (i % 2u)
        {
            g = g * h;
            c = c * h + f;
        }
        f *= h + 1u;
        h *= h;
        i /= 2u;
    }
    *rng_state_p = *rng_state_p * g + c;
    return (uint64_t )(*rng_state_p >> 64);
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
#elif !defined(HAVE___UINT128_T) 
    && ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __clang_major__ >= 9) && defined(__x86_64__))
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

uint64_t minstd_64_next (uint128_v *rng_state_p)
{
    uint128_v nxt = mult128(*rng_state_p, A);
    nxt.lo += 1u;
    nxt.hi += (nxt.lo == 0u); // carry
    *rng_state_p = nxt;
    return nxt.hi;
}

static inline uint128_v add128 (uint128_v x, uint128_v y)
{
    uint128_v res = x;
    res.lo += y.lo;
    res.hi += y.hi + (res.lo < y.lo); // carry
    return res;
}

static inline uint128_v inc128 (uint128_v x)
{
    uint128_v res = x;
    res.lo += 1u;
    res.hi += (res.lo == 0u); // carry
    return res;
}

/* skip ahead N steps implemented using the algorithm described in
** "Random Number Generation with Arbitrary Strides" 
** by Forrest B. Brown of Argonne National Lab, 1994
*/
uint64_t minstd_64_skip (uint128_v *rng_state_p, const uint128_v skip)
{
    // merged algorithms G and C
    uint128_v g = { .hi = 0ULL, .lo = 1ULL };
    uint128_v c = { .hi = 0ULL, .lo = 0ULL };
    uint128_v h = A;
    uint128_v f = { .hi = 0ULL, .lo = 1ULL };
    uint128_v i = skip;

    while ((0u != i.lo) || (0u != i.hi))
    {
        if (i.lo % 2u)
        {
            g = mult128(g, h);
            c = mult128(c, h);
            c = add128(c, f);
        }
        f = mult128(f, inc128(h));
        h = mult128(h, h);
        i.lo /= 2u;
        i.lo |= (i.hi & 1u) << 63;
        i.hi /= 2u;
    }
    uint128_v nxt = mult128(*rng_state_p, g);
    nxt = add128(nxt, c);
    *rng_state_p = nxt;
    return nxt.hi;
}

#endif

void minstd_64_seed (uint128_v *rng_state_p, uint128_v seed)
{
    *rng_state_p = seed; // nothing else needed; all states valid
}

/* ************************ testing code below ************************** */

#ifdef TESTU01

#if HAVE___UINT128_T

static uint128_v rng_state = 1;

#else

static uint128_v rng_state = { .hi = 0u, .lo = 1u };

#endif

static uint64_t minstd_64 (void)
{
    return minstd_64_next(&rng_state);
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#include <unif01.h>
#include <bbattery.h>
#include <TestU01.h>

// unif01.h includes gdef.h which defines these, but we need other stuff from inttypes.h
#undef PRIdLEAST64
#undef PRIuLEAST64
#include <inttypes.h>


inline uint32_t rev32(uint32_t v)
{
    // https://graphics.stanford.edu/~seander/bithacks.html
    // swap odd and even bits
    v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
    // swap consecutive pairs
    v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
    // swap nibbles ...
    v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
    // swap bytes
    v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
    // swap 2-byte-long pairs
    v = ( v >> 16             ) | ( v               << 16);
    return v;
}

/* Note that this function returns a 32-bit number. */
unsigned long minstd_64_32lo (void *unused0, void *unused1)
{
    return (uint32_t )minstd_64();
}

/* Note that this function returns a 32-bit number. */
unsigned long minstd_64_32lorev (void *unused0, void *unused1)
{
    return rev32((uint32_t )minstd_64());
}

/* Note that this function returns a 32-bit number. */
unsigned long minstd_64_32hirev (void *unused0, void *unused1)
{
    return rev32((uint32_t )(minstd_64() >> 32));
}

/* Note that this function returns a 32-bit number. */
unsigned long minstd_64_32hi (void *unused0, void *unused1)
{
    return (uint32_t )(minstd_64() >> 32);
} 

#define NORM_64 ( ( 1./4 ) / ( 1LL << 62 ) )

double minstd_64_01 (void *unused0, void *unused1)
{
    return minstd_64() * NORM_64;
}

//void write_state_64 (void *unused) 
//{
//    printf( "%llu\n", (unsigned long long)rng_state);
//}

void write_128 (uint128_v r) 
{
#if HAVE___UINT128_T
    printf("0x%016" PRIx64 "_%016" PRIx64 "", (unsigned long long)(r >> 64), (unsigned long long)(r));
#else
    printf("0x%016" PRIx64 "_%016" PRIx64 "", (unsigned long long)(r.hi), (unsigned long long)(r.lo));
#endif
    printf("\n");
}

void write_state_128 (void *unused) 
{
    write_128(rng_state);
}

int main(int argc, char *argv[] )
{
    unif01_Gen gen;
    gen.GetU01 = minstd_64_01;
    gen.Write = write_state_128;

    if (argc <= 1)
    {
usage:
        fprintf(stderr, "%s [-v] [-r] [-s|m|b|l] [seeds...]\n", argv[0]);
        exit(1);
    }

    // Config options for TestU01
    swrite_Basic = FALSE;  // Turn of TestU01 verbosity by default
                           // reenable by -v option.

    // Config options for generator output
    bool reverseBits = false;
    bool highBits = false;

    // Config options for tests
    bool testSmallCrush = false;
    bool testCrush = false;
    bool testBigCrush = false;
    bool testLinComp = false;
    bool testSkip = false;

    // Handle command-line option switches
    while (1)
    {
        --argc;
        ++argv;
        if ((argc == 0) || (argv[0][0] != '-'))
            break;
        if ((argv[0][1]=='\0') || (argv[0][2]!='\0'))
            goto usage;
        switch(argv[0][1])
        {
            case 'r':
                reverseBits = true;
                break;      
            case 'h':
                highBits = true;
                break;      
            case 's':
                testSmallCrush = true;
                break;
            case 'm':
                testCrush = true;
                break;
            case 'b':
                testBigCrush = true;
                break;
            case 'l':
                testLinComp = true;
                break;
            case 'v':
                swrite_Basic = TRUE;
                break;
            case 'k':
                testSkip = true;
                break;
            default:
                goto usage;
        }
    }

    if (argc > 1)
    {
#if HAVE___UINT128_T
        rng_state = ((uint128_v )strtoull(argv[0], NULL, 0) << 64) | strtoull(argv[1], NULL, 0);
#else
        rng_state.hi = strtoull(argv[0], NULL, 0);
        rng_state.lo = strtoull(argv[1], NULL, 0);
#endif
    }
    else if (argc > 0)
    {
#if HAVE___UINT128_T
        rng_state = strtoull(argv[0], NULL, 0);
#else
        rng_state.hi = 0u;
        rng_state.lo = strtoull(argv[0], NULL, 0);
#endif
    }
    else
    {
        // punt, use default
    }

    if (highBits)
    {
        if (reverseBits)
        {
            gen.GetBits = minstd_64_32hirev;
            gen.name = "minstd_64 high32 reversed";
        }
        else
        {
            gen.GetBits = minstd_64_32hi;
            gen.name = "minstd_64 high32";
        }
    }
    else
    {
        if (reverseBits)
        {
            gen.GetBits = minstd_64_32lorev;
            gen.name = "minstd_64 low32 reversed";
        }
        else
        {
            gen.GetBits = minstd_64_32lo;
            gen.name = "minstd_64 low32";
        }
    }

#if HAVE___UINT128_T
    printf("Using __uint128_t\n");
#else
    printf("Using emulated uint128_v\n");
#endif

#if 0
    for (int i = 0; i < 1000; i++)
    {
        printf("0x%016" PRIx64 "\n", minstd_64());
    }
#endif

    if (swrite_Basic)
    {
        // verbose
        printf("0x%016" PRIx64 "\n", minstd_64());
    }

    if (testSmallCrush)
    {
        bbattery_SmallCrush(&gen);
        fflush(stdout);
    }
    if (testCrush)
    {
        bbattery_Crush(&gen);
        fflush(stdout);
    }
    if (testBigCrush)
    {
        bbattery_BigCrush(&gen);
        fflush(stdout);
    }
    if (testLinComp)
    {
        static int sizes[] = {250, 500, 1000, 5000, 25000, 50000, 75000};
        scomp_Res* res = scomp_CreateRes();
        swrite_Basic = TRUE;
        for (int i = 0; i < sizeof(sizes)/sizeof(int); i++)
            scomp_LinearComp(&gen, res, 1, sizes[i], 0, 1);
        scomp_DeleteRes(res);
        fflush(stdout);
    }
    if (testSkip)
    {
        uint128_v rng_s = rng_state;
        assert(minstd_64() ==  minstd_64_next(&rng_s));
        //write_128(rng_state);
        //write_128(rng_s);
        uint64_t r0 = minstd_64_skip(&rng_s, UINT128_C(0,1));
        assert(minstd_64() ==  r0);
        //printf("0x%016" PRIx64 "\n", minstd_64());
        //write_128(rng_state);
        //printf("0x%016" PRIx64 "\n", minstd_64_skip(&rng_s, UINT128_C(0,1)));
        //write_128(rng_s);

        // four big skips summing to a full cycle
        uint128_v s1 = UINT128_C(0x0fc94e3bf4e9ab32uLL, 0x866458cd56f5e605uLL);
        uint128_v s2 = UINT128_C((0x8000000000000000uLL - s1.hi) - 1, 0uLL - s1.lo);
        uint128_v s3 = UINT128_C(0x2d99787926d46932uLL, 0xa4c1f32680f70c55uLL);
        uint128_v s4 = UINT128_C((0x8000000000000000uLL - s3.hi) - 1, 0uLL - s3.lo);

        // just confirm that the four test steps sum to zero, i.e., a full cycle
        uint128_v ss;
        ss = add128(s1, s2);
        ss = add128(ss, s3);
        ss = add128(ss, s4);
        assert(ss.hi == 0uLL);
        assert(ss.lo == 0uLL);

        // take the four skips
        uint64_t r1 = minstd_64_skip(&rng_s, s1);
        uint64_t r2 = minstd_64_skip(&rng_s, s2);
        uint64_t r3 = minstd_64_skip(&rng_s, s3);
        uint64_t r4 = minstd_64_skip(&rng_s, s4);

        (void )r1; // ignore the intermediary values
        (void )r2;
        (void )r3;

        // confirm that we're back where we started
        assert(r0 == r4);
        assert(minstd_64() ==  minstd_64_next(&rng_s));
    }

    return 0;
}
#endif
