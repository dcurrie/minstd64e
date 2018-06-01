#include <cstdio>
#include <cstddef>
#include <cstdint>
#include <cinttypes>

//typedef unsigned __int128 uint128_t;
typedef __uint128_t uint128_t;

#define UINT128_C(h,l) (((uint128_t )h << 64) + ((uint128_t )l))


/*
** (number->string 52583122484843402430317208685168068605 16)
** "278f2419a4d3a5f7 c2280069635487fd"
** > #x278f2419a4d3a5f7c2280069635487fd
** 52583122484843402430317208685168068605
*/
/*
** > (number->string 161208845413696081454645897829429164777 16)
** "7947b1bb901b788587850692e84a3ae9"
** > #x7947b1bb901b788587850692e84a3ae9
** 161208845413696081454645897829429164777
*/
/*
** > (number->string 44629693204054986313095191502463994785 16)
** "21935dc2e417e6615a3a5a07fd848ba1"
** > #x21935dc2e417e6615a3a5a07fd848ba1
** 44629693204054986313095191502463994785
*/

const uint128_t A = UINT128_C(0x21935dc2e417e661ULL, 0x5a3a5a07fd848ba1ULL);
const uint128_t C = 1;

static uint128_t rng_state = 1;

static uint64_t rng()
{
    rng_state = (rng_state * A) + C;
    return (uint64_t )(rng_state >> 64);
}

int main()
{
    freopen(NULL, "wb", stdout);  // Only necessary on Windows, but harmless.

    constexpr size_t BUFFER_SIZE = 1024 * 1024 / sizeof(uint64_t);
    static uint64_t buffer[BUFFER_SIZE];

    fprintf(stderr, "A: 0x%016" PRIx64 "_%016" PRIx64 "\n", (uint64_t )(A>>64), (uint64_t )A);
    fprintf(stderr, "C: 0x%016" PRIx64 "_%016" PRIx64 "\n", (uint64_t )(C>>64), (uint64_t )C);
    fprintf(stderr, "S: 0x%016" PRIx64 "_%016" PRIx64 "\n", (uint64_t )(rng_state>>64), (uint64_t )rng_state);

    while (1) {
        for (size_t i = 0; i < BUFFER_SIZE; ++i)
            buffer[i] = rng();
        fwrite((void*) buffer, sizeof(buffer[0]), BUFFER_SIZE, stdout);
    }
}
