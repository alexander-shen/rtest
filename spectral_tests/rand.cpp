/*

Copyright 1997-1999  http://www.yann-ollivier.org/specgraph/specgraph

This software may be freely distributed as is, including
this whole notice.

It may be modified, but any modification should include this whole notice as 
is and a description of the changes made.

This software may not be sold.

This software or any modified version of it may be freely used in free
programs. The program should include a copy of this whole notice. If you
want to use it in a program you sell, contact me

This software comes with absolutely no warranty.

*/

/*
 revision by andrei:
 instead of the standard random generator rand() we use now a primitive linear PRG myrand()
 */


#include<iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

#include<stdlib.h>
#include <sys/time.h>
#include <math.h>

using namespace std;
// a trivial linear pseudo-random generator
// r(i+1) := LinearFactor * r(i) mod Mod;
// see myrand()
unsigned long long int PRGmod = 2147483647; // 2^31-1 ; 99991; // prime number
unsigned long long int LinearFactor = 444555666 ; // co-prime with PRGmod
unsigned long long int RandomNumber = 1234567; // initial value co-prime with PRGmod

/*
 not used anymore

void randomize()
{timeval *tp=new timeval;
gettimeofday(tp,NULL);
srand(tp->tv_usec);
delete tp;
}
*/

// pseudo-random generator returns a periodic sequence of integer numbers;
// the length of the period is Mod - 1
int myrand()
{
    RandomNumber = RandomNumber * LinearFactor % PRGmod ;
    return int(RandomNumber % RAND_MAX);
}

void myrand_rewind(){
  RandomNumber = 1234567;
}

double alea()
{return ((myrand()/((double)RAND_MAX+1.0)+myrand())/((double)RAND_MAX+1.0));}

int alea(int i){
	int r=(int)(floor(alea()*i));
	return (r==i?0:r);
}

double alea_norm()
{
 static bool flag=false;static double next;
 double x,y,norm,r;
 if(!flag){
	 do{
		 x=alea()*2.-1.;y=alea()*2.-1.;
		 norm=x*x+y*y;
	 }while(norm>=1.||norm==0.0);
	 r=sqrt(-2.*log(norm)/norm);
	 next=y*r;
	 flag=true;
	 return x*r;
 }else{
	 flag=false;return next;
 }
/* int i;const int n=100;double s=0;
 for(i=0;i<n;i++)s+=alea()-.5;
 return s*sqrt(12./n);*/
}


