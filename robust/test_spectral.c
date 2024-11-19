#include "test_func.h"
#include "../spectral_tests/spectral_rtest.h"

// Spectral test (TODO: description)

#define MIN_GAP 0.00001
#define MIN_ERROR 0.00001
#define MAX_SIZE 65536
#define MAX_DEGREE 2048
#define MAX_ITERATIONS 100000

#define ERR(m,n) {fprintf(stderr,(m),(n)); exit(1);}

PRG current_generator;

// for the byte generator to be sent to C++ code
bool get_byte(byt *b){
  return g_byte(b, current_generator);
}

bool spectral (long double *value, unsigned long *hash, PRG gen,
            int *param, double *real_param, bool debug){
  assert (param[2]==2); // two values are produced by the test (lower/upper bounds)
  long size= param[4];
  long degree= param[5];
  long iterations= param[6];
  long double gap= real_param[0];
  long double error= real_param[1];
  long double lower, upper;
  if (gap<MIN_GAP) ERR("Gap in spectral test should be at least %f\n", MIN_GAP)
  if (error<MIN_ERROR) ERR("Error in spectral test should be at least %lf\n", MIN_ERROR)
  if (size < 4) ERR("Graph size [%ld] in spectral test should be at least 4\n", size)
  if (size > MAX_SIZE) ERR("Graph size in spectral test should be at most %d\n", MAX_SIZE)
  int lsize=0; long psize= 1; // psize=2^lsize
  while (psize < size){psize*=2; lsize++;} 
  // psize >= size
  if (psize!=size) ERR("Graph size [%ld] in spectral test should be a power of 2\n",size) 
  if (degree < 1) ERR("Degree [%ld] in spectral test should be positive\n", degree) 
  if (degree%2!=0) ERR("Degree [%ld] in spectral test should be even\n", degree)   
  if (degree > MAX_DEGREE) ERR("Degree in spectral test should be at most %d\n", MAX_DEGREE) 
  if (iterations < 1) ERR("Iterations number [%ld] in spectral test should be positive\n", iterations) 
  if (iterations > MAX_ITERATIONS) ERR("Iterations number in spectral test should be at most %d\n", MAX_ITERATIONS) 
  // parameters checked
  if (debug){
    printf("Spectral test:\n");
    printf("Number of vertices = %ld, average degree= %ld\n", size, degree);
    printf("%ld iterations, gap = %Lf, error = %Lf\n", iterations, gap, error);
  }
  current_generator= gen;
  // now the byte generator (to be sent to process_graph) is ready
  process_graph(get_byte, gap, error, size, degree, iterations, &lower, &upper);
  // lower and upper are the bounds for the second eigenvalue of a random graph constructed
  // using bytes from get_byte function
  
  value[0]= lower;
  value[1]= upper;
  // use eight more bytes for hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}
