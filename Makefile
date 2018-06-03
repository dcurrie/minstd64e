

TESTU01_DIR=/Users/e/dev/random/TestU01

LD_LIBRARY_PATH:=$(TESTU01_DIR)/install/lib:"$(LD_LIBRARY_PATH)"
export LD_LIBRARY_PATH

INC = -I$(TESTU01_DIR)/install/include
LIBS = -L$(TESTU01_DIR)/install/lib -lprobdist -lmylib -ltestu01

crush-minstd_64-e: minstd_64.c minstd_64.h
	cc -O3 -std=c99 -Wall minstd_64.c -DTESTU01=1 $(INC) $(LIBS) -o $@

# force use of non-__int128 code
crush-minstd_64-e2: minstd_64.c minstd_64.h
		cc -O3 -std=c99 -Wall minstd_64.c -DHAVE___UINT128_T=0 -DTESTU01=1 $(INC) $(LIBS) -o $@
