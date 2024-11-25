#include "generators.h"

// #define MAX_NUM_PRG 30 
// maximal allowed number of different registered PRG (defined in .h file)
int num_PRG= 0; // the number of currently registered PRG, 0<=num_PRG <=MAX_NUM_PRG

// Generator types (from .h file):

// #define GEN_FILE 0
// #define GEN_XOR 1
// #define GEN_BAD  2
// #define GEN_OTHER 3
// #define GEN_YET_ANOTHER 4
// ...

// Data kept for each generator:

int gen_type[MAX_NUM_PRG];
// for i in [0,num_PRG):
// type of a generator 

unsigned long num_read [MAX_NUM_PRG]; 
// for i in [0,num_PRG):
// the number of bytes already taken from i-th generator


// Data kept for file-based generators:

FILE* source_files[MAX_NUM_PRG];
// for i in [0,num_PRG) source[i] is a pointer to the file
// used by the ith generator (if it reads from a file)

// Data kept for xor-based generator: arguments for xor
PRG arg1[MAX_NUM_PRG], arg2[MAX_NUM_PRG];

// Data kept for synthetic generators:

// index should be in [0,num_PRG):
unsigned long state[MAX_NUM_PRG]; 
// the current state of a synthetic generator
// assuming that 64 bits are enough; extra state array should be
// added for more advanced synthetic generators
unsigned int buf[MAX_NUM_PRG]; // buffers for generators that generate integers
int num_buf[MAX_NUM_PRG]; // number of remaining bytes in the buffer


// Create new generators:

// Create a generator that gets bits from the given file
// several generators can be attached to the same file; they open the file independently
// (and generate the same stream, each having its own position in the file)
// *new_generator= label; true returned if no error
bool g_create_file (PRG *new_generator, char *filename){
  assert (num_PRG < MAX_NUM_PRG); // enough generator labels
  int curr= num_PRG;
  FILE* f;
  f= fopen(filename,"rb"); 
  if (f==NULL) {return(false);}
  *new_generator= curr;
  source_files[curr]= f;
  gen_type[curr]= GEN_FILE;
  num_read[curr]= 0;
  num_PRG++;
  return (true);
}


// Create a generator of pseudo-random bits; init is an array of integers of size INIT_LENGTH;
// where init[0] is the type of the generator, and the rest of init[] is used as seed
// *new_generator= label; true returned if no error
// Only the dummy implementation (+1 as iteration)
bool g_create_synthetic (PRG *new_generator, int *init){
  assert (num_PRG<MAX_NUM_PRG); // enough generator labels
  int curr= num_PRG;
  switch (init[0]) {// first element of the array is generator type
    case GEN_BAD: {
      gen_type[curr]= GEN_BAD;
      state[curr]=init[1];
      num_buf[curr]= 0; // initially the buffer is empty 
      // no need to initialize buf[curr]
    } break;
    default: {
      assert (false);
    }     
  }
  *new_generator= curr;
  num_read[curr]= 0;
  num_PRG++;
  return(true);
}

// Create a generator that is XOR of two existing generators with labels gen1, gen2
// *new_generator= label; true returned if no error
// the existing generators remain available and the calls to them can be
// interleaved with call to XOR-generator and never use the same bits
bool g_create_xor (PRG *new_generator, PRG gen1, PRG gen2){
  assert (num_PRG<MAX_NUM_PRG); // enough generator labels
  int curr= num_PRG;
  *new_generator= curr;
  gen_type[curr]= GEN_XOR;
  arg1[curr]= gen1; arg2[curr]= gen2;
  num_read[curr]= 0;
  num_PRG++;
  return(true);  
}

// TODO: use some attached devices as generators, use transformed WAV files

// HOW TO ACCESS THE BIT STREAM

// for now the access unit is byte; one should think about optimization (longer pieces) and
// test/generators that deal with individual bits

