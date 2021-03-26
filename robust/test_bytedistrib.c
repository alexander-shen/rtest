// Reimplementing dab_bytedistrib.c described in dieharder as follows
/*
 *========================================================================
 *                        DAB Byte Distribution Test
 *
 * Extract n independent bytes from each of k consecutive words. Increment
 * indexed counters in each of n tables.  (Total of 256 * n counters.)
 * Currently, n=3 and is fixed at compile time.
 * If n>=2, then the lowest and highest bytes will be used, along
 * with n-2 bytes from the middle.
 * If the generator's word size is too small, overlapped bytes will
 * be used.
 * Current, k=3 and is fixed at compile time.
 * Use a basic chisq fitting test (chisq_pearson) for the p-value.
 * Previous version also used a chisq independence test (chisq2d); it
 * was found to be slightly less sensitive.
 * I envisioned this test as using a small number of samples and large
 * number of separate tests. Experiments so far show that keeping -p 1
 * and increasing -t performs best.
 *========================================================================
 */
 
/* So the following is happening: for every three 32-bit numbers from 
  the generator (dieharder allows also generators of different int size,
  but we assume the size is 32) one extracts 9 bytes (three from each integer).
  This is repeated n=param[3] times, so we get 9 distributions on {0,1}^8
  corresponding to 9 different byte positions. Then we apply chi-square 
  test to the combined distribution on 9*256 objects that has ndf=9*255
  degrees of freedom (since we have 255 degrees of freedom for each 
  position). One resulting value is returned (param[2] should be 1) 
  
  More precisely, each of 256*9 counters has expected value e=n/256 and
  actual value x; we compute the sum S of (x-e)^2/e for all counters
  and then compute the approximate p-value as
     gsl_sf_gamma_inc_Q (ndf/2, S/2)
*/
 
 
 
#include "test_func.h"
#include <gsl/gsl_math.h>

#define BYTES_PER_INT 3
#define INTS_PER_STEP 3
#define BYTES_PER_STEP 9 // = BYTES_PER_INT*INTS_PER_STEP
#define NDF (255*9) // number of degrees of freedom

void extract_bytes(unsigned int i, byte *b1, byte *b2, byte *b3){
  *b1= i%256;
  *b2= (i>>12)%256;
  *b3= i>>24;   
}


bool bytedistrib(long double *value, unsigned long *hash, PRG gen, int *param, bool debug){

  assert(param[2]==1); // test returns one value
  unsigned long n= param[3]; // number of trials; test uses 3*n 32-bit integers
  assert (n>0);
  // prepare and initialize the counters array:   
  unsigned int count[BYTES_PER_STEP][256]; 
  for (int i=0; i<BYTES_PER_STEP; i++){
    for (int j=0;j<256; j++){
      count[i][j]= 0;
    }
  }
  // process the input stream:
  for (long i=0; i<n; i++){
    // get three random 32-integers, convert them to 9 bytes and 
    // increment the corresponding counters:
    for (int numint=0; numint < INTS_PER_STEP; numint++){
      // read an int from the generator:
      unsigned int curr;
      if (!g_int32_lsb(&curr,gen)){return(false);}
      // extract three bytes from the int:
      byte b1, b2, b3;
      extract_bytes(curr, &b1,&b2,&b3);      
      // increase the corresponding counters:
      count[numint*BYTES_PER_INT][b1]++;
      count[numint*BYTES_PER_INT+1][b2]++;
      count[numint*BYTES_PER_INT+2][b3]++;
    }
  }

  // now array is filled; it remains to compute and normalize the sum s
  long double s= 0.0; // the sum in chi-square test
  long double expectation = ((long double)n)/256.0;
  for (int i=0; i<BYTES_PER_STEP; i++){
    for (int j=0;j<256; j++){
      long double deviation= ((long double) count[i][j])-expectation;
      s+= deviation*deviation/expectation;
    }
  }
  *value=(long double) gsl_sf_gamma_inc_Q (((double)NDF)/2.0, s/2.0);
  if (debug){
    printf("%10.8Lf\n",*value);
  }
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}
