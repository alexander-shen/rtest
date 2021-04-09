// Discrete Fourier Transform (Spectral) Test as described uin NIST
// (800-22, section 3.5, page 3-6 and 3-7)
//
// The dimension of the test is 1, the number of used bits is a power of 2
// (at least 32, since bits are taken from 32-but integers)
// The sequence of bits is interpreted as +1/-1, and a discrete Fourier
// transform is computed:
// f_j = \sum_{k=0}^{n-1} x_k exp (2\pi i (kj/n))
// Then the absolute values (lengths) of complex numbers f_j (first n/2 
// of them due to symmetry, all x_i are real) are considered, and we 
// count those who are less than some threshold h=\sqrt{ln(1/0.05)n}. For the 
// two-sample test that's enough (and any value of threshold can be used), 
// but NIST says that about 95% of all values f_j should be less
// than this threshold, and their distribution should be close to normal
// that corresponds with probability of success 95%, so the number of
// values smaller than h is transformed to supposedly uniform distribution
// using normality approximation 
//
// Discrete FFT taken from GSL (https://www.gnu.org/software/gsl/doc/html/fft.html)



#include "test_func.h"
#define REAL(z,i) ((z)[2*(i)])
#define IMAG(z,i) ((z)[2*(i)+1])

bool fftest (long double *value, unsigned long *hash, PRG gen,
             int *param, double *real_param, bool debug){
  assert (param[2]==1); // only one value is produced by the test
  long m = param[3]; // uses m ints = 32m bits; m should be power of 2
  unsigned int p= m;                 
  while (p%2==0){p/=2;}
  if (p!=1){
     fprintf(stderr, "The number of used ints in FFT test should be a power of 2\n");
     assert (false);
  }
  unsigned long n = m*32; // number of bits used
  if(debug){printf("Running FFT test for %ld ints, %ld bits\n",m,n);}
  double r[2*n]; // array of n complex numbers; s-th number is (r[2s],r[2s+1])
  for (long i=0; i<n; i++){IMAG(r,i)=0.0;} // FFT applied to real numbers (+-1s)
  long filled= 0; // number of bits already there
  for (long i=0; i<m; i++){ // reading m integers and putting n=32m bits into r as +-1s
    unsigned int next;
    if(!g_int32_lsb(&next, gen)){return(false);}
    for (int k=0; k<32; k++){// extracting 32 bits starting from least significant ones
      bool nextbit= next%2;
      REAL(r,filled)= (nextbit? 1.0 : -1.0);
      filled++;
      next/=2;
    } 
  }
  assert (filled==n);
  // Now we are ready to call FFT for n complex numbers
  gsl_fft_complex_radix2_forward (r, 1, n);
  // [In GSL the forward transformation has additional minus under the exponent,
  // but this gives just conjugated values, so this is ok for us
  // n is the number of complex numbers, the second argument is "stride",
  // it is equal to 1 (and this means that we consider an entire array, not
  // some arithmetic sequence of indices)
  // The transformation is performed in place
  double threshold = sqrt(2.99573227355*n); // the constant is ln(1/0.05)
  double tsquare= threshold*threshold;
  long count=0;
  for (long i=0; i<n/2; i++){
     double re= REAL(r,i);
     double im= IMAG(r,i);
     if (re*re+im*im>=tsquare){count++;}
  } 
  // count is the number of values among the first n/2 exceeding h  
  // it should be about 0.05(n/2) according to theory
  double expected= 0.05*((double)n)/2.0;
  double sigma= sqrt( (((double)n)/2.0)*0.05*0.95 );
  // for binomial (N,p) distribution sigma=sqrt(Np(1-p))
  // converting to presumably uniform distribution
  value[0]=gsl_cdf_gaussian_P( ((double) count)-expected, sigma);
    if(debug){
    printf("Out of %ld values there are %ld exceeding threshold %lf\n", n/2, count, threshold);
    printf("Presumably uniform p-value: %Lf\n", value[0]);
  }
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}  
