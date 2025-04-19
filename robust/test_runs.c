#include "test_func.h"

// original description from dieharder (that starts with description from 
// diehard)
/*
 *========================================================================
 * This is the Diehard Runs test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 * Here is the test description from diehard_tests.txt:
 *
 * This is the RUNS test. It counts runs up, and runs down,in a sequence
 * of uniform [0,1) variables, obtained by floating the 32-bit integers
 * in the specified file. This example shows how runs are counted:
 *  .123, .357, .789, .425,. 224, .416, .95
 * contains an up-run of length 3, a down-run of length 2 and an up-run
 * of (at least) 2, depending on the next values.  The covariance matrices
 * for the runs-up and runs-down are well-known, leading to chisquare tests
 * for quadratic forms in the weak inverses of the covariance matrices.
 * Runs are counted for sequences of length 10,000.  This is done ten times,
 * then repeated.
 *
 *                            Comment
 *
 * I modify this the following ways. First, I let the sequence length be
 * the variable -t (vector length) instead of fixing it at 10,000.  This
 * lets one test sequences that are much longer (entirely possible with
 * a modern CPU even for a fairly slow RNG).  Second, I repeat this for
 * the variable -s (samples) times, default 100 and not just 10.  Third,
 * because RNG's often have "bad seeds" for which they misbehave, the
 * individual sequences can be optionally -i reseeded for each sample.
 * Because this CAN let bad behavior be averaged out to where
 * it isn't apparent for many samples with few bad seeds, we may need to
 * plot the actual distribution of p-values for this and other tests where
 * this option is used.  Fourth, it is silly to convert integers into floats
 * in order to do this test.  Up sequences in integers are down sequences in
 * floats once one divides by the largest integer available to the generator,
 * period. Integer arithmetic is much faster than float AND one skips the
 * very costly division associated with conversion.
 * *========================================================================
 */
 
 // The original description is given on p. 65 of Knuth Art of Programming
 // (1982 edition). The dieharder implementation deviates from it since
 // it considers the first element in a rather strange way. Reimplemented.
 
 // For a sequence of length n we count the number of parts in its minimal
 // splitting into non-decreasing subsequences (runs). Then we consider
 // 6-vector consisting of numbers x_1,..,x_6 of runs of lengths 1,2,
 // 3, 4, 5, 6-infty.
 // Then let V=1/n \sum_{1\le i,j\le 6} (x_i-nb_i)(x_j-nb_j)a_{ij},
 // where a and b are given by tables:
 
static double a[6][6] = {
 { 4529.4,   9044.9,  13568.0,   18091.0,   22615.0,   27892.0},
 { 9044.9,  18097.0,  27139.0,   36187.0,   45234.0,   55789.0},
 {13568.0,  27139.0,  40721.0,   54281.0,   67852.0,   83685.0},
 {18091.0,  36187.0,  54281.0,   72414.0,   90470.0,  111580.0},
 {22615.0,  45234.0,  67852.0,   90470.0,  113262.0,  139476.0},
 {27892.0,  55789.0,  83685.0,  111580.0,  139476.0,  172860.0}
};

static double b[6] = {
 1.0/6.0,
 5.0/24.0,
 11.0/120.0,
 19.0/720.0,
 29.0/5040.0,
 1.0/840.0,};
  

// taken from Knuth (section 3.3.2, p. 65) and dieharder code (they coincide)

#define RUNMAX 6

// Note that the matrices are computed by Knuth for the case where 6,7,... runs
// are counted together.
//
// Then V is normalized wrt chi-square distribution with 6 degrees of freedom

#define NORMALIZE(v) gsl_sf_gamma_inc_Q(3.0,(v)/2.0);

// The same is done for nonincreasing sequences, so the test returns two values

bool knuth_runs (long double *value, unsigned long *hash, PRG gen, 
                 int *param, double *real_param, bool debug){
  assert(param[2]==2);
  long int n= param[3];
  assert(n>=1); // n should be reasonably large, diehard used n=10000   
                // for smaller n the approximation used is not sufficient
  if (debug){printf("Number of ints to be tested: %ld\n", n);}

  long up_runs[RUNMAX], down_runs[RUNMAX]; // note the index shift: runs[i]=#runs of length (i+1)
  for (int i=0; i<RUNMAX; i++){up_runs[i]= 0; down_runs[i]= 0;}
  long i=1;
  long up_run= 1;
  long down_run= 1;
  int last;
  if(!g_int32_lsb((unsigned int*) &last, gen)){return(false);}
  if(debug){printf("%d ",last);}
  // up_run and down_run are the length of the last up/down run
  // that includes the ith element
  // all runs except the last one are taken into account
  while (i!=n){
    int next; 
    if(!g_int32_lsb((unsigned int*) &next, gen)){return(false);}
    if(debug){printf("%d ",next);}   
    if (next>last){
      up_run++;
      down_runs[((down_run<RUNMAX)?(down_run-1):RUNMAX-1)]++;
      down_run= 1;      
    }else if(next<last){
      down_run++;
      up_runs[((up_run<RUNMAX)?(up_run-1):RUNMAX-1)]++;
      up_run= 1;
    }else{
      assert(next==last);
      up_run++; 
      down_run++;
    }
    last= next;    
    i++;
  }
  // add the last run into account:
  down_runs[((down_run<RUNMAX)?(down_run-1):RUNMAX-1)]++;
  up_runs[((up_run<RUNMAX)?(up_run-1):RUNMAX-1)]++;
  double up_centered[RUNMAX]; 
  double down_centered[RUNMAX];
  for (int i=0; i<RUNMAX; i++){
    up_centered[i]= up_runs[i]-((double) n)*b[i];
    down_centered[i]= down_runs[i]- ((double) n)*b[i];
  }
  double vup=0;
  double vdown=0;
  for (int i=0; i<RUNMAX; i++){
    for (int j=0; j<RUNMAX; j++){
     vup+= a[i][j]*up_centered[i]*up_centered[j];
     vdown+= a[i][j]*down_centered[i]*down_centered[j];
    }
  }
  if (debug){
    printf("Number of up-runs of lengths 1,2,3,4,5,6+:\n");
    for (int i=0; i<RUNMAX; i++){
      printf ("%6ld ", up_runs[i]);
    }
    printf ("\n");
    printf("Number of down-runs of lengths 1,2,3,4,5,6+:\n");
    for (int i=0; i<RUNMAX; i++){
      printf ("%6ld ", down_runs[i]);
    }
    printf ("\n");
  }
  value[0]= NORMALIZE(vup/n);
  value[1]= NORMALIZE(vdown/n);
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}

