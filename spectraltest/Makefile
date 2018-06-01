CXXFLAGS=-I/usr/local/include -Wall -std=c++11

CLFLAGS=-L/usr/local/lib

DESTS=subdir???

all: lll_spect lll_search lll_esearch

lll_spect: lll_spect.o
	g++ $(CLFLAGS) -o lll_spect lll_spect.o -lntl -lgmp -lm

lll_search: lll_search.o
	g++ $(CLFLAGS) -o lll_search lll_search.o -lntl -lgmp -lm

lll_esearch: lll_esearch.o
	g++ $(CLFLAGS) -o lll_esearch lll_esearch.o -lntl -lgmp -lm

clean:
	rm -f $(DESTS) *.o
