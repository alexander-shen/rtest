#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

/* compute and print entropy of standard input considered as a sequence of 16-bit integers */

#define N 65536

int main(int argc, char *argv[]){

  int count [N]; /* how many times each integer appears */

  for (int i=0; i<N; i++){count [i]=0;}
  
  int c;
  unsigned int buf = 0;
  int numchar = 0;
  long long numint = 0;
  while ((c=getchar())!=EOF) {
      /* add char to the current buffer as the lsb */
    buf = buf<<8;
    buf += (unsigned char) c;
    buf &= 0xFFFF;
    numchar = 1-numchar;
    assert (0<=buf); assert (buf<N);
    if (numchar==0){numint++; count [buf]++;}
  }
  double ent=0.0;
  for (int i=0; i<N; i++){
    if (count[i]>0){
       assert (numint>0);
       double freq = ((double) count[i])/((double) numint);
       double entadd = freq * (logl(1.0/freq)/logl(2.0));
       ent +=entadd;
    }
  }
  printf ("Number of 16 bit integer samples: %lld\n", numint);
  printf ("Entropy of their distribution: %12.11lf\n", ent);
  exit(0);
}

