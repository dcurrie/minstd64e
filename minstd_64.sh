# TODO - make this independent of my installation of TestU01

export LD_LIBRARY_PATH=/Users/e/dev/random/TestU01/install/lib:${LD_LIBRARY_PATH}
export LIBRARY_PATH=/Users/e/dev/random/TestU01/install/lib:${LIBRARY_PATH}
export C_INCLUDE_PATH=/Users/e/dev/random/TestU01/install/include:${C_INCLUDE_PATH}

cc -std=c99 -Wall minstd_64.c -O3 -DTESTU01=1 -lprobdist -lmylib -ltestu01 -o crush-minstd_64-e
# testing compile no __int128 ...
cc -std=c99 -Wall minstd_64.c -O3 -DTESTU01=1 -DHAVE___UINT128_T=0 -lprobdist -lmylib -ltestu01 -o crush-minstd_64-e-tst
