/* description from Dieharder test suite (taken from diehard); the dieharder code
 * does not match it but we follow the description, not the code
 * ========================================================================
 * This is the Diehard OPSO test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *         OPSO means Overlapping-Pairs-Sparse-Occupancy         ::
 * The OPSO test considers 2-letter words from an alphabet of    ::
 * 1024 letters.  Each letter is determined by a specified ten   ::
 * bits from a 32-bit integer in the sequence to be tested. OPSO ::
 * generates  2^21 (overlapping) 2-letter words  (from 2^21+1    ::
 * "keystrokes")  and counts the number of missing words---that  ::
 * is 2-letter words which do not appear in the entire sequence. ::
 * That count should be very close to normally distributed with  ::
 * mean 141,909, sigma 290. Thus (missingwrds-141909)/290 should ::
 * be a standard normal variable. The OPSO test takes 32 bits at ::
 * a time from the test file and uses a designated set of ten    ::
 * consecutive bits. It then restarts the file for the next de-  ::
 * signated 10 bits, and so on.                                  ::
 *
 * Note: Overlapping samples must be used to get the right sigma.
 * The tests BITSTREAM, OPSO, OQSO and DNA are all closely related.
 *
 * This test is now CORRECTED on the basis of a private communication
 * from Paul Leopardi (MCQMC-2008 presentation) and Noonan and Zeilberger
 * (1999), Rivals and Rahmann (2003), and Rahmann and Rivals (2003).
 * The "exact" target statistic (to many places) is:
 * \mu  = 141909.3299550069,  \sigma = 290.4622634038
 *========================================================================
 */

/* Original paper where opso test was described: G.Marsaglia, A.Zaman,
Monkey tests for random numbers, Computers. Math. Applic. 26(9),1-10 (1993) */

/* Test generates 32-10+1=23 values according to 23 possible positions 
   of 10-bit substrings in a 32-bit string. For each of 23 positions we
   get a word of n=2^21+1 letters  that correspond to n 32-bit integers
   read at the input. We count words of length 2 that are not factors
   of this word, and convert this count according to the distribution
   function of (approximate) normal distribution.
*/   

/* Implementation notes: we read the input stream once and do not 
   store it; for each pair of the input 32-strings we look at all 23 
   possible positions and consider a pair of 10-strings that happens at
   each position and register the appearence of this pair. For that 
   we need 23 bits that can be packed into 3 bytes
*/
#define ALPHABET_SIZE 1024
#define NUM_POSITIONS 23
#define BYTES_PER_PAIR 3 // upper rounding of NUM_POSITIONS/8
#define NINTS (1<<21)+1
#define MEAN 141909.3299550069
#define SIGMA 290.4622634038

#include "test_func.h"
#include <gsl/gsl_math.h>


bool opso (long double *value, unsigned long *hash, PRG gen,
           int *param, double *real_param, bool debug){
  assert (param[2]==23); // dimension is fixed to be 23
  assert (param[3]==NINTS); // number of read integers should be 2^{23}+1
  // the number of read integer could be changed easily if we do not use the distribution
  // but kept the same as in dieharder for now
 
  byt *bitset;
  bitset= (byt*) malloc(ALPHABET_SIZE*ALPHABET_SIZE*BYTES_PER_PAIR);
  if (bitset==NULL) return(false);
  // bitset is treated as a bit array: BYTES_PER_PAIR bytes 
  // starting at the position (ALPHABET_SIZE*x+y)*BYTES_PER_PAIR are used
  // for registering pairs (x,y) taken from the different positions of 
  // consecutive 32-integer pairs.
  for (int i=0; i<ALPHABET_SIZE*ALPHABET_SIZE*BYTES_PER_PAIR; i++){
    bitset[i]= (byt) 0;
  }  
  // initially bitset is empty

#define ADD_PAIR(x,y,pos) bitset[((x)*ALPHABET_SIZE+(y))*BYTES_PER_PAIR+((pos)/8)]|=(byt)(1<<((pos)%8))  
  // adding pair (x,y) of 1024-letters at position pos \in [0,23]
#define IS_PAIR(x,y,pos) ((bitset[((x)*ALPHABET_SIZE+(y))*BYTES_PER_PAIR+((pos)/8)]&((byt)(1<<((pos)%8))))!=0)
  // checking whether the correspodint pair exists for a given position pos
#define BOUND_CHECK(x,y,pos) ( (0<=(x))&&((x)<ALPHABET_SIZE)&&(0<=(y))&&((y)<ALPHABET_SIZE)&&(0<=(pos))&&((pos)<NUM_POSITIONS) )  
  
  unsigned int x0,y0; // for two consecutive 32-bit ints from the generator
  if (!g_int32_lsb(&y0,gen)){free(bitset);return(false);}
  for (long i=0; i<NINTS-1;i++){
    x0= y0;
    if (!g_int32_lsb(&y0,gen)){free(bitset);return(false);}
    // process pair (x0,y0) at all the positions:
    unsigned int x=x0; unsigned int y=y0;
    for (int pos=0; pos<NUM_POSITIONS; pos++){
      BOUND_CHECK(x%1024,y%1024,pos);
      ADD_PAIR(x%1024,y%1024,pos);
      x/=2; y/=2;
    }
    assert ((x<512)&&(y<512)); // no more positions
  }
  // count the free slots in tables for all positions
  long count[NUM_POSITIONS];
  for (int i=0; i<NUM_POSITIONS; i++){count[i]=0;}
  for (int x=0; x<ALPHABET_SIZE; x++){
    for (int y=0; y<ALPHABET_SIZE; y++){
      // sum counters for all positions and a given pair
      for (int pos=0; pos<NUM_POSITIONS; pos++){
        BOUND_CHECK(x,y,pos);
        count[pos]+= IS_PAIR(x,y,pos);     
      }  
    }
  }
  // returning the values
  for (int pos=0; pos<NUM_POSITIONS; pos++){
    double raw= ALPHABET_SIZE*ALPHABET_SIZE-count[pos];
    double normalized= gsl_cdf_ugaussian_P((raw-MEAN)/SIGMA);
    // normalization not needed for 2-sample KS, but does not hurt and is
    // useful for graphical representation and comparison to dieharder
    value[pos]= (long double) normalized;
  }  
  // use eight more bytes for hash
  unsigned int h1, h2;
    if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){free(bitset);return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  if(debug){
    printf("Normalized values:\n");
    for (int pos=0; pos<NUM_POSITIONS; pos++){
      printf("%10.5Lf ", value[pos]);
      if ((pos+1)%5==0){printf("\n");}
    }
    printf("\n");
  }
  free(bitset); 
  return(true);
}

