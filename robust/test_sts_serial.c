/*
 * This code is adapted from the Dieharder suite (sts_serial.c) and follows Section
 * 2.11 (Serial test) from NIST 800-22 document
 */

/* The test depends on two parameters: the number of bits (n) analyzed
 and the maximal length of substrings analyzed (m). It produces several
 an array of 3m values (2 values called P-value1 and P-value2 in the NIST, 
 for each string of length up to m.

 Let X be the sequence of n bits to be analyzed. For each word w of length 
 at most l we count the number count[w] of occurences of w in X (considered 
 as a cyclic word); the sum of all counts[w] for words w of given length 
 equals n. The we compute 
   psi_square [k] = (2^{k}/n)\sum_x (count[x]^2) - n
 where the sum is taken over all strings x of length k.
 
 For our purposes the values psi_square[k] can be used directly, but NIST
 recommends to consider the first and second differences
   delpsi[k] = psi_square[k] - psi_square[k-1];
   delpsi2[k] = psi_square[k] - 2psi_square[k-1] + psi_square[k-2];
 The dieharder code suggests to let psi_square[0]=0, so delpsi[k] is 
 defined for k>=1 and delpsi2 is defined for k>=2.  
 
 These numbers are converted to p-values according to approximate distributions,
    pvalue1 = igamc(2^{k-2},delpsi[k]/2),
    pvalue2 = igamc(2^{k-3},delpsi2[k]/2).
 [Here NIST description is inconsistent: 2-28 (5) does not include "/2",
 but the example later does include division by 2. The numerical values
 0.9057 and 0.8805 in this example are incorrect, but correct values are
 given in 2.11.6. The description above follows the NIST/dieharder code.]
 
 Checked on e=2.1718... bit sequnces from NIST software and the results
 are as mentioned in 2.11.6 of NIST document
 
 */

#include "test_func.h"
#include <gsl/gsl_math.h>

bool sts_serial (long double *value, unsigned long *hash, PRG gen, int *param, bool debug){
  int m= param[2]/3; // we need space to return 3m values (raw and processed)
  assert ((m>0)&&(m<32));
  assert (sizeof(int)>=4); assert(sizeof(long)>=8); // let us check here, too...
  for (int i=0; i<param[2];i++){value[i]=(long double)0.0;} 
  // unused values should be 0 
  long nint = param[3]; // number of int samples to be processed
  long n=nint*32; // number of bytes to be processed
  unsigned int buf,curr,loop;
  int bufc,currc; // the number of bits in buf and curr
  unsigned int *count;
  unsigned int powerm = 1<<m; // powerm = 2^m; note that m<32, so shift is legal
  count= (unsigned int *)malloc(powerm*sizeof(int));
  if (count==NULL){fprintf(stderr, "Not enough memory\n"); exit(1);}
  for (int i=0; i<powerm; i++){count[i]= 0;}
  
  /* Bits are read from the generator in groups of 32 bits, as ints.
  Since many generators produce them in this way, it is better to keep 
  the byte ordering unchanged. These ints are placed in buf, and 
  bufc is the numbers of bits that are left unused in buf. The processed
  bits are accumulated in curr, and as soon as there are m bits, we start
  filling the count array */
  
  int currsize=0; // number of bits already in curr; initially 0, then increases 
                  // up to m and stays at m
  int used_ints;
  int counted_words= 0;
  unsigned int mask= powerm-1; // m least significant bits = 1
  
  bool start = true;
  while(true){
    if (start){
      if (!g_int32_lsb (&buf,gen)) {free(count); return(false); }
      bufc= 32;
      loop= buf; // to be used as the next byte after the end
      used_ints= 1; currc=0;      
      start= false;
      continue;
    }
    if ((currc<m) && (bufc>0)){
      // get bit from the buffer and put it to current;
      bufc--;
      bool bit= ((buf&(((int)1)<<bufc))!=0);
      curr= ((curr<<1)|bit) & mask;
      currc++;
      continue;
    }
    if ((currc<m) && (bufc==0) && (used_ints<nint)){
      if (!g_int32_lsb (&buf,gen)) {free(count); return(false); }
      bufc= 32;
      used_ints++;
      continue;
    }
    if ((currc<m) && (bufc==0) && (used_ints==nint)){
      buf= loop;
      bufc= 32;
      used_ints++;
      continue;
    }
    if ((currc==m)&&(counted_words<n)){
      count[curr]++;
      counted_words++;
      currc--;
      continue;
    }
    if ((currc==m)&&(counted_words==n)){
      break;
    }
    assert(false); // this should not happen!
  }
  double long psi_square[m+1]; // last value is psi_square[m]
  int mm= m;
  long data_len= powerm;
  // count cointains 2^mm counts of mm-bit strings in the first 2^mm positions;
  // psi_square[i] are computed for all i>mm
  // data_len = 2^mm
  int count0,count1;
  while (mm!=0){
    //compute psi_square[mm]:
    long double sum=0.0;
    for (long x=0; x<data_len; x++){sum+=((long double)count[x])*((long double)count[x]);}
    psi_square [mm] = (((long double) data_len)/((long double)n))*sum - (long double) n;
    // combine counts[x0] and count[x1] to get count[x] (and put it in place of count[x0]:
    data_len/= 2;
    for (long x=0; x<data_len; x++){count[x]+= count[x+data_len];}
    mm--; 
    if(mm==1){count0=count[0]; count1=count[1];} // store for future binomial test as in dieharder
  }
  assert (count[0]== n);
  psi_square[0]=0.0; // extension as described in dieharder
  long double delpsi[m+1], delpsi2[m+1];
  for (int k=1; k<=m; k++){delpsi[k] = psi_square[k] - psi_square[k-1];}
  for (int k=2; k<=m; k++){delpsi2[k] = psi_square[k] - 2*psi_square[k-1] + psi_square[k-2];}
  // filling raw values:
  for (int k=1; k<=m; k++){value[k-1]= psi_square[k];} // first returning m raw values (shifted)
  
  // This is sts_monobit, basically.
  long double mono_mean =  (long double)count0-(long double)count1;
  long double mono_sigma = sqrt((double)n);
  value[m]= (long double) gsl_cdf_gaussian_P(mono_mean,mono_sigma);
  // registering p-value for the #0-#1-test (using binomial distribution)
  for (int k=2; k<=m; k++){
    value[m+k-1]=gsl_sf_gamma_inc_Q((1<<(k-2)),delpsi[k]/2.0);
    double arg1= (k==2 ? 0.5 : 1<<(k-3));
    double arg2= delpsi2[k]/2;
    if (arg2<0.0){
      fprintf(stderr, "Warning: domain error in gsl_sf_gamma(%lf, %lf)\n", arg1, arg2);
      arg2= 0.0;
    } // to prevent domain errors due to rounding;
    value[2*m+k-1]=gsl_sf_gamma_inc_Q(arg1,arg2);
  }
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){free(count);return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if(debug){
    print64(*hash); printf("\n");
    printf("Raw values:\n");
    for (int k=0; k<m; k++){printf("value[%d]=%Lf\n", k, value[k]);}
    printf("First differences processed:\n");
    for (int k=m; k<2*m; k++){printf("value[%d]=%Lf\n", k, value[k]);}
    printf("Second differences processed:\n");
    for (int k=2*m; k<3*m; k++){printf("value[%d]=%Lf\n", k, value[k]);}  
  }  
  free(count);  
  return(true);
}

