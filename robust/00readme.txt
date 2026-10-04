Packages used: 
  gmp (ubuntu package libgmp-dev) -- multiprecision arithmetic
  gsl (ubuntu package libgsl-dev) -- gnu statistical library

Files:

Makefile                     compilation et al. instructions

Main program: rtest.c

test_func.h                  contains the forward definitions of all test functions (type test_func)
                             Each of them gets a PRG label, some parameters (in param[2..] and real_param[0..]),
                             and a reference to array where the test results 
                             (long double values [], unsigned long hash[]) should be placed. 
                             (One call may produce several values, their number is called 
                             dimension of the test and is provided as param[2].) 
                             The hash values are some data to resolve ties if needed. 
                             For any test function one may compare (using Kolmogorov-Smirnov test) 
                             the samples obtained when this function is applied to 
                             test generator and etalon generator (see file rtest.c)

test_func.c                  The list of all test functions (with short descriptions) 
                             examples of how the test function could look like:
                             dummy_dimension() (template for debugging purposes only)
                             all_bytes() (number of readings before all 256 bytes appear)
                             all_16() (number of readings before all 16-bit blocks appear) 

                             Adding a new test function requires: 
                             (1) adding the forward declaration in test_func.h; 
                             (2) adding the entry for this function in the list test_func.c; 
                             (3) creating a new .c file with the code, adding to the Makefile variable TESTS_C
test_sts_serial.c            sts_serial() (NIST 2.11, adapted from dieharder)
test_opso.c                  opso() dieharder_opso test according to Marsaglia description
test_oqso.c                  oqso() dieharder_oqso test according to Marsaglia description
test_bytedistrib.c           bytedistrib() reimplemented according to dieharder description
test_runs.c                  knuth_runs() Knuth run test (reimplemented)
test_sums.c                  osums() dieharder overlapping test with more general parameters
test_ent.c                   ent_8_16() entropy for stream of 8- and 16-bit blocks
test_fft.c                   fftest() Fourier test as described in NIST 3.5
test_rank.c                  rank32x32(), rank6x8() adapted from dieharder
test_bitstream.c             bitstream_o() overlapped bitstream_n() nonoverlapped from diehard/dieharder
test_lz.c                    lz_split() Lempel-Ziv split, was in NIST and then excluded, but still ok for robust setting
test_birthday.c              birthdays() birthday test from diehard reimplemented according to the description
test_dist2d.c                mindist2d() minimal distance between 2d random points, uses c++ code from the next file
minimal_distance.hpp         c++ code from minimal distance test
test_spectral.c              spectral() spectral test uses c++ code from ../spectral_tests
test_ks1.c                   ksone() Kolmogorov-Smirnov one-sample test
test_monobit.c               monobit() checking the distribution of number of bits in bytes TODO: description
test_nist_block.c            nist_block() NIST block frequency test TODO: description

generators.h
generators.c
 A simple interface to call "generators", now limited to reading files (in different portions) and computing xor, but may be later adapted to other possible generators (synthetic, randomness amplification etc.) Needed to allow processing either the primary stream or xor of two streams in the same way. 

test-generators.c
test-generators.out
test-generators.c is a simple unit test; "make test-generators-run" compares its output with precomputed result test-generators.out 



 

File list:

00readme.txt             this file
doc                      directory for documentation

=== batch files with test batteries for different file length
rtest100m.sh
rtest10g.sh
rtest10m.sh
rtest1g.sh
rtest1m.sh



test_func.h              definitions for test functions structure
 


generators.c
generators.h
genone.bin.test100m
genone.bin.test10m
Makefile

reserve.c





















