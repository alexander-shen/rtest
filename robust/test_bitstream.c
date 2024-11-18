#include "test_func.h"
// Bitstream test from diehard and dieharder
//
// Diehard: 
/*
                  THE BITSTREAM TEST                         
The file under test is viewed as a stream of bits. Call them
b1,b2,... .  Consider an alphabet with two "letters", 0 and 1
and think of the stream of bits as a succession of 20-letter
"words", overlapping.  Thus the first word is b1b2...b20, the
second is b2b3...b21, and so on.  The bitstream test counts  
the number of missing 20-letter (20-bit) words in a string of
2^21 overlapping 20-letter words.  There are 2^20 possible 20
letter words.  For a truly random string of 2^21+19 bits, the
number of missing words j should be (very close to) normally 
distributed with mean 141,909 and sigma 428.  Thus           
 (j-141909)/428 should be a standard normal variate (z score)
that leads to a uniform [0,1) p value.  The test is repeated 
twenty times.                                                
*/

// Dieharder adds a non-overlapping version of this test:
/*
The test is repeated 100 times by default in dieharder, but the
size of the sample is fixed (tsamples cannot/should not be
varied from the default).  The sigma of this test REQUIRES the
use of overlapping samples, and overlapping samples are not
independent.  If one uses the non-overlapping version of this
test, sigma = 290 is used instead, smaller because now there
are 2^21 INDEPENDENT samples.
*/

// Since the overlapped and non-overlapped versions use different 
// number of input integers, we make two separate functions

bool bitstream_o (long double *value, unsigned long *hash, PRG gen, 
                  int *param, double *real_param, bool debug){
  assert (param[2]==1); 
  assert (param[3]==(1<<21)+19);
  unsigned int word= 0; // only 20 last bits are used
  unsigned int mask= (((unsigned int)1)<<20)-1;
  bool *bitset; // array of size 2^20 allocated for counters
  bitset = (bool *) malloc((1<<20)*sizeof(bool));
  if (bitset==NULL){return(false);}
  // bit array 
  if (debug){printf("bitset allocated\n");}
  for (int i=0; i<(1<<20);i++){
    bitset[i]=false;
  } 
  if (debug){printf("bitset initialized\n");}
  // bitset is empty 
  for (int i=0; i<20; i++){
    bool b;
    if (!g_getbit(&b, gen)){free(bitset); return(false);}
    word=(word<<1)|b;   
  }
  assert (word<(1<<20));
  bitset[word]= true;
  if (debug){printf("first word registered\n");}
  int reg= 1; // number of registered words
  while (reg!=(1<<21)){
    // get and register next word
    bool b;
    if (!g_getbit(&b, gen)){free(bitset); return(false);}
    word=((word<<1)|b)&mask;
    assert (word<(1<<20));
    bitset[word]= true;
    reg++;
  }
  if (debug){printf("bitset filled\n");}
  // 2^21 20-bit words registered
  int missing= 0;
  for (int i=0; i<(1<<20); i++){
    if (!bitset[i]){missing++;}
  }
  if(debug) {printf("There are %d missing 20-bit words among 2^21 overlapping factors\n",missing);}
  double mean= 141909; 
  double sigma= 428;
  if(debug) {printf("Theoretical mean: %lf, sigma: %lf\n", mean, sigma);}
  value[0]=gsl_cdf_ugaussian_P((((double)missing)-mean)/sigma);
  if(debug) {printf("Normalized: %Lf\n", value[0]);}
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){free(bitset);return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  free(bitset); 
  return(true);
}

#define WLEN 20 // should not be changed (and should be less than 32 if changed)

bool bitstream_n (long double *value, unsigned long *hash, PRG gen,
                  int *param, double *real_param, bool debug){
  assert (param[2]==1); 
  assert (param[3]==(1<<21)*20);
  bool *bitset; // array of size 2^20 allocated for counters
  bitset = (bool *) malloc((1<<20)*sizeof(bool));
  if (bitset==NULL){return(false);}
  // bit array 
  if (debug){printf("bitset allocated\n");}
  for (int i=0; i<(1<<20);i++){
    bitset[i]=false;
  } 
  if (debug){printf("bitset initialized\n");}
  // bitset is empty 
  int reg= 0; // number of registered words
  while (reg!=(1<<21)){
    // get and register next word
    unsigned word;
    if (!g_getword(&word,WLEN,gen)){free(bitset); return (false);}
    bitset[word]= true;
    reg++;
  }
  if (debug){printf("bitset filled\n");}
  // 2^21 20-bit words registered
  int missing= 0;
  for (int i=0; i<(1<<20); i++){
    if (!bitset[i]){missing++;}
  }
  if(debug) {printf("There are %d missing 20-bit words among 2^21 overlapping factors\n",missing);}
  double mean= 141909; 
  double sigma= 290;
  if(debug) {printf("Theoretical mean: %lf, sigma: %lf\n", mean, sigma);}
  value[0]=gsl_cdf_ugaussian_P((((double)missing)-mean)/sigma);
  if(debug) {printf("Normalized: %Lf\n", value[0]);}
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){free(bitset);return(false);}
  *hash = (((unsigned long) (unsigned)h2)<<32)+((unsigned long)(unsigned)h1);
  if (debug) {print64(*hash); printf("\n");}
  free(bitset); 
  return(true);
}