// *b= the next byte from generator with label gen
bool g_byte (byt *b, PRG gen){
  assert (gen>=0); assert (gen<num_PRG);
  int type= gen_type[gen];
  switch(type){
    case GEN_FILE: {
      int c= getc(source_files[gen]);
      if (c==EOF){printf("oops, eof in generator %d\n", gen); return (false);}
      *b = (byt) c;
      num_read[gen]++;
      return (true);
    } break;
    case GEN_BAD: {  
      if (num_buf[gen]==0){
        buf[gen]= (unsigned int) state[gen];
        num_buf[gen]= 4; // number of used bytes in state
        state[gen]= state[gen]+1; // or any other iterative generator
      }
      assert(num_buf[gen]>0);
      *b = (byt) (buf[gen]%256);
      buf[gen]/= 256;
      num_buf[gen]--;
      num_read[gen]++;
      return(true);
    } break;
    case GEN_XOR: {
      byt b1,b2;
      if (!g_byte(&b1,arg1[gen])) {printf("oops XOR in arg1 %d\n",gen); return(false);}
      if (!g_byte(&b2,arg2[gen])) {printf("oops XOR in arg2 %d\n",gen); return(false);}
      *b= (byt) b1^b2;
      num_read[gen]++;
      return(true);
    } break;
    default:
      printf("Not implemented\n");
      exit(1);
  }
  assert (false); // this should not happen
}

// *i= 32-bit integer from input stream of generator gen (first byte + 256*second byte+...)
bool g_int32_lsb(unsigned int *i, PRG gen){
  byt b[4];
  for (int j=0; j<4; j++){
    if (!g_byte(&(b[j]),gen)) {printf("oops... no byte from gen %d\n", gen); return(false);} 
  }
  *i= (unsigned int) 0;
  for (int j=0; j<4; j++){
    *i= ((*i)<<8)+b[3-j]; 
  }
  return(true);
}

// *i= 16-bit integer from input stream of generator gen (first byte + 256*second byte)
bool g_int16_lsb(unsigned int *i, PRG gen){
  byt b[2];
  for (int j=0; j<2; j++){
    if (!g_byte(&(b[j]),gen)) {return(false);} 
  }
  *i= 0;
  for (int j=0; j<2; j++){
    *i= ((*i)<<8)+b[1-j]; 
  }
  return(true);
}

// b*= one bit from the statically bufferized stream of 32-bits integers, obtained from 
// byte stream of generator gen (least significant bit first), bits go in the 
// least-significant-first order
bool g_getbit(bool *b, PRG gen){
  static bool buf[32];
  static int nbuf= 0; // number of remaining bits (will be taken 
             // in buf[31]..buf[0] order
  if (nbuf==0){
    unsigned int next;
    if (!g_int32_lsb(&next,gen)){return(false);}
    for (int i=0; i<32; i++){
      buf[i]=next%2;
      next/=2;
    }
    nbuf= 32;  
  }
  nbuf--;
  *b = buf[nbuf];  
  return(true);
}

// get wlen<32 bits from gen and place them in the lsbits of w
bool g_getword(unsigned *w, int wlen, PRG gen){
  assert (wlen<32);
  static unsigned buf; // least significant nbuf bits in buf are kept
  static int nbuf=0;   // to be used in the next calls (in the same LR order)
  // variables are static, i.e., kept between function calls and initialized once
  *w= 0; 
  int missing= wlen; // number of missing bits in *w
  while(missing!=0){
    if (nbuf==0){ // read one full word in the buffer
      if (!g_int32_lsb(&buf,gen)){return(false);}
      nbuf= 32;
    }
    assert(nbuf>0);
    int moved= (nbuf<missing?nbuf:missing); // min(missing,nbug) to be moved
    assert (moved<32); // moving 32 bits left is not implemented!
    *w= ((*w)<<moved)|(buf>>(nbuf-moved));
    nbuf-=moved;
    missing-=moved;
    buf&=(((unsigned int)1)<<nbuf)-1;    
  }
  return(true);
}

// Access to statistics
unsigned long g_num_read (PRG gen){
  assert (gen>=0); assert (gen<num_PRG);
  return(num_read[gen]); 
}
