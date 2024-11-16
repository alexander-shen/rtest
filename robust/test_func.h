#include "generators.h"
#include <gsl/gsl_math.h>
#include <gsl/gsl_cdf.h>
#include <gsl/gsl_randist.h>
#include <gsl/gsl_sf.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_fft_complex.h>
#include <gsl/gsl_sort_uint.h>
#include "../kolmogorov-smirnov/k1smirnov.h"

typedef bool test_func(long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//This is the type definition for possible test functions 
// Each of them determines a robust test 
// returns OK if no errors; *value= function value; 
// *hash= additional hash used in case of ties; 
// gen is the generator to be called;
// param[2...] are integer parameters determining the behavior
// of the function. Note that param[0] and param[1] are reserved 
// for the sample size and should not be used
// dimension=param[2] is the dimension of the output (input parameter)
// param[3] is the sample size in some sense (if applicable)
// param[4..] have test-dependent meaning
// the computed values are placed in value[0]..value[dimension-1]
// debug causes printing additional information

typedef struct test_function
{
  test_func *reference;
  const char* description;
} test_function;

// Here the forward definitions of all test functions should be provided


bool all_bytes (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);       
//0 dimension 1 size=auto [coupon collector time for bytes]

bool all_16 (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);          
//1 dimension 1 size=auto [coupon collector time for 16 ints]

bool dummy_dimension(long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);  //2
// just for testing

bool sts_serial (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);      
//3 dimension = a multiple of 3, at most 93 size number of ints processed [sts_serial.c]

bool opso (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);  
//4 dimension = 23, size= 2097153 [diehard_opso.c]

bool oqso (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);  
//5 dimension = 28, size= 2097155 [diehard_oqso.c]

bool bytedistrib(long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);      
//6 dimension = 1, uses 3*dize nints [DAB Byte Distribution Test, dab_bytedistrib.c]

bool knuth_runs(long double * value, unsigned long * hash, PRG gen, int * param, double *real_param, bool debug);    
//7 dimension = 2, uses size nints (>=10000 recommended) [diehard_runs.c]

bool osums (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);           
//8 dimension = 1, uses 2*size-1 nints [diehard_sums.c, rejected as incorrect by dieharder]

bool ent_8_16 (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//9 dimension = 4 (ent8, chi8, ent16, chi16), uses size nints [ent ubuntu utility]

bool fftest (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//10 dimension = 1 (number of peaks converted assuming normal approximation) [NIST FFT test]

bool rank32x32 (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//11 dimension =1 (chi-square value for the rank distribution of param[3] matrices 32*32 from 32*param[3] ints

bool rank6x8 (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//12 dimension =25 (chi-square value for the rank distribution of 6*8 matrices obtained 
//   in 25 positions from 6n random ints (25*param[3] matrices from 6*param[3] ints)

bool bitstream_o (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//13 overlapped bitstream test; dimension =1, param[3] should be 2^21+19 = 2097171

bool bitstream_n (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//14 non-overlapped bitstream test; dimension =1, param[3] should be 20*2^21 = 41943040

bool lz_split (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//15 dimension = 1; recommended param[3]=31250; in this case conversion to uniform distibution happens

bool birthdays (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//16 diehard/dieharder birthday test

bool mindist2d (long double *value, unsigned long *hash, PRG gen, int *param, double *real_param, bool debug);
//17 minimal distance in 2d test

