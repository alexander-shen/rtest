#include "test_func.h"

/* Automatically generated bit-count table for 0-255 */
static const int number_of_ones[256] = {
    0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    1, 2, 2, 3, 2, 3, 3, 4, 2, 3, 3, 4, 3, 4, 4, 5,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    2, 3, 3, 4, 3, 4, 4, 5, 3, 4, 4, 5, 4, 5, 5, 6,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    3, 4, 4, 5, 4, 5, 5, 6, 4, 5, 5, 6, 5, 6, 6, 7,
    4, 5, 5, 6, 5, 6, 6, 7, 5, 6, 6, 7, 6, 7, 7, 8
};

// assuming unsigned has at least 4 bytes, using only lower 4 bytes
int count_bits (unsigned u){
    int cnt= 0;
    for (int i=0; i<4; i++){
        unsigned bt = u & ((unsigned) 255); 
        cnt += number_of_ones [bt];
        u >>= 8;
    }
    return(cnt);
}

bool monobit (long double *value, unsigned long *hash, PRG gen, 
                 int *param, double *real_param, bool debug){
  assert(param[2]==1); // dimension is 1
  long int n= param[3]; // number of 32 bit integers taked from gen
  assert(n>=1); 
  long int num_bits = 32*n;
  // now count 1s in 32*n bits, reading n 32-bit integers:
  if (debug){printf("Number of ints/bits to be tested: %ld/%ld\n", n, 32*n);}
  long i= 0; // number of integers read
  long count_ones= 0; // number of non-zero bits in the i integers read
  while (i!=n){
     unsigned next;
     if(!g_int32_lsb(&next, gen)){return(false);}
     // next 32 bits are obtained
     count_ones += count_bits(next);
     i++;
     if(debug){printf("%d ones in %u in binary\n",count_bits(next),next);}   
  }
  int disbalance = 2*count_ones - num_bits; 
  // = difference between number of ones and number of zeros  
  // = sum of n independent {-1,+1} variables, mean=0, variance=num_bits
  double normal_approx = (double) disbalance / sqrt ((double) num_bits);
  // normal_approx has distribution close to Normal(mean=0,variance=1)
  if(debug){printf("Normalized disbalance: %lf\n", normal_approx);}
  double abs_normal_approx=fabs(normal_approx); // fabs for double
  value[0]= 2.0* gsl_cdf_ugaussian_Q(abs_normal_approx);
  // return p-value, presumably uniformly distributed in [0,1]
  if (debug){printf("Presumably uniform p-value: %Lf\n", value[0]);}

  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}
