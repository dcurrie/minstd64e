/*--------------------------------------------------------------------

Search for LCG multiplier using the LLL-Spectral Test!  January 2000

Authors: Karl Entacher, Karl.Entacher@fh-sbg.ac.at 

         Thomas Schell, Dept. of Scientific Computing, Univ. Salzburg

--------------------------------------------------------------------*/

// Modified by e 2018-05-10 for LCG with power of two modulus


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



ZZ lambda, modul, seed;



struct {

  ZZ lambda;

  double min_norm_x_x;

} best;


#define MAX_DIMS 24

#define DIMS 24 // TODO: make this a command line arg

double fact[MAX_DIMS];

RR fact_RR[MAX_DIMS];

const int x_xmax = DIMS;       /* Test for dimensions <= DIMS */

const ZZ n_min=to_ZZ(1);

//const ZZ n_max=to_ZZ(10000);
const ZZ n_max=to_ZZ(1000000);



/*-----------------------------------------------------------------------

   Call the function with:

        lll_search ["output-file"] [power-of-two-modulus] [seed]

   Example:

        lll_esearch "output-file" 340282366920938463463374607431768211456 3

  ----------------------------------------------------------------------*/



int main(int argc, char *argv[])
{

/*--------------------------    Input-Check        -----------------------*/

    if (!(argc == 4))
    {
        cout << "lll_search_rnd <output-file-name> <power-of-two-modulus> <seed>" << endl;
        exit(0);
    }

/*--------- reads the input-parameters: "file" modulus seed ------*/

    ofstream out_file(argv[1], ios::out);

    modul = to_ZZ(argv[2]);

    //seed = to_ZZ(argv[3]);
    SetSeed((const unsigned char *)argv[3], strlen(argv[3]));

    long blen = (long )(log(modul)/log(2.0)) - 2L;

/*------------- output of modulus and seed ---------------*/

    out_file << modul;

    //out_file << seed;

    out_file << " " << blen;

    out_file << " " << RandomBits_ZZ(blen);

    out_file << endl;


/*------------- Constants for the normalized Spectral Test   -----------------*/

    fact[0] = 0.0;                                // intentionally left uninitialized -> not used

#if 0

    fact[1] = to_double(to_RR(1.0) / pow(to_RR(4.0/3.0), to_RR(1.0/4.0)) / pow(to_RR(modul), to_RR(1.0/2.0)));

    fact[2] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(1.0/6.0)) / pow(to_RR(modul), to_RR(1.0/3.0)));

    fact[3] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(1.0/4.0)) / pow(to_RR(modul), to_RR(1.0/4.0)));

    fact[4] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(3.0/10.0)) / pow(to_RR(modul), to_RR(1.0/5.0)));

    fact[5] = to_double(to_RR(1.0) / pow(to_RR(64.0/3.0), to_RR(1.0/12.0)) / pow(to_RR(modul), to_RR(1.0/6.0)));

    fact[6] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(3.0/7.0)) / pow(to_RR(modul), to_RR(1.0/7.0)));

    fact[7] = to_double(to_RR(1.0) / pow(to_RR(2.0), to_RR(1.0/2.0)) / pow(to_RR(modul), to_RR(1.0/8.0)));

#else

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


  for (int i = 1; i < x_xmax; i++)
  {
    fact[i] = to_double(1 / (to_RR(2) * pow(fact_RR[i] * to_RR(modul), to_RR(1.0) / to_RR(i+1))));
  }

#endif

/*------------Search, Matrix (Basis) Input, LLL und Output -------------*/

    best.lambda = to_ZZ(0);

    best.min_norm_x_x = 0.0;

    ZZ t_h = to_ZZ(10);

    struct rusage cur_ru;

    for (ZZ n = n_min; n <= n_max; n++)
    {
        double min_norm_x_x = 0.0;

        lambda = RandomBits_ZZ(blen) * 4 + 1;

        mat_ZZ x;

        x.SetDims(x_xmax, x_xmax);

        min_norm_x_x = 1.0;

        for (int j = 2; j <= x_xmax; j++)
        {

            x.SetDims(j,j);

            x[0][0] = modul;     // first index = rows, second index = columns

            for (int i = 1; i < j; i++)          // fill in the 1s

                x[i][i] = 1;

            for (int i = 1; i < j; i++)

                x[i][0] =-(power(lambda, i)); 

            ZZ det, rg;

            rg = LLL(det, x, 0);

            double min_x_x = to_double(x[0] * x[0]);

            for (int i = 1; i < j; i++)
            {
                double x_x = to_double(x[i] * x[i]);

                if (min_x_x > x_x)

                    min_x_x = x_x;
            }

            double norm_x_x = fact[j-1] * sqrt(min_x_x);

            if (min_norm_x_x > norm_x_x)

                min_norm_x_x = norm_x_x;
        }

        if (min_norm_x_x > best.min_norm_x_x)
        {
            best.min_norm_x_x = min_norm_x_x;

            best.lambda = lambda;

            getrusage(RUSAGE_SELF, &cur_ru);

            out_file << "time\t" << cur_ru.ru_utime.tv_sec << "\tn\t" << n << "\tl\t" << lambda << "\t" << min_norm_x_x << endl;
        }

        ZZ n_ = n - n_min;

        if (n_ >= t_h)
        {
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
