// Lempel-Ziv test considers the bit string and split its into words: 
// each next words is the minimal prefix of the remaining tail that is
// not a prefix of one of the previous words. (So the first word is
// just one bit). The test output is the number of words created when
// we read n=param[3]*32 bits. (This test was included in the first 
// version of NIST and then excluded because of the problems found in
// Kim et al., but can be used safely for the robust setting (as any
// other function) If n=10^6 as NIST recommends, we convert the output
// to a presumably uniform distribution

#include "test_func.h"

bool lz_split (long double *value, unsigned long *hash, PRG gen, int *param, bool debug){
  assert (param[2]==1); // test dimension is 1 (the number of words returned)
  long m= param[3]; // the number of integers used; NIST uses 10^6 bits (31250 ints)
  		   // and the conversion parameters are valid only for this number
  assert (m>0); //
  long n= m*32; // number of bits  		   
  long maxvert= (n<1000000? 100000: n/10);
  //reserve space for 100000 words, this should be enough for 10^6 bits (NIST says 
  // the average number of words is about 70000 for 10^6 words, and the average length
  // of the word should increase with n;
  int sons[maxvert][2]; // numbers of left and right sons of a vertex
  int nil=-1; int root= 0;// 
  sons[0][0]= nil; sons[0][1]= nil;  
  int firstfree= 1; // first free slot for vertex = number of vertices including the empty one
  int curvert= root; // tree root 
  // tree that contains only root is created;
  // current incomplete word exists in the tree as verter curvert
  long processed= 0; // number of bits already processed
  bool free_space= true;
  while ((processed != n)&&free_space){
    bool b;
    if (!getbit(&b,gen)){return(false);}
    // new bit is read
    if (sons[curvert][b]!=nil){ // next bit still keeps us in the tree
      curvert=sons[curvert][b];
      if (debug){
        printf("%d",b);
      }
    }else if (firstfree<maxvert) { 
      // we have to create new vertex and there is space in the tree
      assert (sons[curvert][b]==nil);
      sons[curvert][b]=firstfree;
      sons[firstfree][0]= nil; sons[firstfree][1]= nil;
      firstfree++;
      if (debug){
         printf("%d\n",b);
      }
      // current word becomes empty:
      curvert= root;      
    }else{ // no free space in the tree
      free_space= false;
    }
    processed++;
  }
  // report the number of words 
  value[0]= firstfree-1; // number of completed words = number of vertices except the root
  if (n==1000000){// according to Kim-Umeno-Hasegawa
    double mean= 69588.2019; 
    double sigma= sqrt(73.23726011);
    if(debug) {printf("Theoretical mean: %lf, sigma: %lf\n", mean, sigma);}
    value[0]=gsl_cdf_ugaussian_P((value[0]-mean)/sigma);
  }
  if (debug){
    printf("For n=%ld the value is %Lf (for n=10^6 it is a converted value\n", n, value[0]);
  }
  // get hash
  unsigned int h1, h2;
  if ((!g_int32_lsb(&h1,gen))|| (!g_int32_lsb(&h2,gen))){return(false);}
  *hash = (((unsigned long) h2)<<32)+((unsigned long) h1);
  if (debug) {print64(*hash); printf("\n");} 
  return(true);
}
