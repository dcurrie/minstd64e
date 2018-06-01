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

/*------------------------ Definitions ------------------------------*/

ZZ mult, modul; 

const int dim_max = 24;

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

  output_file << "==============================================================" <<endl;

  output_file << "[k]\t" << "[dim]\t" << "[n_spect]\t" << endl << endl;



/*------------- Constants for the normalized Spectral Test   ----------------*/

  fact_RR[0] = to_RR(0.0);  // intentionally left uninitialized -> not used

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



  for (int i = 1; i < dim_max; i++) {

    fact[i] = to_double(1 / (to_RR(2) * pow(fact_RR[i] * to_RR(modul), to_RR(1.0) / to_RR(i+1))));

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
