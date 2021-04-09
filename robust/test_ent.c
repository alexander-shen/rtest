// This test takes param[3] 32-bit integers and looks at
// the distribution of bytes and 16-int halves, computing
// their entropy (that should be theoretically close to 8
// and 16, and chi-square test value (converted to approximately
// uniform distribution

#include "test_func.h"

bool ent_8_16 (long double *value, unsigned long *hash, PRG gen, 
               int *param, double *real_param, bool debug){
  assert (param[2]==4); // returns ent8, ent16, chisquare8, chisquare 16
  long n= param[3]; // number of 32-bit ints that should be analyzed
  assert (n>0);
  // initialize the arrays for counting bytes and 16-ints
  if (debug){
    printf("Starting entropy/chi-square test for bytes and 16-ints using %ld 32-ints\n", n);
  }
  unsigned int *bytes, *int16s;
  bytes= (unsigned int *) malloc(256*sizeof(int));
  if (bytes==NULL){fprintf(stderr,"Not enough memory\n"); exit(1);}
  int16s= (unsigned int *) malloc(65536*sizeof(int));
  if (int16s==NULL){free(bytes); fprintf(stderr,"Not enough memory\n"); exit(1);}
  for (int i=0; i<256; i++){bytes[i]= 0;}
  for (int i=0; i<65536; i++){int16s[i]= 0;}
  //counter array initialized
  if(debug){printf("Counters initialized\n");}
  for (int i=0; i<n; i++){
    unsigned int next;
    if(!g_int32_lsb(&next, gen)){printf("oops.. no input\n"); free(bytes); free(int16s); return(false);}
    int16s[next%65536]++; int16s[next/65536]++;
    for (int j=0; j<4; j++){
      bytes[next%256]++;
      next/=256;
    }
  }
  if(debug){printf("Counting finished\n");}
  // bytes[] and int16s[] are filled
  long double ent8= 0.0;
  long double expectation8= ((long double) 4*n)/256.0; 
  long double chi8= 0.0;
  for (int i=0; i<256; i++){
    if (bytes[i]>0){
      long double freq = ((long double) bytes[i])/(((long double)n)*4.0);
      ent8-= freq * (logl(freq)/logl(2.0));
    }  
    long double deviation = ((long double) bytes[i]) - expectation8;
    chi8+= deviation*deviation/expectation8;
  }
  value[0]= ent8;
  if (debug){
    printf("The entropy for the distribution of %ld bytes is %Lf\n", 4*n, ent8);
  }
  value[1]= gsl_sf_gamma_inc_Q(255.0/2.0,chi8/2.0);
  if (debug){
    printf("The chi-square p-value for the distribution of %ld bytes is %Lf\n", 4*n, value[1]);
  }  
  // bytes distribution parameters are ready
  long double ent16= 0.0;
  long double expectation16= ((long double) 2*n)/65536.0; 
  long double chi16= 0.0;
  for (int i=0; i<65536; i++){
    if (int16s[i]>0){
      long double freq = ((long double) int16s[i])/(((long double)n)*2.0);
      ent16-= freq * (logl(freq)/logl(2.0));
    }  
    long double deviation = ((long double) int16s[i]) - expectation16;
    chi16+= deviation*deviation/expectation16;
  }
  value[2]= ent16;
  if (debug){
    printf("The entropy for the distribution of %ld 16-bit ints is %Lf\n", 2*n, ent16);
  }
  value[3]= gsl_sf_gamma_inc_Q(65535.0/2.0,chi16/2.0);
  if (debug){
    printf("The chi-square p-value for the distribution of %ld 16-bit ints is %Lf\n", 2*n, value[3]);
  }  
  // int16 parameters are ready
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){free(bytes); free(int16s); return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if(debug){
    print64(*hash); printf("\n"); 
  }  
  free(bytes);
  free(int16s);
  return(true);
}
