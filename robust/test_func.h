#include "generators.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_sort_uint.h>
#include <gsl/gsl_sort_int.h>
#include "../kolmogorov-smirnov/k1smirnov.h"

typedef bool test_func(long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//This is the type definition for possible test functions 
// Each of them determines a robust test 
// returns true if no errors; in this case 
// value[0..]= function values; 
// hash[0..]= additional hash values used in case of ties; 
// gen is the generator to be called;
// param[2...] are integer parameters determining the behavior
// of the function. Note that param[0] and param[1] are reserved 
// for the sample size and should not be used
// dimension=param[2] is the dimension of the output (input parameter
// that determines how many position is value[] and hash[] are filled) 
// param[3] is the amount of used data in some test-depending sense (if applicable)
// param[4..] have test-dependent meaning
// real_param[0..] also have test-dependent meaning
// debug causes printing additional information

typedef struct test_function
{
  test_func *reference;
  const char* description; // used for short messages
} test_function;

// Here the forward definitions of all test functions should be provided

test_func all_bytes;       
//0 dimension 1 size=auto [coupon collector time for bytes]

test_func all_16;          
//1 dimension 1 size=auto [coupon collector time for 16 ints]

test_func dummy_dimension;  
//2
// just for testing

test_func sts_serial;      
//3 dimension = a multiple of 3, at most 93 size number of ints processed [sts_serial.c]

test_func opso;  
//4 dimension = 23, size= 2097153 [diehard_opso.c]

test_func oqso;  
//5 dimension = 28, size= 2097155 [diehard_oqso.c]

test_func bytedistrib;      
//6 dimension = 1, uses 3*dize nints [DAB Byte Distribution Test, dab_bytedistrib.c]

test_func knuth_runs;    
//7 dimension = 2, uses size nints (>=10000 recommended) [diehard_runs.c]

test_func osums;           
//8 dimension = 1, uses 2*size-1 nints [diehard_sums.c, rejected as incorrect by dieharder]

test_func ent_8_16;
//9 dimension = 4 (ent8, chi8, ent16, chi16), uses size nints [ent ubuntu utility]

test_func fftest;
//10 dimension = 1 (number of peaks converted assuming normal approximation) [NIST FFT test]

test_func rank32x32;
//11 dimension =1 (chi-square value for the rank distribution of param[3] matrices 32*32 from 32*param[3] ints

test_func rank6x8;
//12 dimension =25 (chi-square value for the rank distribution of 6*8 matrices obtained 
//   in 25 positions from 6n random ints (25*param[3] matrices from 6*param[3] ints)

test_func bitstream_o;
//13 overlapped bitstream test; dimension =1, param[3] should be 2^21+19 = 2097171

test_func bitstream_n;
//14 non-overlapped bitstream test; dimension =1, param[3] should be 20*2^21 = 41943040

test_func lz_split;
//15 dimension = 1; recommended param[3]=31250; in this case conversion to uniform distibution happens

test_func birthdays;
//16 diehard/dieharder birthday test

test_func mindist2d;
//17 minimal distance in 2d test
