#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

// usage: entdep [-m mask] file1 file2: take pair of byted from file1 and file2 (while they exist)
// and compute the entropy for the distribution of pairs and for marginal distributions 
// mask should be a string of zeros and ones. The length of the mask is the number of bytes that form
// each sample. The bits determine how the byte is generated from the sample. 
// For example, mask 0001 means that we read four bytes (from each file) 
// and form the pair of the last bytes from the group. Mask 11 means that we take xor of two 
// consecutive bytes (from each file), etc.


#define N 65536
#define N1 256
#define N2 256
#define MAXLEN 64

// read masklen bytes from file in, combine them using mask intmask
// (by xoring the bytes where intmask[]==1 and ignoring the bytes where intmask[]==0)
// put the result into *nextbyte and return true if ok, otherwise return false;

bool get_reading(int *next_byte, FILE *in, int masklen, int* intmask){
  unsigned char result;
  result= (unsigned char) 0;
  for (int i=0; i<masklen; i++){
    int next_char=getc(in);
    if (next_char==EOF){return(false);}
    assert (0<=next_char); assert (next_char<256);
    if (intmask[i]==1){
      result^=(unsigned char)next_char;
    }
  }
  *next_byte= result; 
  return(true);
} 

int main(int argc, char *argv[]){

  // read and remember the options
  opterr= 0;
  int c; 
  char* mask;
  mask = "1";
  while ((c= getopt(argc, argv,"m:"))!=-1){
    switch(c){
    case 'm': 
      mask=optarg;
      break;
    case '?':
      if (isprint(optopt)){
        printf("Error: option -%c invalid\n", optopt);                                                                                                               
        printf ("Usage: %s [-m mask] file1 file2\n", argv[0]);
      }else{
        printf ("Unknown option character `\\x%x'.\n",optopt);
      }
      exit(1);
    default: assert(false);
    }    
  }
  if (argc!=optind+2){
    printf("Usage: %s [-m mask] file1 file2\n", argv[0]);
    exit(1);
  }

  // mask analysis: length and the vector of coefficients
  int intmask[MAXLEN]; // vector of zeros and ones
  int masklen= strlen(mask);
  assert (masklen>=1); assert (masklen<=MAXLEN);
  for (int i=0;i<masklen;i++){
    if (mask[i]=='0'){
      intmask[i]= 0;
    }else if (mask[i]=='1'){
      intmask[i]= 1;
    }else{
      printf("Error: mask should be a 0-1-string, not [%s]\n", mask);
      exit(1);
    }    
  }
  // mask analyzed
  
  // now opening files:
  FILE *in1, *in2;
  in1= fopen(argv[optind],"rb");
  if (in1==NULL){
    printf("Error: cannot read file: %s\n",argv[optind]);
    exit(1);
  }
  in2= fopen(argv[optind+1],"rb");
  if (in2==NULL){
    printf("Error: cannot read file: %s\n",argv[optind+1]);
    exit(1);
  }   
  // files opened


  int count [N]; // how many times each pair (combined int16) appears 
  long count1[N1]; long count2[N2]; // counters for marginal distributions 

  for (int i=0; i<N; i++){count [i]=0;}
  for (int i=0; i<N1; i++){count1[i]=0;}
  for (int i=0; i<N2; i++){count2[i]=0;}
  
  long long numint = 0;
  while(true){
    int c1, c2;
    if (!get_reading(&c1,in1,masklen,intmask)){break;}
    if (!get_reading(&c2,in2,masklen,intmask)){break;}
    count1[c1]++;
    count2[c2]++;     
    count[256*c1+c2]++;
    numint++;    
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
  double ent1= 0.0;
  for (int i=0; i<N1; i++){
    if (count1[i]>0){
       assert (numint>0);
       double freq = ((double) count1[i])/((double) numint);
       double entadd = freq * (logl(1.0/freq)/logl(2.0));
       ent1 +=entadd;
    }
  }
  double ent2= 0.0;
  for (int i=0; i<N2; i++){
    if (count2[i]>0){
       assert (numint>0);
       double freq = ((double) count2[i])/((double) numint);
       double entadd = freq * (logl(1.0/freq)/logl(2.0));
       ent2 +=entadd;
    }
  }
  
  printf ("Number of pairs: %lld\n", numint);
  printf ("pair entropy = %11.10lf\n", ent);
  printf ("byte entropies = %11.10lf, %11.10lf\n", ent1, ent2);
  printf ("mutual information = %11.10lf\n",ent1+ent2-ent);
  exit(0);
}

