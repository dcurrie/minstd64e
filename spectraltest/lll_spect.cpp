/*--------------------------------------------------------------------

Normalized LLL-Spectral Test for Linear Congruential Generators!

Authors: Karl Entacher, Karl.Entacher@fh-sbg.ac.at    January 2000

         Thomas Schell, Dept. of Scientific Computing, Univ. Salzburg

--------------------------------------------------------------------*/


/* Modified by Doug Currie, March 2021, with help from Melissa O'Neill
// -- extended fact_RR[] constants beyond 24 using L'Ecuyer, Leech, Rogers values
// -- added alternate fact_RR[] constants from Cohn & Elkies
// -- switched from simpler LLL algorithm to BKZ version to obtain accuracy
//     equivalent to Karl Entacher's Mathematica implementation
// -- added optparse to control selection of constants and BKZ blocksize
*/

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

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static inline
#include "optparse.h"

using namespace std;
using namespace NTL;


/*------------------------ Definitions ------------------------------*/

ZZ mult, modul;

const int dim_max_max = 48;

RR fact_RR[dim_max_max];

double fact[dim_max_max];

int x_xmax;



/*-----------------------------------------------------------------------

   Call the function with:

        lll_spect <output-file-name> <dim> <mult> <modul>

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

static void usage()
{
  cout << "lll_spect [-c|--Cohn_Elkies [0|1]] [-b|--blocksize <2..48>] [-h|--help] <output-file-name> <dim> <mult> <modul>" << endl;
}

static void help()
{
  usage();
  cout << "   [-c|--Cohn_Elkies [0|1]]" << endl
       << "     O: use original Entacher constants extended with L'Ecuyer, Leech, Rogers values" << endl
       << "     1: use Cohn & Elkies new upper bounds constants" << endl
       << "      : Cohn & Elkies constants used when argument omitted" << endl
       << "      : without -c the original Entacher constants are used" << endl;
  cout << "   [-b|--blocksize <2..48>]" << endl
       << "     2: fastest, most conservative, least accurate" << endl
       << "    30: slower, matches Karl Entacher's Mathematica implementation in accuracy" << endl
       << "      : between 2 and 30 will trade speed and accuracy" << endl
       << "      : larger values are permitted but should be smaller than <dim> to be meaningful" << endl
       << "      : without -b the default value 10 is used" << endl;
  //cout << " " << endl;
}

int main(int argc, char *argv[]) {

    char buf[40];

    int constset = 0; // default to original Entacher constants extended with L'Ecuyer, Leech, Rogers values
    int dim_max = 48; // varies by constset
    int blocksize = 10;

    struct optparse_long longopts[] = {
        {"Cohn_Elkies", 'c', OPTPARSE_OPTIONAL}, // OPTPARSE_NONE
        {"blocksize", 'b', OPTPARSE_REQUIRED},
        {"help", 'h', OPTPARSE_NONE},
        {0}
    };

    int option;
    struct optparse options;

    optparse_init(&options, argv);
    options.permute = 0;

/*--------  Reads the input parameters: "file" dim mult modul -------------*/

  while ((option = optparse_long(&options, longopts, NULL)) != -1)
  {
    switch (option)
    {
      case 'b':
          blocksize = atoi(options.optarg);
          break;
      case 'c':
          constset = options.optarg
                     ? (atoi(options.optarg) == 0 ? 0 : 1)
                     : 1;
          break;
      case 'h':
          help();
          exit(0);
      case '?':
          fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
          usage();
          exit(EXIT_FAILURE);
    }
  }

  if ((argc - options.optind) != 4)
  {
    usage();
    exit(EXIT_FAILURE);
  }

  ofstream output_file(optparse_arg(&options), ios::out);

  x_xmax = atoi(optparse_arg(&options));

  mult = to_ZZ(optparse_arg(&options));

  modul = to_ZZ(optparse_arg(&options));



/*-------------Output of mult modul ----------------------------------------*/

  output_file << "==============================================================" <<endl;

  output_file << "Modulus:      \t\t" << modul <<endl;

  output_file << "Multiplikator:\t\t" << mult <<endl;

  output_file << "Multipler hex:\t\t" << hexstr128(mult,buf) <<endl;

  output_file << "Dimension bis:\t\t" << x_xmax<<endl;

  output_file << "Block Korkin-Zolotarev reduction blocksize:\t" << blocksize <<endl;

  if (constset == 0)
  {
    output_file << "Using Entacher & L'Ecuyer/Leech/Rogers Constants" <<endl;
    dim_max = 48;
  }
  else
  {
    output_file << "Using Cohn & Elkies Constants" <<endl;
    dim_max = 36;
  }

  assert(dim_max <= dim_max_max);
  assert((x_xmax > 0) && (x_xmax <= dim_max));

  output_file << "==============================================================" <<endl;

  output_file << "[k]\t" << "[dim]\t" << "[n_spect]\t" << endl << endl;



/*------------- Constants for the normalized Spectral Test   ----------------*/

  fact_RR[0] = to_RR(0.0);  // intentionally left uninitialized -> not used

  if (constset == 0)
  {
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

    /* From Melissa O'Neill, Mathematica definition for the constants up to 48:
        { 1/(2 Sqrt[3]), 1/(4 Sqrt[2]), 1/8, 1/(8 Sqrt[2]), 1/(8 Sqrt[3]),
          1/16, 1/16, 0.06007, 0.05953, 0.06136, 0.06559, 0.07253,
          0.08278, 0.09735, 0.11774, 0.14624, 0.18629, 0.24308, 0.32454,
          0.44289, 0.61722, 0.87767, 1.27241, 1.87977, 2.82677, 4.32515,
          6.72955, 10.6416, 17.0940, 27.8797, 46.1471, 77.4869, 131.937,
          227.715, 398.245, 705.496, 1265.56, 2298.18, 4223.47, 7852.67,
          14767.7, 28083.1, 53989.6,  104907,  205984,  408604,  818696 }
      Melissa says: I've confirmed that they reproduce L'Ecuyer's work.
      Also see file calcconsts.c
      L'Ecuyer's paper recommends use of these constants for dimensions beyond 24,
      although the formula he gives is is due to Leech
      (https://dx.doi.org/10.4153/CJM-1971-081-3, p.743)
      based on the work of Rogers (https://doi.org/10.1112/plms/s3-8.4.609)
    */
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

  }
  else
  {
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
  }

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

        //rg = LLL(det, x, 0);
        //rg = G_LLL_RR(x);
        //rg = G_LLL_XD(x); // same results as _RR
        //rg = G_LLL_QP(x); // G_LLL_QP: numbers too big...use G_LLL_XD
        //rg = LLL_XD(x); // same results as G_LLL_XD, and faster
        //rg = G_BKZ_XD(x); // best results yet, even better...
        //rg = G_BKZ_XD(x, 0.99, 30); // 20: 12 s, 30: 27 s best results yet, matching Karl Entacher's Mathematica implementation
        //rg = BKZ_XD(x, 0.99, 30); // 30 wins at 19 s and same results as G- variant
        //rg = BKZ_XD(x, 0.99, 2); // 2 lower quality, 4 s
        rg = BKZ_XD(x, 0.99, blocksize);

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
