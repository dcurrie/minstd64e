/*--------------------------------------------------------------------

Normalized LLL-Spectral Test for Linear Congruential Generators!

Authors: Karl Entacher, Karl.Entacher@fh-sbg.ac.at    January 2000

         Thomas Schell, Dept. of Scientific Computing, Univ. Salzburg

--------------------------------------------------------------------*/





/*-------------Libraries (needs Victor Shoups NTL-Lib----------------*/

#include <iostream>

#include <cinttypes>

#include <fstream>

#include <time.h>

#include <sys/resource.h>

#include <math.h>

#include <float.h>

#include <NTL/ZZ.h>

#include <NTL/RR.h>

#include <NTL/mat_ZZ.h>

#include <NTL/LLL.h>

using namespace std;
using namespace NTL;

/* Set one of USE_Entacher_CONSTS or USE_Cohn_Elkies_CONSTS to 1, the other 0
** Set USE_ONeill_CONSTS 1 to extend original Entacher_CONSTS to 48 dimensions
*/
#define USE_Entacher_CONSTS (1)
#define USE_Cohn_Elkies_CONSTS (0)
#define USE_ONeill_CONSTS (1)

/*------------------------ Definitions ------------------------------*/

ZZ mult, modul;

#if USE_Entacher_CONSTS
#if USE_ONeill_CONSTS
const int dim_max = 48;
#else
const int dim_max = 24;
#endif
#endif

#if USE_Cohn_Elkies_CONSTS
const int dim_max = 36;
#endif

RR fact_RR[dim_max];

double fact[dim_max];

int x_xmax;



/*-----------------------------------------------------------------------

   Call the function with:

        lll_spect ["output-file"] [mult] [modul]

   Example:

        lll_spect "output-file" 8 16907 2147483647

  ----------------------------------------------------------------------*/




static char *hexstr128(ZZ x, char *buf)
{
    //std::snprintf(buf, 36, "0x%016" PRIx64 "_%016" PRIx64 "", conv<unsigned long>(x>>64), conv<unsigned long>(x));
    std::snprintf(buf, 36, "0x%08" PRIx32 "%08" PRIx32 "_%08" PRIx32 "%08" PRIx32 ""
        , conv<unsigned int>(x>>96)
        , conv<unsigned int>(x>>64)
        , conv<unsigned int>(x>>32)
        , conv<unsigned int>(x));
    return buf;
}



