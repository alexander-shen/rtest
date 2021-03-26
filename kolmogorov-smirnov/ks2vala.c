// using double real approximate version of inductive computation
#include "ks2.h"

int main(int argc, char* argv[]){
  if (argc!=1){
    printf("Usage: %s < filename_of_a_string_with_two_characters\n", argv[0]);
    exit(1);
  }
  #define MAXSTRINGLEN 1000000 
  char let[256];
  long count[256];
  int numlet=0;
  // read the argument string, put the letters in let[numlet] and their number of appearences in count[numlet]
  int next;
  long length = 0;
  char* str = malloc(MAXSTRINGLEN*sizeof(char));
  if (str == NULL){printf("Not enough memory\n"); exit(1);}
  while ((next= getchar())!=EOF){
    char nextchar = (char) next;
    //find or add the letter nextchar and increase the count
    if ((nextchar !='\n') && (nextchar != ' ') && (nextchar != '\t')){
      int j=0;
      while ((j<numlet)&&(let[j]!=nextchar)){j++;}
      if ((j<numlet)&&(let[j]==nextchar)){
        count[j]++;
      }else if (j==numlet){
        numlet++;
        assert (numlet<256);
        let[j]= nextchar;
        count[j]= 1;
      }else{
        assert(false);
      }
      assert (length < MAXSTRINGLEN);
      str[length]= nextchar;
      length++;
    }
  }
  printf("Profile:\n");
  printf("  length: %ld\n",length);
  for (int i=0; i<numlet; i++){
     printf("  character [%c] appears [%ld] times\n", let[i], count[i]);
  }
  if ((numlet==2) && (count[0]>0) && (count[1]>0)){
    printf("File suitable for KStest\n");
    long n0=count[0]; long n1=count[1];
    long pref= 0;
    long max_dev= 0;
    long k0=0;
    long k1=0;
    // max_dev = n0*n1*(maximal deviation for prefixes up to length pref)
    // k0, k1 = number of let[0], let[1] among first pref letters
    while (pref!=length){
      if (str[pref]==let[0]){
        k0++;
      }else if(str[pref]==let[1]){
        k1++;
      }else{
        assert(false);
      }  
      pref++;
      if (abs(k0*n1-k1*n0)>max_dev){max_dev=abs(k0*n1-k1*n0);}
    }
    double deviation=(double)max_dev/(((double)n0)*((double)n1));
    printf("Maximal deviation: %f\n", (float) deviation);
    printf("KSvalue: %20.15Lf\n",1-psmirnov2x(deviation,(int) n0, (int) n1));
  }else{
    assert(numlet!=2);
    printf("Error: input string should contain exactly two letters\n");
  }
  free(str);
  exit(0);
}
