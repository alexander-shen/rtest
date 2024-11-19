# generators.h, generators.c: tools for creating generators and testing
#
# rtest.c: main file that runs robust tests, needs rtest.h
# test_func.c: file with different functions used for testing, needs rtest.h
# TESTS_C: additional tests
# CPP_CODE: parts of the code that involve c++ (here and in ../spectral_tests)
# CPP_INCLUDE: files used by c++ code

# generators.c, generators.h: definition of generators
# ../kolmogorov-smirnov: tools for computing Kolmogorov-Smirnov values

# rtest1m.sh, rtest10m.sh, rtest100m.sh, rtest1g.sh, rtest10g.sh: 
# tests that make sense for 1Mb/10Mb/100Mb/1Gb/10Gb files

# compiler definition
CC = g++

CPP_INCLUDE = minimal_distance.hpp ../spectral_tests/specgraph.h ../spectral_tests/spectral_rtest.h ../spectral_tests/rand.h

# C++ files for distance and spectral tests

CPP_CODE =  ../spectral_tests/specgraph.cpp  ../spectral_tests/spectral_rtest.cpp  ../spectral_tests/rand.cpp

SCRIPTS = rtest1m.sh rtest10m.sh rtest100m.sh rtest1g.sh rtest10g.sh

# different tests implemented in several files
TESTS_C = test_func.c test_opso.c test_oqso.c test_sts_serial.c test_bytedistrib.c test_runs.c test_sums.c test_ent.c test_lz.c test_fft.c test_rank.c test_bitstream.c test_birthday.c test_dist2d.c test_spectral.c

# for Kolmogorov-Smirnov p-value computation
KS_HEADERS = ../kolmogorov-smirnov/ksmirnov.h ../kolmogorov-smirnov/k1smirnov.h ../kolmogorov-smirnov/ks2.h
KS_C = ../kolmogorov-smirnov/ksmirnov.c ../kolmogorov-smirnov/k1smirnov.c ../kolmogorov-smirnov/ks2mp.c

# needed for gsl library and for standard math library
FLAGS = -I /usr/local/include -L /usr/local/bin/ -lgsl -lgslcblas -lm -lgmp

rtest: rtest.c test_func.h $(TESTS_C) $(CPP_INCLUDE) $(CPP_CODE) generators.c generators.h $(KS_HEADERS) $(KS_C) ../general/general.c ../general/general.h; \
$(CC) -o rtest rtest.c $(TESTS_C) $(CPP_CODE) generators.c $(KS_C) ../general/general.c $(FLAGS)

test-generators: test-generators.c generators.c generators.h; \
   gcc -o test-generators test-generators.c generators.c;
   
test-generators-run: test-generators;\
   ./test-generators > tmp.out; diff test-generators.out tmp.out;\
   echo "No diff output means OK"
   
clean: ; \
rm *~ *.o a.out rtest test-generators tmp.out; exit 0;

install: rtest; \
sudo cp rtest $(SCRIPTS) /usr/local/bin/

uninstall: ; \
sudo rm /usr/local/bin/rtest /usr/local/bin/rtest*m.sh
