#include "ks2.h"

int main(int argc, char* argv[]){
  if (argc!=4){
    printf("Usage: %s number_of_zeros_n0 number_of_ones_n1 deviation_times_n0_n1\n", argv[0]);
    exit(1);
  }
  int n0= atoi(argv[1]);
  int n1= atoi(argv[2]);
  long dev_n0_n1= atoi(argv[3]);
  double statistic = ((double)(dev_n0_n1))/(((double)n0)*((double)n1));
  printf("Two samples two-side exact Kolmogorov-Smirnov p-value:\n");
  printf ("p-value for two samples (%d+%d) and deviation %f [=%ld/%d*%d] is \n%16.15f\n",
                     n0,n1, 
                     statistic,
                     dev_n0_n1,n0,n1,
                     ks2bar(n0,n1,dev_n0_n1)); 

  double Klm= sqrt( ((double)n0)*((double)n1)/(((double) n0) + ((double)n1)) )* statistic;                                            
  printf("For reference:\n  asymptotic formula: %10.9lf\n  floating point iterations: %10.9Lf\n",
                                         1.0-pkstwo(Klm),     1.0-psmirnov2x(statistic,n0,n1));
  exit(0);
}

