// From https://www.jstatsoft.org/article/view/v008i18 (Marsaglia-Tsang-Wang)
// Removed the main() program and replaced double by long double
// inverted: ks1(n.d) = 1-K(n,d) = Pr[deviation_n >= d]

/*
 The C program to compute Kolmogorov's distribution

             K(n,d) = Prob(D_n < d),         where

      D_n = max(x_1-0/n,x_2-1/n...,x_n-(n-1)/n,1/n-x_1,2/n-x_2,...,n/n-x_n)

    with  x_1<x_2,...<x_n  a purported set of n independent uniform [0,1)
    random variables sorted into increasing order.
    See G. Marsaglia, Wai Wan Tsang and Jingbo Wong, J.Stat.Software.
*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "k1smirnov.h"

    /* prototypes*/
static long double K(int n, long double d) ;                                             //Kolmogorov distribution
static void mMultiply(long double *A, long double *B, long double *C,int m) ;            //Matrix product
static void mPower(long double *A,int eA, long double *V,int *eV,int m,int n);           //Matrix power

long double ks1 (int n, long double d){
  return(1.0-K(n,d));
}

static long double K(int n, long double d)
{
   int k,m,i,j,g,eH,eQ;
   long double h,s,*H,*Q;
    /* OMIT NEXT TWO LINES IF YOU REQUIRE >7 DIGIT ACCURACY IN THE RIGHT TAIL*/
s=d*d*n;
if(s>7.24||(s>3.76&&n>99)) return 1-2*exp(-(2.000071+.331/sqrt(n)+1.409/n)*s);
   k=(int)(n*d)+1;
   m=2*k-1;
   h=k-n*d;
   H=(long double*)malloc((m*m)*sizeof(long double));
   Q=(long double*)malloc((m*m)*sizeof(long double));
       for(i=0;i<m;i++)
         for(j=0;j<m;j++)
           if(i-j+1<0) H[i*m+j]=0;
          else     H[i*m+j]=1;
    for(i=0;i<m;i++)
    {
    H[i*m]-=pow(h,i+1);
    H[(m-1)*m+i]-=pow(h,(m-i));
    }
    H[(m-1)*m]+=(2*h-1>0?pow(2*h-1,m):0);
    for(i=0;i<m;i++)
    for(j=0;j<m;j++)
    if(i-j+1>0)
        for(g=1;g<=i-j+1;g++) H[i*m+j]/=g;
    eH=0;
    mPower(H,eH,Q,&eQ,m,n);
    s=Q[(k-1)*m+k-1];
    for(i=1;i<=n;i++)
    {
    s=s*i/n;
    if(s<1e-140){s*=1e140; eQ-=140;}
    }
    s*=pow(10.,eQ);
    free(H);
    free(Q);
    return s;
}


static void mMultiply(long double *A, long double *B,long double *C,int m)
{
    int i,j,k;
    long double s;
    for(i=0;i<m;i++)
    for(j=0;j<m;j++)
    {
    s=0.;
    for(k=0;k<m;k++) s+=A[i*m+k]*B[k*m+j];
    C[i*m+j]=s;
    }
}

static void mPower(long double *A,int eA,long double *V,int *eV,int m,int n)
{
    long double *B;int eB,i;
       if(n==1)
        {
        for(i=0;i<m*m;i++) V[i]=A[i];
        *eV=eA;
        return;
        }
      mPower(A,eA,V,eV,m,n/2);
      B=(long double*)malloc((m*m)*sizeof(long double));
      mMultiply(V,V,B,m);
      eB=2*(*eV);
    if(n%2==0) {for(i=0;i<m*m;i++) V[i]=B[i]; *eV=eB;}
    else   {mMultiply(A,B,V,m);*eV=eA+eB;}
    if(V[(m/2)*m+(m/2)]>1e140)
      {for(i=0;i<m*m;i++) V[i]=V[i]*1e-140; *eV+=140;}
 free(B);
}

/* main program commented out:
int main()
{   int n;
    long double d;
    while(1)
    {
    printf("Please give values for n and d:");
    scanf("%d%Lf",&n,&d);

    printf("             Prob(D_%d < %Lf) = %18.16Lf\n",n,d,K(n,d));
    }
}
*/

