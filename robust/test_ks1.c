// This test takes param[3] 32-bit unsigned integers x_1...x_n and 
// considers them (divided by $2^32$) as sample from uniform distribution
// on [0,1], and applies Kolmororov-Smirnov test and the approximation
// for p-values from https://www.jstatsoft.org/article/view/v008i18
// to get a return value.

#include "test_func.h"

int cmplong (const void *a, const void *b){
  if (*(unsigned long*)a < *(unsigned long*)b){return(-1);}
  if (*(unsigned long*)a > *(unsigned long*)b){return(1);}
  return(0);
}

bool ksone (long double *value, unsigned long *hash, PRG gen, 
            int *param, double *real_param, bool debug){
  assert (param[2]==1); // returns one value from KS test
  int n= param[3]; // number of 32-bit ints that should be analyzed
  assert (n>0);
  // initialize the arrays for keeping n long unsigned integer for sorting
  if (debug){
    printf("Starting Kolmogorov-Smirnov test using %d 32-ints\n", n);
  }
  unsigned long *numbers;
  numbers= (unsigned long *) malloc(n*sizeof(unsigned long));
  if(debug){printf("Memory for samples allocated\n");}
  for (int i=0; i<n; i++){
    unsigned int next;
    if(!g_int32_lsb(&next, gen)){printf("oops.. no input\n"); free(numbers); return(false);}
    numbers[i]= (unsigned long) next;
  }
  if(debug){printf("All integers sampled\n");}
  // numbers[0,n) filled
  qsort(numbers,n,sizeof(*numbers),cmplong);
  if(debug){printf("Numbers sorted\n");}
  long unsigned two16 = ((long unsigned)1)<<16;
  long double two32 = ((long double) two16)*((long double) two16);
  long double max_deviation= 0.0; // two-sided deviation
  for (int i=0; i<n; i++){
    long double u = ((long double) numbers[i])/two32;
    long double d_low= ((long double)(i+1))/((long double) n) - u;
    long double d_high = u - ((long double)i)/((long double) n);
    if (d_low > max_deviation) max_deviation= d_low;
    if (d_high > max_deviation) max_deviation= d_high;
  }
  if(debug){printf("Maximal deviation: %Lf\n", max_deviation);}
  long double ksval; // for KS-value  
  ksval= ks1(n,max_deviation); 
  if(debug){printf("KS-value: %Lf\n", ksval);}
  value[0]= ksval;
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){free(numbers); return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if(debug){
    print64(*hash); printf("\n"); 
  }  
  free(numbers);
  return(true);
}
