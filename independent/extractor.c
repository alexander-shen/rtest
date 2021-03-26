#include "extractor.h"

#define BYTESINSAMPLE 2

int main (int argc, char* argv[]){
  if (argc!=3){
    printf ("Usage: %s file1 file2 (sends extracted bits to stdout)\n", argv[0]);
    exit(1); 
  }
  FILE *in1, *in2;
  in1= fopen(argv[1],"rb");
  if (in1==NULL){
    printf ("Cannot open file: %s\n",argv[1]);
    exit(1);
  }
  in2= fopen(argv[2],"rb");
  if (in2==NULL){
    printf ("Cannot open file: %s\n",argv[2]);
    fclose(in1);
    exit(1);
  }
  bool ok= true;
  while (ok){
    bitarr l,r,e;
    // read DEGREE cooked bits from both sources to l and r
    int numfilled=0; // number of already placed bits
    while (ok & (numfilled != DEGREE)){
      byte rawl[BYTESINSAMPLE], rawr[BYTESINSAMPLE];
      // put the next portion
      int read1=fread(rawl,sizeof(rawl),1,in1);
      int read2=fread(rawr,sizeof(rawr),1,in2);
      ok=(read1==1)&&(read2==1);
      if (ok) {
      // process raw bytes and put the cooked bits in l and r:
       byte cookedl = rawl[0]^rawl[1];
       byte cookedr = rawr[0]^rawr[1];
       for (int i=0; i<8; i++){
         assert(numfilled<DEGREE);
         b_set_bit(l,numfilled,cookedl%2); cookedl/=2;
         b_set_bit(r,numfilled,cookedr%2); cookedr/=2;
         numfilled++;
       }
      }
    }
    if (numfilled==DEGREE){
      // produce SIZE bits for output:
      b_extract(e,l,r); 
      // output these bits
      assert(SIZE%8==0);
      byte outbyte = 0;
      int i=0; // i bits are processed
      while(i<SIZE){
        outbyte=(outbyte<<1)+b_get_bit(e,i);
        i++;
        if (i%8==0){
          putchar(outbyte); outbyte= 0;
        }
      } 
    }
  }
  fclose(in1); fclose(in2);
  exit(0);
}

