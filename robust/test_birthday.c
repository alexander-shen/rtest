// Birthday test described by Marsaglia in diehard and 
// transferred to dieharder. Here are Marsaglia' description
// and Brown's comments
 /* =======================================================================
 * This is the Diehard Birthdays test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *               This is the BIRTHDAY SPACINGS TEST        
 * Choose m birthdays in a year of n days.  List the spacings
 * between the birthdays.  If j is the number of values that 
 * occur more than once in that list, then j is asymptotically
 * Poisson distributed with mean m^3/(4n).  Experience shows n
 * must be quite large, say n>=2^18, for comparing the results
 * to the Poisson distribution with that mean.  This test uses
 * n=2^24 and m=2^9 [Marsaglia uses m=2^10 both in the comments
   and in the code - AS],  
                      so that the underlying distribution for j
 * is taken to be Poisson with lambda=2^27/(2^26)=2. [Marsaglia
   speaks about 2^30/2^{26}=16 - AS]
                                                       A sample
 * of 500 [Marsaglia speaks about 200 in the comments and in the
   message but uses 500 in the code - AS] 
          j's is taken, and a chi-square goodness of fit test
 * provides a p value.  The first test uses bits 1-24 (counting
 * from the left) from integers in the specified file.       
 *   Then the file is closed and reopened. Next, bits 2-25 are
 * used to provide birthdays, then 3-26 and so on to bits 9-32.
 * Each set of bits provides a p-value, and the nine p-values
 * provide a sample for a KSTEST.
 *
 *                            Comment
 *
 * I'm modifying this test several ways.  First, I'm running it on all 32
 * 24 bit strings implicit in the variables.  We'll do this by rotating
 * each variable by one bit position in between a simple run of the test.
 * A full run will therefore be 32 simple (rotated) runs on bits 1-24, and
 * we can do -p psamples runs to get a final set of p-values to evaluate.
 *========================================================================
 */
 
// The original diehard C code and dieharder implementation differ
// significantly. Both do not consider the intervals that cross the year's
// boundary (dieharder considers new year start as a starting point
// for an interval, but not as an endpoint). More significant difference:
// if there are three equal intervals, should we count them as two
// coincidences (diehard) or one (dieharder). Still the parameters
// of the Poisson disstibution used for comparison are the same.
// Another strange thing: dieharder implementation
// speaks about 32 24-bit words extracted from each 32-bit integer, but
// it is not done in the code: the variable irun that can be used
// does not appear in the code.
// 
// In our code we tried to follow the description of the test.
// It has dimension 32 (number of possible positions of bit strings),
// so param[2] should be 32
// It repeats param[3] times the computation of the number of repeated
// intervals (in diehard or dieharder sence, see above, depending on the
// definition of COUNT_UNIQUE) for all the positions 
// and gets 32 empirical distributions; for each of them we compute
// chi-square deviation from the presumed distribution and get 32 p-values
// in that way

#include "test_func.h"

#define m 10
#define M 1024 // 2^m, the number of "birthdays" 
#define n 24  // 2^n is the length of the year, so we use 24-bits cyclic
              // factors of 32 bit integers as "birthdays"
# define N 16777216 // = 2^n = 1<<24

// 0<=s<32; return n last bits of 32-bit unsigned int u 
// cyclically shifted s positions to the right
unsigned int cshift(unsigned int u, unsigned int s){
  const unsigned int mask= (1<<n)-1; // n last bits are 1
  assert ((0<=s)&&(s<32));
  if (s==0) {return(u&mask);} // since 32-bit shift does not work
  // 0<s<32
  return ( ((u>>s)|(u<<(32-s)))&mask );
}

//#define COUNT_UNIQUE // as it is done in dieharder (but not diehard c code in die.c)

