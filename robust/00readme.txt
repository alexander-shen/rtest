Packages used: 
  gmp (ubuntu package libgmp-dev) -- multiprecision arithmetic
  gsl (ubuntu package libgsl-dev) -- gnu statistical library

Files:

generators.h
generators.c
 A simple interface to call generators, now limited to reading files (in different portions) and computing xor, but may be later adapted to other possible generators (synthetic, randomness amplification etc.) 

test-generators.c
test-generators.out
test-generators.c is a simple unit test; "make test-generators-run" compares its output with precomputed result test-generators.out 

test_func.h
contains the forward definitions of all test functions (type test_func)
Each of them gets a PRG label, some parameters (in param[2..] and real_param[0..]), and a reference to array where the test results (long double values [], unsigned long hash[]) should be placed. (One call may produce several values, their number is called dimension of the test and is provided as param[2].) The hash values are some data to resolve ties if needed. For any test function one may compare (using Kolmogorov-Smirnov test) the samples obtained when this function is applied to test generator and etalon generator (see file rtest.c)

test_func.c
The list of all test functions (with short descriptions) and two example of how the test function could look like.

Adding a new test function requires: (1) adding the forward declaration in test_func.h; (2) adding the entry for this function in the list test_func.c; (3) creating a new .c file with the code of the function and adding in the Makefile variable TESTS_C

Main program: rtest.c 
