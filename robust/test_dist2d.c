// This test takes 2*param[3] 32-bit integers and interprets pairs of them
// as points in R^2. Then computes the minimal distance between points
// (could be 0 if there are two identical 

#include <time.h>
#include "test_func.h"
#include "minimal_distance.hpp"

// provides void argmin_distance(int * x0, int * y0, int * x1, int * y1, int n, int * X, int * Y)

long double dist2d(long double x0, long double y0, long double x1, long double y1){
  long double dx= x0 - x1;
  long double dy= y0 - y1;
  return sqrt(dx*dx+dy*dy);
}

bool mindist2d (long double *value, unsigned long *hash, PRG gen, 
                int *param, double *real_param, bool debug){
  assert (param[2]==2); // returns minimal distance (scaled)
  long n= param[3]; // number of pairs of 32-bit ints that should be analyzed
                    // 2n 16-bit pairs are analyzed, too
  assert (n>1); // otherwise no pairs!

  if (debug){
    printf("Starting 2d minimal distance test, %ld pairs of 32-bit ints\n", n);
  }
  long double *X, *Y, *X16, *Y16; // arrays for points
  X = (long double*) malloc (n*sizeof(long double));
  if (X==NULL){fprintf(stderr,"Not enough memory\n"); exit(1);}
  Y = (long double*) malloc (n*sizeof(long double));
  if (Y==NULL){fprintf(stderr,"Not enough memory\n"); free(X); exit(1);}
  X16 = (long double*) malloc (2*n*sizeof(long double));
  if (X16==NULL){fprintf(stderr,"Not enough memory\n"); free(X);free(Y); exit(1);}
  Y16 = (long double*) malloc (2*n*sizeof(long double));
  if (Y16==NULL){fprintf(stderr,"Not enough memory\n"); free(X);free(Y);free(X16); exit(1);}
  if(debug){printf("Arrays allocated\n");}
  int i16= 0;
  long double power32= (long double)(((long) 1)<<32);
  long double power16= (long double) (1<<16);
  unsigned int mask16= (1<<16)-1;
  for (int i=0; i<n; i++){
    unsigned int next;
    if(!g_int32_lsb(&next, gen)){free(X); free(Y); return(false);}
    X[i]= (long double) next / power32; 
    X16[i16]=(long double)((next>>16)&mask16)/ power16;
    Y16[i16]=(long double)(next&mask16)/power16;
    i16++;
    if(!g_int32_lsb(&next, gen)){free(X); free(Y); return(false);}
    Y[i]= (long double) next / power32;    
    X16[i16]=(long double)((next>>16)&mask16)/ power16;
    Y16[i16]=(long double)(next&mask16)/power16;
    i16++;
    if(debug){
      printf("X[%d]=%Lf, Y[%d]=%Lf\n", i, X[i], i, Y[i]);
      printf("X16[%d]=%Lf, Y16[%d]=%Lf\n", i16-2, X16[i16-2], i16-2, Y16[i16-2]);
      printf("X16[%d]=%Lf, Y16[%d]=%Lf\n", i16-1, X16[i16-1], i16-1, Y16[i16-1]);
    }  
  }
  assert (i16==2*n);
  if(debug){printf("Arrays filled\n");}
  long double x0,y0,x1,y1; // here the closest points will be
  clock_t begin,end;
  double time_spent;
  if(debug){begin = clock();}
  argmin_distance_real (&x0,&y0,&x1,&y1,n,X,Y);
  if(debug){
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Closest points: \n(%Lf,%Lf)-(%Lf,%Lf)\n",x0,y0,x1,y1);
    printf("Distance: %Lf\n", dist2d(x0,y0,x1,y1));
    printf("Computation time: %lf\n", time_spent);  
  }
  value[0]= dist2d(x0,y0,x1,y1);
  // now the same for 16-bit numbers
  if(debug){begin = clock();}
  argmin_distance_real (&x0,&y0,&x1,&y1,2*n,X16,Y16);
  if(debug){
    clock_t end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Closest points (16 bits): \n(%Lf,%Lf)-(%Lf,%Lf)\n",x0,y0,x1,y1);
    printf("Distance (16 bits): %Lf\n", dist2d(x0,y0,x1,y1));
    printf("Computation time (16 bits): %lf\n", time_spent);  
  }
  value[1]= dist2d(x0,y0,x1,y1);
  // now checking the answer, for 32 bits only
  if (debug){
    begin = clock();
    argmin_distance_real (&y0,&x0,&y1,&x1,n,Y,X); // exchanging the coordinates
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Closest points (computation with exchanged coordinates):\n(%Lf,%Lf)-(%Lf,%Lf)\n",x0,y0,x1,y1);
    printf("Distance: %Lf\n", dist2d(x0,y0,x1,y1));
    printf("Computation time: %lf\n", time_spent);  
    begin = clock();
    // Naive computation:
    int i0, j0;
    long double min=1E+10;
    for (int i=0; i<n; i++){
      for (int j=i+1; j<n; j++){
        long double cur_dist= dist2d(X[i],Y[i],X[j],Y[j]);
        if (cur_dist<min){i0=i; j0=j; min= cur_dist;}
      }
    }
    x0=X[i0]; y0=Y[i0]; x1=X[j0]; y1=Y[j0]; 
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Closest points (naive computation):\n(%Lf,%Lf)-(%Lf,%Lf)\n",x0,y0,x1,y1);
    printf("Distance: %Lf\n", dist2d(x0,y0,x1,y1));
    printf("Computation time: %lf\n", time_spent); 
  }  
  free(X); free(Y); free(X16); free(Y16); 
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if(debug){print64(*hash); printf("\n");}  
  return(true);
}