bool birthdays (long double *value, unsigned long *hash, PRG gen, 
                int *param, double *real_param, bool debug){
  assert(param[2]==32);
  long slen= param[3]; // sample size, was 500 in the original test
  // The recommended rule of thumb for chi-square testing is to combine small probabilities
  // until all the expectations are at least 5. This should be done at both ends;
  // dieharder does this only for large values, and counted separately all the bins 
  // where the assumed expected values exceed 5,
  // and one more, and ignores the others (strangely)
  assert (slen>50); // small slen can create problems with thresholds
  double lambda=((double)M)*((double)M)*((double)M)/(4.0*((double)N)); // M^2/4N
  int kmin= 0;
  double sump= gsl_ran_poisson_pdf(0,lambda);
  // sump= combined probabilities for 0..kmin
  while (sump*((double)slen)<5.0){
    kmin++;
    sump+= gsl_ran_poisson_pdf(kmin,lambda);
  }
  // kmin is minimal value such that the expected value of hits in 0..kmin is >=5
  double ltail = sump; // total probability of the left tail combined
  int kmax= M; // greater values are ignored since the probability should be negligible
  sump= gsl_ran_poisson_pdf(kmax,lambda);
  // sump = combined probabilities for kmax..M
  while (sump*((double)slen)<5.0){
    kmax--;
    sump+= gsl_ran_poisson_pdf(kmax,lambda);
  }
  // kmax is minimal such that the expected value of hits in kmax..M is >=5 
  int ndf=kmax-kmin; // number of degrees of freedom for chi^2 test (=#values-1)
  double rtail = sump; // total probability of the right tail combined
  assert (kmin<kmax); // kmin=kmax makes the test useless
  if (debug){
    printf("kmin= %d, kmax= %d\n", kmin, kmax);
  }
  int distrib[32][kmax+1]; // how many times given shift produces given number of repetitions
                           // values smaller than kmin are reserved but not used
  for (int sft=0; sft<32; sft++){
    for (int k=0; k<=kmax; k++){
      distrib[sft][k]= 0;
    }
  } // distrib initialized with zeros
  for (int trial=0; trial<slen; trial++){//read M 32-bit integers and update distrib[][]
    unsigned int bd[32][M]; // arrays for birthdays
    int bd_difference[32][M]; // differences between birthdays
    for (int numbd=0; numbd<M; numbd++){
      unsigned int next;
      if (!g_int32_lsb(&next,gen)){return(false);}
      // take 32 cyclic factors of next and put them in bd[][numbd]
      for (int sft=0; sft<32; sft++){
        assert (cshift(next,sft)<N);
        bd[sft][numbd]=cshift(next,sft);
      }
    }
    // bd[shift][number of birthday] filled for shift in 0..31, number in 0..N-1

    for (int sft=0; sft<32;sft++){ // for each shift sft
      // compute the intervals and number of repetitions 
      if (debug){
        printf("Birthdays:\n");
        for (int i=0; i<M; i++){
          printf("%10u\n", bd[sft][i]);
        }      
      }
      gsl_sort_uint(bd[sft],1,M);// stride 1, consecutive integers; gsl_sort_uint.h
      if (debug){
        printf("Sorted birthdays:\n");
        for (int i=0; i<M; i++){
          printf("%10u\n", bd[sft][i]);
        }      
      }
      for (int i=0; i<M-1; i++){
        bd_difference[sft][i]= (int) bd[sft][i+1]- (int) bd[sft][i]; 
        // all intervals computed except for the last one
      }
      bd_difference[sft][M-1]= (bd[sft][0]+N)-bd[sft][M-1]; // last interval uses first bd of the next year
      // all intervals are computed, sum of intervals is N
      int sumdiff=0;
      for (int i=0; i<M; i++){
        assert (bd_difference[sft][i]>=0);
        sumdiff+= bd_difference[sft][i];
      }
      assert (sumdiff==N);
      if (debug){
        printf("Intervals:\n");
        for (int i=0; i<M; i++){
          printf("%10d\n", bd_difference[sft][i]);
        }      
      }
      gsl_sort_int(bd_difference[sft],1,M);// stride 1, consecutive integers; gsl_sort_int.h 
      // sorting again (now intervals, they are signed int even if non-negative)
      // now we have to count all the repetitions
      if (debug){
        printf("Sorted intervals:\n");
        for (int i=0; i<M; i++){
          printf("%10d\n", bd_difference[sft][i]);
        }      
      }
      int last= 1; 
      int nrepet= 0;
      bool last_repeated= false;
      // we have nrepet repetitions in bd[sft][0..last)
      // last_repeated= false // = (last value appeared more than once)
      while (last!=M){ 
        if (bd_difference[sft][last]==bd_difference[sft][last-1]){
#ifdef COUNT_UNIQUE      
          if (!last_repeated){nrepet++;}
#else
          nrepet++;  
#endif
        }
        last_repeated= (bd_difference[sft][last]==bd_difference[sft][last-1]); 
        last++;
      }
      // nrepet = number of repetitions for position sft
      if (debug){
        printf("nrepet=%d\n", nrepet);
      }  
      if (nrepet>kmax){nrepet= kmax;}
      if (nrepet<kmin){nrepet= kmin;}
      distrib[sft][nrepet]++;
      if (debug){
        printf("trial= %d, nrepet=%d, distrib[%d][%d]=%d\n", trial, nrepet, sft, nrepet, distrib[sft][nrepet]);
      }  
    } //endfor (int sft=0; sft<32;sft++)
    // numbers of repetitions (reduced to kmin..kmax) for each shift are registered in distrib[shift][] 
  } // endfor (int trial=0; trial<slen; trial++)
  // counters distrib[sft][k] are computed
  double expected[kmax+1];
  for (int sft=0; sft<32; sft++){
    for (int k=kmin; k<=kmax; k++){
      double p= gsl_ran_poisson_pdf(k,lambda);
      if (k==kmin) {p= ltail;}
      if (k==kmax) {p= rtail;}
      expected[k]= slen*p;
    }
  }
  // expected values for combined bins (k<kmin unused!)
  // compute p-values for each position using distrib[][]:
  for (int sft=0; sft<32; sft++){
    double chisquare= 0.0;
    for (int k=kmin; k<=kmax; k++){
       double deviation=distrib[sft][k]-expected[k];
       chisquare+= deviation*deviation/expected[k];
    }
    value[sft]= gsl_sf_gamma_inc_Q(ndf/2.0, chisquare/2.0);
  }
  // value[0..31] filled
  if(debug){
    for (int sft=0; sft<32; sft++){
      printf("Distribution for shift %d:\n", sft);
      for (int k=kmin; k<=kmax; k++){
        printf(" %3d [%4.1lf]", distrib[sft][k], expected[k]);
      }
      printf("\n");
    }
  }  
  
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}              
