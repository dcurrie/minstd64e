/** calcconsts.c
 * by Melissa O'Neill
*/
#include <math.h>
#include <stdio.h>

/* L'Ecuyer's paper says to use these constants for dimensions beyond 24, so
 * so we call them L'Ecuyer's constants, although the formula he gives is is
 * due to Leech (https://dx.doi.org/10.4153/CJM-1971-081-3, p.743) based on the
 * work of Rogers (https://doi.org/10.1112/plms/s3-8.4.609)  They're also
 * listed (up to 36) by Cohn & Elkies.
 * So, "L'Ecuyer's recommended density constants (including Leech's formulation
 * of Rogers's bound)" coded by Melissa O'Neill.
 *
 * Prints out these constants following the relevant formula from Leech (ra)
 * and Rogers (r).
*/

int main() {
 for (int n = 24; n <= 48; ++n) {
   /* Roger's formula, more directly */
   double r = exp(lgamma(n + 2.0) -
                  0.5 * (n * log(4.0 * n * M_PI / M_E) + log(2)) - 1.0);
   /* Leech's formula to calculate numbers recommended by L'Ecuyer */
   double ra = exp2(0.5 * n * log2(n / (4.0 * M_E * M_PI)) + 1.5 * log2(n) -
                    log2(0.5 * M_E * M_2_SQRTPI) + 5.25 / (2.5 + n));
   printf("%d\t%9.6g\t%.6g\n", n, r, ra);
 }
}
