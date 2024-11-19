#include "test_func.h"

// List of all test functions (see test_func.h about their type)

// Here one should provide references to all test functions and their short descriptions
test_function functions_list[]={
  {all_bytes,"Number of readings before all 256 bytes appear"},
  {all_16,"Number of reading before all 65536 16-but strings appear"},
  {dummy_dimension, "Test function for debugging purposes"},
  {sts_serial, "Serial test according to NIST 800-22"},
  {opso, "dieharder_opso test according to Marsaglia description"},
  {oqso, "dieharder_oqso test according to Marsaglia description"},
  {bytedistrib, "dab byte distribution test from dieharder suite"},
  {knuth_runs, "Run test from Knuth-diehard-dieharder"},
  {osums, "Overlapping sums test from diehard"},
  {ent_8_16,"Entropy and p-value for chi-square, 8 and 16-bit distributions"},
  {fftest, "NIST 800-22 Fourier transform test"},
  {rank32x32, "Dieharder 32x32 rank test"},
  {rank6x8, "Dieharder 6x8 rank test"},
  {bitstream_o, "Overlapped bitstream test"},
  {bitstream_n, "Overlapped bitstream test"},
  {lz_split, "Lempel-Ziv test restored from NIST"},
  {birthdays, "Diehard/dieharder birthday test"},
  {mindist2d, "2D minimal distance 32 int pairs test"}
};

int len_func_list=sizeof(functions_list)/sizeof(test_function);


// For testing 
bool dummy_dimension(long double *value, unsigned long *hash, PRG gen, 
                     int *param, double *real_param, bool debug){
  static int count=0;
  for (int i=0; i<param[2]; i++){ // dimension=param[2] according to conventions
    value[i]=(long double)count+i*0.01;       
  }   
  *hash= count;
  count++;
  return(true);
}

// how many bytes needed to see all 256 values
bool all_bytes (long double *value, unsigned long *hash, PRG gen,
                int *param, double *real_param, bool debug){
  assert (param[2]==1); // this function returns one value
#define NUM_BYTES 256  
  bool occur[NUM_BYTES];
  for (int i=0; i<NUM_BYTES; i++){occur[i]= false;}
  long count= 0;
  long num_observed=0;
#define BOUND 20000 // to bound the number of trials  
  while ((num_observed<NUM_BYTES)&&(count<BOUND)){
    byt next;
    if (!g_byte (&next,gen)) {return(false); }
    if (!occur[next]){
      occur[next]= true;
      num_observed++;
    }
    count++;   
  }
  *value = (long double) count;
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) (unsigned)h2)<<32)+((unsigned long)(unsigned)h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}

// how many 16-bit integers needed to see all 2^{16} values
bool all_16 (long double *value, unsigned long *hash, PRG gen,
             int *param, double *real_param, bool debug){
  assert (param[2]==1); // this function returns one value
#define NUM_16 65536  
  bool occur[NUM_16];
  for (int i=0; i<NUM_16; i++){occur[i]= false;}
  long count= 0;
  long num_observed=0;
#define BOUND_16 1000000 // to bound the number of trials  
  while ((num_observed<NUM_16)&&(count<BOUND_16)){
    byt next1, next2;
    if (!g_byte (&next1,gen)) {return(false); }
    if (!g_byte (&next2,gen)) {return(false); }
    int int16= (((unsigned int)next1)<<8)+((unsigned int)next2);
    if (!occur[int16]){
      occur[int16]= true;
      num_observed++;
    }
    count++;   
  }
  *value = (long double) count;
  // use eight more bytes for hash
  unsigned int h1, h2;
  g_int32_lsb(&h1,gen); g_int32_lsb(&h2,gen);
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) (unsigned)h2)<<32)+((unsigned long)(unsigned)h1);
  if(debug){print64(*hash); printf("\n");}
  return(true);
}
