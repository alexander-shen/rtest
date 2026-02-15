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
static int count_bits (unsigned u){
    int cnt= 0;
    for (int i=0; i<4; i++){
        unsigned bt = u & ((unsigned) 255); 
        cnt += number_of_ones [bt];
        u >>= 8;
    }
    return(cnt);
}

bool nist_block (long double *value, unsigned long *hash, PRG gen, 
                 int *param, double *real_param, bool debug){
  assert(param[2]==1); // dimension should be 1
  long int n= param[3]; // number of blocks
  long int m= param[4]; // each block contains m 32-integers = 32m bits
  assert(n>=1);  assert(m>=1);
  if (debug){printf("%ld blocks, %ld bits each\n", n, 32*m);}
  long int num_bits = 32*m;
  double *freqs;
  freqs= (double *) malloc(n*sizeof(double)); // for frequencies in n blocks
  if (freqs==NULL){fprintf(stderr,"Not enough memory\n"); exit(1);}
  for (int i=0; i<n; i++){ // compute freqs[i]:
    long count_ones= 0;
    for (int j=0; j<m; j++){ // process m 32-bit unsigned integers:
     unsigned next;
     if(!g_int32_lsb(&next, gen)){return(false);}
     // next 32 bits are obtained
     count_ones += count_bits(next);
    } 
    // count_ones = total number of 1s in the block
    freqs[i]= ((double)count_ones)/(32.0 *((double)m));  
    //freqs[i] is computed
  } 
  // freqs[0..n) are computed 
  if(debug){
    printf("Frequencies in %ld blocks:", n);
    for (int i=0; i<n; i++){printf(" %lf", freqs[i]);}
    printf("\n");
  }
  long double chisquare= 0.0L;
  for (int i=0; i<n; i++){
    long double diff= (freqs[i]-0.5L);
    chisquare+= diff*diff; // later multiplied as required
  }
  chisquare*= 128.0L*((long double) m); // 4*block size*(sum of squares)
  value[0]= gsl_sf_gamma_inc_Q(((double)n)/2.0,((double)chisquare)/2.0);
  // return p-value, presumably uniformly distributed in [0,1]
  if (debug){printf("Presumably uniform p-value: %Lf\n", value[0]);}
  free(freqs);

  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}
