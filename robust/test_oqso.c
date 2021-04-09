// OQSO test from Dieharder suite (that follows Diehard suite)

/*
 * OQSO means Overlapping-Quadruples-Sparse-Occupancy        
 * The test OQSO is similar [to opso], except that it considers 4-letter 
 * words from an alphabet of 32 letters, each letter determined  
 * by a designated string of 5 consecutive bits from the test    
 * file, elements of which are assumed 32-bit random integers.   
 * The mean number of missing words in a sequence of 2^21 four- 
 * letter words,  (2^21+3 "keystrokes"), is again 141909, with   
 * sigma = 295.  The mean is based on theory; sigma comes from   
 * extensive simulation.                                         
 *
 * Note: 2^21 = 2097152
 * Note: One must use overlapping samples to get the right sigma.
 * The tests BITSTREAM, OPSO, OQSO and DNA are all closely related.
 *
 * This test is now CORRECTED on the basis of a private communication
 * from Paul Leopardi (MCQMC-2008 presentation) and Noonan and Zeilberger
 * (1999), Rivals and Rahmann (2003), and Rahmann and Rivals (2003).
 * The "exact" target statistic (to many places) is:
 * \mu = 141909.6005321316,  \sigma = 294.6558723658
 * ========================================================================
 */




/* Original paper where opso test was described: G.Marsaglia, A.Zaman,
Monkey tests for random numbers, Computers. Math. Applic. 26(9),1-10 (1993) */

/* Test generates 32-10+1=28 values according to 28 possible positions 
   of 5-bit substrings in a 32-bit string. For each of 28 positions we
   get a word of n=2^21+3 letters (32-letter alphabet) 
   that correspond to n 32-bit integers read at the input. 
   We count words of length 4 that are not factors
   of this word, and convert this count according to the distribution
   function of (approximate) normal distribution.
*/   

/* Implementation notes: we read the input stream once and do not 
   store it; for each quadruple of the input 32-strings we look at all 28 
   possible positions and consider a quadruple of 5-strings that happens at
   each position and register the appearence of this quadruple. For that 
   we need 28 bits that can be packed into 4 bytes
*/
#define ALPHABET_SIZE 32
#define NUM_QUADS (32*32*32*32)
#define NUM_POSITIONS 28
#define BYTES_PER_PAIR 4 // upper rounding of NUM_POSITIONS/8
#define NINTS (1<<21)+3 // to get 2^21 factors of length 4
#define MEAN 141909.6005321316
#define SIGMA 294.6558723658

#include "test_func.h"
#include <gsl/gsl_math.h>

// transfrom coordinates in 32*32*32*32 array into 1-dimensional array
unsigned int linear(byte x, byte y, byte z, byte t){
  assert ((x>=0)&&(x<32)); assert ((y>=0)&&(y<32));
  assert ((z>=0)&&(z<32)); assert ((t>=0)&&(t<32));
  unsigned int xx= x;
  unsigned int yy= y;
  unsigned int zz= z;
  unsigned int tt= t;
  return ((((((xx<<5)|yy)<<5)|zz)<<5)|tt);
}

bool oqso (long double *value, unsigned long *hash, PRG gen,
           int *param, double *real_param, bool debug){
  assert (param[2]==28); // dimension is fixed to be 23
  assert (param[3]==NINTS); // number of read integers should be 2^{23}+1
  // the number of read integer could be changed easily if we do not use the distribution
  // but kept the same as in dieharder for now
 
  byte *bitset;
  bitset= (byte *) malloc(NUM_QUADS*BYTES_PER_PAIR);  
  if (bitset==NULL) return(false);
  // bitset is treated as a bit array: BYTES_PER_PAIR bytes 
  // starting at the position POS(x,y,z,t) are used
  // for registering pairs quadruples (x,y,z,t) taken from the different positions of 
  // consecutive 32-integer pairs.
  for (int i=0; i<NUM_QUADS*BYTES_PER_PAIR; i++){
    bitset[i]= (byte) 0;
  }  
  // initially bitset is empty

#define ADD_QUAD(x,y,z,t,pos) bitset[linear((x),(y),(z),(t))*BYTES_PER_PAIR+((pos)/8)]|=(byte)(1<<((pos)%8))  
  // adding pair (x,y) of 1024-letters at position pos \in [0,23]
#define IS_QUAD(x,y,z,t,pos) (bitset[linear((x),(y),(z),(t))*BYTES_PER_PAIR+((pos)/8)] & ((byte)(1<<((pos)%8))) != 0)
  // checking whether the correspodint pair exists for a given position pos
  
  unsigned int quad[4]; // for 4 consecutive 32-bit ints from the generator
  for (long i=0; i<NINTS; i++){
    for (byte j=0; j<3; j++){quad[j]= quad[j+1];} // left shift
    if (!g_int32_lsb(&(quad[3]),gen)){free(bitset);return(false);}
    // i+1 generator values are read
    if (i+1>=4){// quad is filled with read values
      // process quad[0..3] in all positions 
      unsigned int q0=quad[0];
      unsigned int q1=quad[1];
      unsigned int q2=quad[2];
      unsigned int q3=quad[3];
      for (int pos=0; pos<NUM_POSITIONS;pos++){
        ADD_QUAD(q0%32,q1%32,q2%32,q3%32,pos);
        q0/=2; q1/=2; q2/=2; q3/=2;
      }
      assert((q0<16)&&(q1<16)&&(q2<16)&&(q3<16)); // no more positions
    }
  }
  // count the free slots in tables for all positions
  long count[NUM_POSITIONS];
  for (int i=0; i<NUM_POSITIONS; i++){count[i]=0;}
  for (byte x=0; x<ALPHABET_SIZE; x++){
    for (byte y=0; y<ALPHABET_SIZE; y++){
      for (byte z=0; z<ALPHABET_SIZE; z++){
        for (byte t=0; t< ALPHABET_SIZE; t++){
          // sum counters for all positions and a given pair
          for (int pos=0; pos<NUM_POSITIONS; pos++){
            count[pos]+= IS_QUAD(x,y,z,t,pos);     
          }
        }
      }      
    }
  }
  // returning the values:
  for (int pos=0; pos<NUM_POSITIONS; pos++){
    double raw= NUM_QUADS-count[pos];
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

