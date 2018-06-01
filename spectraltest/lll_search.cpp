/*--------------------------------------------------------------------

Search for LCG multiplier using the LLL-Spectral Test!  January 2000

Authors: Karl Entacher, Karl.Entacher@fh-sbg.ac.at 

         Thomas Schell, Dept. of Scientific Computing, Univ. Salzburg

--------------------------------------------------------------------*/



/*-------------Libraries (needs Victor Shoups NTL-Lib----------------*/

#include <iostream>

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



ZZ lambda, modul, *m_primes;



struct {

  ZZ lambda;

  double min_norm_x_x;

} best;



double fact[8];



const int x_xmax = 8;       /* Test for dimensions <= 8 */

const ZZ n_min=to_ZZ(1);

//const ZZ n_max=to_ZZ(10000);
const ZZ n_max=to_ZZ(10000000);



/*-----------------------------------------------------------------------

   Call the function with:

        lll_search ["output-file"] [modulus] [prim-factors of modulus-1] 

   Example:

        lll_search "output-file" 2147483647 2 3 7 11 31 151 331

  ----------------------------------------------------------------------*/



int main(int argc, char *argv[]) {



/*--------------------------    Input-Check        -----------------------*/

  if (!(argc >= 2)) {

    cout << "lll_search_rnd <output-file-name> <modul> <prime factors of modul-1>" << endl;

    exit(0);

  }



/*--------- reads the input-parameters: "file" modulus primefactors ------*/

  ofstream out_file(argv[1], ios::out);

  modul = to_ZZ(argv[2]);

  if (argc-5 > 0) {

    m_primes = new ZZ [argc-3];

    for (int i = 3; i < argc; i++)

      m_primes[i-3] = to_ZZ(argv[i]);

  }



/*------------- output of modulus and factors of modulus-1 ---------------*/

  out_file << modul;

  for (int i = 0; i < argc-3; i++)

    out_file << " " << m_primes[i];

  out_file << endl;



/*-----------------Search for the first primitiv root and its output-------*/

  int is_primitive_root;

  ZZ pr = to_ZZ(1);

  do {

    int k = 0;

    pr++;

    while (k < argc-3 && (is_primitive_root = (PowerMod(pr, (modul-1) / m_primes[k++], modul) != 1)));

  } while (!is_primitive_root);

  cout << "primitive root " << pr << endl;



/*------------- Constants for the normalized Spectral Test   -----------------*/

  fact[0] = 0.0;                                // intentionally left uninitialized -> not used

  fact[1] = to_double(to_RR(1.0) / pow(to_RR(4.0/3.0), to_RR(1.0/4.0)) / pow(to_RR(modul), to_RR(1.0/2.0)));

  fact[2] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(1.0/6.0)) / pow(to_RR(modul), to_RR(1.0/3.0)));

  fact[3] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(1.0/4.0)) / pow(to_RR(modul), to_RR(1.0/4.0)));

  fact[4] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(3.0/10.0)) / pow(to_RR(modul), to_RR(1.0/5.0)));

  fact[5] = to_double(to_RR(1.0) / pow(to_RR(64.0/3.0), to_RR(1.0/12.0)) / pow(to_RR(modul), to_RR(1.0/6.0)));

  fact[6] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(3.0/7.0)) / pow(to_RR(modul), to_RR(1.0/7.0)));

  fact[7] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(1.0/2.0)) / pow(to_RR(modul), to_RR(1.0/8.0)));



/*------------Search, Matrix (Basis) Input, LLL und Output -------------*/

  best.lambda = to_ZZ(0);

  best.min_norm_x_x = 0.0;

  ZZ t_h = to_ZZ(10);

  struct rusage cur_ru;

  for (ZZ n = n_min; n <= n_max; n++) {

    double min_norm_x_x = 0.0;

    if (GCD(n, modul - 1) == 1) {

      lambda = PowerMod(pr, n, modul);

      mat_ZZ x;

      x.SetDims(x_xmax, x_xmax);

      min_norm_x_x = 1.0;

      for (int j = 2; j <= x_xmax; j++) {

    x.SetDims(j,j);

    x[0][0] = modul;     // first index = rows, second index = columns

    for (int i = 1; i < j; i++)          // fill in the 1s

      x[i][i] = 1;

    for (int i = 1; i < j; i++)

      x[i][0] =-(power(lambda, i)); 

    ZZ det, rg;

    rg = LLL(det, x, 0);

    double min_x_x = to_double(x[0] * x[0]);

    for (int i = 1; i < j; i++) {

      double x_x = to_double(x[i] * x[i]);

      if (min_x_x > x_x)

        min_x_x = x_x;

    }

    double norm_x_x = fact[j-1] * sqrt(min_x_x);

    if (min_norm_x_x > norm_x_x)

      min_norm_x_x = norm_x_x;

      }

      if (min_norm_x_x > best.min_norm_x_x) {

    best.min_norm_x_x = min_norm_x_x;

    best.lambda = lambda;

    getrusage(RUSAGE_SELF, &cur_ru);

    out_file << "time\t" << cur_ru.ru_utime.tv_sec << "\tn\t" << n << "\tl\t" << lambda << "\t" << min_norm_x_x << endl;

      }

    }

    ZZ n_ = n - n_min;

    if (n_ >= t_h) {

      getrusage(RUSAGE_SELF, &cur_ru);

      cout << n_ << "\t" << cur_ru.ru_utime.tv_sec << endl;

      t_h *= 10;

    }

  }

  getrusage(RUSAGE_SELF, &cur_ru);

  cout << "total time elapsed\t" << cur_ru.ru_utime.tv_sec << endl;

  out_file << "time\t" << cur_ru.ru_utime.tv_sec << "\tl\t" << best.lambda << "\t" << best.min_norm_x_x << endl;

  out_file.close();

}
