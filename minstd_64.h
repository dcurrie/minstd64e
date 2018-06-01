/** @file minstd_64.h
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

#include <stdint.h>

#ifndef HAVE___UINT128_T
#if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6) || __clang_major__ >= 9) && defined(__x86_64__))
#define HAVE___UINT128_T 1
typedef unsigned __int128 uint128_t;
#endif
#endif

#if HAVE___UINT128_T

typedef uint128_t uint128_v;

#else

typedef struct uint128_s
{
    uint64_t lo;
    uint64_t hi;
} uint128_v;

#endif

uint64_t minstd_64_next (uint128_v *rng_state_p);

void minstd_64_seed (uint128_v *rng_state_p, uint128_v seed);