int main(int argc, char *argv[]) {

    char buf[40];

/*--------------------------    Input-Check          -----------------------*/

  if (!(argc >= 4)) {

    cout << "lll_spect <output-file-name> <dim> <mult> <modul>" << endl;

    exit(0);

  }

/*--------  Reads the input parameters: "file" dim mult modul -------------*/

  ofstream output_file(argv[1], ios::out);

  x_xmax = atoi(argv[2]);

  mult = to_ZZ(argv[3]);

  modul = to_ZZ(argv[4]);



/*-------------Output of mult modul ----------------------------------------*/

  output_file << "==============================================================" <<endl;

  output_file << "Modulus:      \t\t" << modul <<endl;

  output_file << "Multiplikator:\t\t" << mult <<endl;

  output_file << "Multipler hex:\t\t" << hexstr128(mult,buf) <<endl;

  output_file << "Dimension bis:\t\t" << x_xmax<<endl;

#if USE_Entacher_CONSTS
#if USE_ONeill_CONSTS
  output_file << "Using Entacher & O'Neill Constants" <<endl;
#else
  output_file << "Using Entacher Constants" <<endl;
#endif
#endif

#if USE_Cohn_Elkies_CONSTS
  output_file << "Using Cohn & Elkies Constants" <<endl;
#endif

  output_file << "==============================================================" <<endl;

  output_file << "[k]\t" << "[dim]\t" << "[n_spect]\t" << endl << endl;



/*------------- Constants for the normalized Spectral Test   ----------------*/

/* From Melissa O'Neill, Mathematica definition for the constants up to 48:
        { 1/(2 Sqrt[3]), 1/(4 Sqrt[2]), 1/8, 1/(8 Sqrt[2]), 1/(8 Sqrt[3]),
          1/16, 1/16, 0.06007, 0.05953, 0.06136, 0.06559, 0.07253,
          0.08278, 0.09735, 0.11774, 0.14624, 0.18629, 0.24308, 0.32454,
          0.44289, 0.61722, 0.87767, 1.27241, 1.87977, 2.82677, 4.32515,
          6.72955, 10.6416, 17.0940, 27.8797, 46.1471, 77.4869, 131.937,
          227.715, 398.245, 705.496, 1265.56, 2298.18, 4223.47, 7852.67,
          14767.7, 28083.1, 53989.6,  104907,  205984,  408604,  818696 }
Melissa says: I've confirmed that they reproduce L'Ecuyer's work.
*/
  fact_RR[0] = to_RR(0.0);  // intentionally left uninitialized -> not used

#if USE_Entacher_CONSTS

  fact_RR[1] = to_RR(0.2886751345948128822545744);
  fact_RR[2] = to_RR(0.1767766952966368811002111);
  fact_RR[3] = to_RR(0.125);
  fact_RR[4] = to_RR(0.0883883476483184405501055);
  fact_RR[5] = to_RR(0.0721687836487032205636436);
  fact_RR[6] = to_RR(0.0625);
  fact_RR[7] = to_RR(0.0625);
  fact_RR[8] = to_RR(0.06007);
  fact_RR[9] = to_RR(0.05953);
  fact_RR[10] = to_RR(0.06136);
  fact_RR[11] = to_RR(0.06559);
  fact_RR[12] = to_RR(0.07253);
  fact_RR[13] = to_RR(0.08278);
  fact_RR[14] = to_RR(0.09735);
  fact_RR[15] = to_RR(0.11774);
  fact_RR[16] = to_RR(0.14624);
  fact_RR[17] = to_RR(0.18629);
  fact_RR[18] = to_RR(0.24308);
  fact_RR[19] = to_RR(0.32454);
  fact_RR[20] = to_RR(0.44289);
  fact_RR[21] = to_RR(0.61722);
  fact_RR[22] = to_RR(0.87767);
  fact_RR[23] = to_RR(1.27241);

#if USE_ONeill_CONSTS

  fact_RR[24] = to_RR(1.87977);
  fact_RR[25] = to_RR(2.82677);
  fact_RR[26] = to_RR(4.32515);
  fact_RR[27] = to_RR(6.72955);
  fact_RR[28] = to_RR(10.6416);
  fact_RR[29] = to_RR(17.0940);
  fact_RR[30] = to_RR(27.8797);
  fact_RR[31] = to_RR(46.1471);
  fact_RR[32] = to_RR(77.4869);
  fact_RR[33] = to_RR(131.937);
  fact_RR[34] = to_RR(227.715);
  fact_RR[35] = to_RR(398.245);
  fact_RR[36] = to_RR(705.496);
  fact_RR[37] = to_RR(1265.56);
  fact_RR[38] = to_RR(2298.18);
  fact_RR[39] = to_RR(4223.47);
  fact_RR[40] = to_RR(7852.67);
  fact_RR[41] = to_RR(14767.7);
  fact_RR[42] = to_RR(28083.1);
  fact_RR[43] = to_RR(53989.6);
  fact_RR[44] = to_RR( 104907);
  fact_RR[45] = to_RR( 205984);
  fact_RR[46] = to_RR( 408604);
  fact_RR[47] = to_RR( 818696);
#endif
#endif

#if USE_Cohn_Elkies_CONSTS

/* See Annals of Mathematics, 157 (2003), 689–714
  New upper bounds on sphere packings I
  By Henry Cohn and Noam Elkies
*/
  fact_RR[ 1] = to_RR(0.28868);
  fact_RR[ 2] = to_RR(0.18616);
  fact_RR[ 3] = to_RR(0.13126);
  fact_RR[ 4] = to_RR(0.09975);
  fact_RR[ 5] = to_RR(0.08084);
  fact_RR[ 6] = to_RR(0.06933);
  fact_RR[ 7] = to_RR(0.06251);
  fact_RR[ 8] = to_RR(0.05900);
  fact_RR[ 9] = to_RR(0.05804);
  fact_RR[10] = to_RR(0.05932);
  fact_RR[11] = to_RR(0.06279);
  fact_RR[12] = to_RR(0.06870);
  fact_RR[13] = to_RR(0.07750);
  fact_RR[14] = to_RR(0.08999);
  fact_RR[15] = to_RR(0.10738);
  fact_RR[16] = to_RR(0.13150);
  fact_RR[17] = to_RR(0.16503);
  fact_RR[18] = to_RR(0.21202);
  fact_RR[19] = to_RR(0.27855);
  fact_RR[20] = to_RR(0.37389);
  fact_RR[21] = to_RR(0.51231);
  fact_RR[22] = to_RR(0.71601);
  fact_RR[23] = to_RR(1.01998);
  fact_RR[24] = to_RR(1.48001);
  fact_RR[25] = to_RR(2.18614);
  fact_RR[26] = to_RR(3.28537);
  fact_RR[27] = to_RR(5.02059);
  fact_RR[28] = to_RR(7.79782);
  fact_RR[29] = to_RR(12.30390);
  fact_RR[30] = to_RR(19.71397);
  fact_RR[31] = to_RR(32.06222);
  fact_RR[32] = to_RR(52.90924);
  fact_RR[33] = to_RR(88.55925);
  fact_RR[34] = to_RR(150.29783);
  fact_RR[35] = to_RR(258.54994);
#endif

  for (int i = 1; i < dim_max; i++) {

    fact[i] = to_double(1 / (to_RR(2) * pow(fact_RR[i] * to_RR(modul), to_RR(1.0) / to_RR(i+1))));

    // cout << fact[i] << endl;

  }



/*---------------------------------for time measurements------------------*/

  struct rusage cur_ru;





/*------------Definition of the Matrix (Basis) Input, LLL und Output ------*/

  mat_ZZ x;

  x.SetDims(x_xmax, x_xmax);

  double norm;

      for (int j = 2; j <= x_xmax; j++) {

        x.SetDims(j,j);

        x[0][0] = modul;     // first Index: Rows, second Index: columns

        for (int i = 1; i < j; i++)     // fill in ones

          x[i][i] = 1;

        for (int i = 1; i < j; i++)

          x[i][0] =-(power(mult, i));

        ZZ det, rg;

        rg = LLL(det, x, 0);

        double min_x_x = to_double(x[0] * x[0]);

        for (int i = 1; i < j; i++) {

          double x_x = to_double(x[i] * x[i]);

          if (min_x_x > x_x)

            min_x_x = x_x;

        }

        norm = fact[j-1] * sqrt(min_x_x);           //Normalization

        output_file << j << "\t" << norm << endl;

      }

/*-------------------------Output of Timings-----------------------------*/

  getrusage(RUSAGE_SELF, &cur_ru);

  cout << "total time elapsed\t" << cur_ru.ru_utime.tv_sec << endl;

  output_file << endl << "Time [s]:\t" << cur_ru.ru_utime.tv_sec << endl;

  output_file.close();

}
