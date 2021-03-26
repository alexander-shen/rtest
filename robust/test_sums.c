#include "test_func.h"

// The overlapping sums test in Diehard (and moved to Dieharder with
// some changes --- not clear whether the changes are correct, see below)

// Here is the description from dieharder (including the original one from diehard)

/*
 *========================================================================
 * This is the Diehard sums test, rewritten from the description
 * in tests.txt on George Marsaglia's diehard site.
 *
 *             The  OVERLAPPING SUMS test                        ::
 * Integers are floated to get a sequence U(1),U(2),... of uni-  ::
 * form [0,1) variables.  Then overlapping sums,                 ::
 *   S(1)=U(1)+...+U(100), S(2)=U(2)+...+U(101),... are formed.  ::
 * The S's are virtually normal with a certain covariance mat-   ::
 * rix.  A linear transformation of the S's converts them to a   ::
 * sequence of independent standard normals, which are converted ::
 * to uniform variables for a KSTEST. The  p-values from ten     ::
 * KSTESTs are given still another KSTEST.                       ::
 *
 *                       Comments
 * 
 * From the papers I've collected, the reason for the overlap is
 * simply to use all of the data when observing whether or not
 * the quantities are uniformly distributed.  The use of a
 * covariance matrix doesn't test anything different, it just
 * uses fewer rands to achieve a given sensitivity.  I would therefore
 * strongly suggest that users use the far more sensitive and reliable
 * rgb_lagged_sum test which tests more or less the same thing in an
 * unbroken and scalable way.
 *
 * At this point I think there is rock solid evidence that this test
 * is completely useless in every sense of the word.  It is broken,
 * and it is so broken that there is no point in trying to fix it.
 * If I crank up tsamples to where one can actually see the asymptotic
 * distribution (which should be uniform) it is visibly non-uniform and
 * indeed the final kstest CONVERGES to a non-zero pvalue of 0.09702690
 * for ALL rngs tested, which hardly seems useful.  If one runs the test
 * on only 100 samples (as Marsaglia's code did in both the original
 * fortran or the newer C version) but then increases the number of
 * runs of the test from the default 100, it is easy to make it fail for
 * gold standard generators.  The test just doesn't work.  It cannot be
 * used to identify a generator that fails the null hypothesis.
 *
 * Don't use it.
 *========================================================================
 */

// Test computes sums of m [100 in diehard] uniform random variables (in the hope
// that the sum has almost normal distribution. Then one of the 
// terms is replaced by a new variable, new sum is computed, etc., 
// unless we have m correlated sums. They they are linearly 
// transformed (by some formulas that are given by Marsaglia
// without derivation and then _changed_ is dieharder (though
// the comments in dieharder explain author does not understand
// the derivation) to get (presumably) 100 independent normal
// variables. Then they are converted to 100 (presumably) independent
// and uniformly distributed variables and 1-sample KS-test is applied.
// Then this procedure is repeated with fresh random integer, and
// another KS-test value is computed. In the original Marsaglia this was
// done 100 times to get 100 p-values, and then KS-test was applied
// to these 100 values, to get one second-order KS-value. Then everythin
// was repeated to get 10 second-order KS-values, and one final KS-value
// was made out of them.

// In our code the test has dimension (param[2]) 1 and returns one p-value
// obtained from 2m-1 random integers. The value of m is taken from
// param[3]; it is not clear whether the formulas are valid for any
// m or for m=100 only;

#define MAXINT 4294967296l
#define MAX(x,y) ((x)>(y)?(x):(y))

int cmpdbl(const void *a, const void *b){
  if (*(double*)a < *(double*)b){return(-1);}
  if (*(double*)a > *(double*)b){return(1);}
  return(0);
}


bool osums (long double *value, unsigned long *hash, PRG gen, int *param, bool debug){
  assert (param[2]==1); // only one value is produced by the test
  int m = param[3]; // number of terms in the sum and the number of sums
                    // uses 2m-1 calls to the generator
  assert (m>1);                  
  double x[m]; // first m reals                    
  for (int i=0; i<m; i++){
    unsigned int next;
    if(!g_int32_lsb(&next, gen)){return(false);}
    x[i]= ((double)next)/((double) MAXINT); 
  }
  // x[0..m) is filled 
  double y[m]; // m sums
  // compute y[0]:
  double s=0.0;
  for (int i=0; i<m; i++){
      s+=x[i];
  }
  y[0]= s;
  // it remains to compute y[1..m) replacing x[0..m-1) by fresh reals
  for (int i=1; i<m; i++){
    // get fresh real:
    unsigned int next;
    if(!g_int32_lsb(&next, gen)){return(false);}
    y[i]=y[i-1]-x[i-1]+((double)next)/((double) MAXINT); 
  }
  // y[0..m) are the overlapping sums
  double mean=((double)m)/2.0;
  // expected value of the sum is mean, and the
  // variance is m/12
  for (int i=0; i<m; i++){
    y[i]=(y[i]-mean)*sqrt((double) 12);
  }
  // now the expected value of each y is 0, and variance is m.
  // now the transformation using Marsaglia's recipe
  // reusing the x-array
  // Marsaglia's code with changed names:
  x[0]=y[0]/sqrt((double)m);
  x[1]= -x[0]*((double)m-1.0)/sqrt(2*((double)m)-1.0)+y[1]*sqrt(((double)m)/(2.0*((double)m)-1.0));
  for(int k=2; k< m; k++){
    double a=2*m+1-k;
    double b=2*a-2;
    // Diehard: x[k]= y[0]/sqrt(a*b)-sqrt((a-1)/(b+2))*y[k-1]+y[k]*sqrt(a/b);
    // Dieharder uses y[k-2] instead of y[0] with no explanations 
    x[k]= y[0]/sqrt(a*b)-sqrt((a-1)/(b+2))*y[k-1]+y[k]*sqrt(a/b);
  }
  // now presumably x[0..m) are independently sampled from standard normal distribution
  for (int i=0; i<m; i++){
    x[i] =  gsl_cdf_gaussian_P(x[i],1.0);
  }
  // now presumably x[0..m) are independent uniformly distributed in [0,1]
  if (debug){
    printf("Presumably independent uniform variables obtained from overlapping sums:\n");
    for (int i=0; i<m; i++){
      printf("%8.6lf\n", x[i]);
    }
  }
  // now sorting:
  qsort(x, m, sizeof(double),cmpdbl);
  if (debug){
    printf("After sorting:\n");
    for (int i=0; i<m; i++){
      printf("%8.6lf\n", x[i]);
    }
  }  
  // array x is sorted and we may compute KS-deviation: 
  // max(x[0]-0/m,x[1]-1/m,...,x[m-1]-(m-1)/m, 1/m-x[0],2/m-x[1],...,1-x[m-1]);
  double d= 0;
  for (int i=0; i<m; i++){
    d= MAX(d, x[i]-((double)i)/((double)m) );
    d= MAX(d, ((double)(i+1))/((double)m)-x[i]);  
  }  
  // d= Kolmogorov-Smirnov 1-sample deviation for x   
  value[0]= ks1(m,d);
  if (debug){
    printf("The resulting 1-sample KS p-value: %15.13Lf\n", value[0]);
  }
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}
