// This file implements two tests from the dieharder. One studies the 
// rank of 32*32 matrices over Z_2 and is described as follows (warning:
// the comments in the file diehard_rank_32x32.c speak about 31*31 matrices
// as it was in diehard, but the code is for 32x32):
/* Binary Rank Test
This is the BINARY RANK TEST for 32x32 matrices. A random 32x32
binary matrix is formed, each row a 32-bit random integer.
The rank is determined. That rank can be from 0 to 32, ranks
less than 29 are rare, and their counts are pooled with those
for rank 29.  Ranks are found for 40,000 such random matrices
and a chisquare test is performed on counts for ranks  32,31,
30 and <=29. As always, the test is repeated and a KS test applied to the
resulting p-values to verify that they are approximately uniform.
*/

// Another test checks 6x8 matrices. The dieharder description says
/*
              Diehard 6x8 Binary Rank Test
 This is the BINARY RANK TEST for 6x8 matrices.  From each of
 six random 32-bit integers from the generator under test, a
 specified byte is chosen, and the resulting six bytes form a
 6x8 binary matrix whose rank is determined.  That rank can be
 from 0 to 6, but ranks 0,1,2,3 are rare; their counts are
 pooled with those for rank 4. Ranks are found for 100,000
 random matrices, and a chi-square test is performed on
 counts for ranks 6,5 and <=4.

As always, the test is repeated and a KS test applied to the
resulting p-values to verify that they are approximately uniform.
*/
// Our test has dimension 25 (for 32-8+1 possible positions of a byte
// inside 32-bit integer), for each 6 consecutve integers 25 matrix
// are formed and their ranks are computed. Then chi-square test is
// applied to each position separately and 25 values are returned


#include "test_func.h"

// Binary vectors of size <= ROW_LENGTH are reprented by unsigned
// integers (long integers can be used for 64-bit rows)

typedef unsigned int row;
#define ROW_LENGTH 32


// rank of the matrix with rows m[0],..., m[h-1]
// only w last columns may be non-zero
int rank(row *m, int w, int h){
  assert (w<=ROW_LENGTH);
  if (w==0) {return(0);}
  if (h==0) {return(0);}
  row first_column_mask = 1<<(w-1);
  // find a row with 1 in the first column
  int i= 0; // m[0..i-1) have 0 in the first column
  while (true){
    // zero first column?
    if (i==h){return (rank(m,w-1,h));}
    // i<h; ith row starts with zero?
    if ((m[i]&first_column_mask)==0){i++; continue;}
    // i<h; ith row has 1 in the first column
    break;
  }
  // i<h; m[i] has 1 in the first column
#define EXCHANGE_ROWS(i,j) {row tmp=m[(i)]; m[(i)]= m[(j)]; m[(j)]= tmp;}
  // exchange ok if i=j
  EXCHANGE_ROWS(0,i);
  //  m[0] starts with 1
  assert((m[0]&first_column_mask)==first_column_mask);
  for (int i=1; i<h; i++){
    if ((m[i]&first_column_mask)!=0){m[i]^=m[0];}
    assert ((m[i]&first_column_mask)==0);
  }
  EXCHANGE_ROWS(0,h-1);
  return(1+rank(m,w-1,h-1));
}


// given n=param[3], reads 32*n 32-bit unsigned integers and computes n binary ranks 
// 32*32 matrices. Counts the fraction of matrices with ranks 32, 31, 30, 29-
// and compares with the theoretical probabilities using chi-square test
bool rank32x32 (long double *value, unsigned long *hash, PRG gen, int *param, bool debug){
  assert (param[2]==1);
  long n=param[3];
  assert (n>0);
  long count[4]; // number of matrices with rank 29-, 30, 31, 32
  for (int i=0; i<4; i++){count[i]= 0;}
  for (long i=0; i<n; i++){
    //read 32 integers into a matrix:
    row m[32];
    for (int j=0; j<32; j++){
      unsigned int next;
      if (!g_int32_lsb(&next,gen)){return(false);}
      m[j]= next;        
    }
    int rk=rank(m,32,32);
    if (debug){printf("Rank of a 32*32 matrix: %d\n",rk);}
    int pos= (rk<29 ? 0 : rk-29);
    count[pos]++;
  }
  // count is filled 
  double expected[4];
  expected[0]=((double)n)*0.0052854502;
  expected[1]=((double)n)*0.1283502644;
  expected[2]=((double)n)*0.5775761902;
  expected[3]=((double)n)*0.2887880952;
  // using chi-square test
  int ndf= 3; // number of degrees of freedom 
  double chisquare= 0.0;
  for (int i=0; i<4; i++){
    chisquare+= (count[i]-expected[i])*(count[i]-expected[i])/expected[i];
  }
  if (debug){printf("gsl_sf_gamma_inc_Q args: %lf, %lf\n", ndf/2.0, chisquare/2.0);}
  value[0]= gsl_sf_gamma_inc_Q(ndf/2.0,chisquare/2.0);
  if(debug){printf("p-value for rank 32*32 test: %15.13Lf\n",value[0]);}
  // get hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}

bool rank6x8 (long double *value, unsigned long *hash, PRG gen, int *param, bool debug){
  assert(param[2]==25); // dimension is 25
  long n= param[3]; // number of 6x8 matrices for each position (the total number is 25*n
  long count[25][3]; // as Marsaglia recommends, we count ranks <=4, 5, 6
                     // \texttt{dieharder} count also ranks <=2 and 3 separately, but 2
                     // the probabilities are 0.149858E-06 and 0.808926E-04, so this
                     // does not make much sense for reasonable sample sizes
                     // count[i][j]=# matrices in position i with (truncated) rank j
  for (int i=0; i<25; i++){
    for (int j=0; j<3; j++){
      count[i][j]= 0;
    }
  }
  // count is initialized
  for (long i=0; i<n; i++){
    // get 6 32-bit integers, for each of 25 position compute rank and increment the counters
    row m[6]; // 6*32 matrix
    for (int j=0; j<6; j++){
      unsigned int next;
      if (!g_int32_lsb(&next,gen)){return(false);}
      m[j]= next;        
    }
    // matrix is filled
    row m8[6]; // 6*8 matrix for rank computation
    for (int k=0; k<25; k++){
      //compute 6*8 rank for position k
      for (int l=0; l<6; l++){
        m8[l]=((m[l])>>k)&((unsigned int)255);
      }
      int r=rank(m8,8,6);
      //increase the counter:
      count[k][(r<4?0:r-4)]++;
    }
  }
  // counters are filled
  double expected[3];
  expected[0]=((double)n)*0.009443;
  expected[1]=((double)n)*0.217439; 
  expected[2]=((double)n)*0.773118;
  int ndf= 2; // numbers of degrees of freedom
  for (int k=0; k<25; k++){
    double chisquare= 0.0;
    for (int i=0; i<3; i++){
      chisquare+= (count[k][i]-expected[i])*(count[k][i]-expected[i])/expected[i];
    }
    if (debug){printf("k=%d, gsl_sf_gamma_inc_Q args: %lf, %lf\n", k, ndf/2.0, chisquare/2.0);}
    value[k]= gsl_sf_gamma_inc_Q(ndf/2.0,chisquare/2.0);
    if (debug){printf("p-value for chi-square: %Lf\n", value[k]);}
  }
  // get hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");}
  return(true);
}
