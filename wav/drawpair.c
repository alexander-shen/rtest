/*
Reads data from two files, interprets the data from both files as signed integers and draws a corresponding pair. 


*/

#include "drawpair.h"
#include "graph_pipe.h"

FILE *in1, *in2;

//options: -b num how many bytes should be used for one reading (1..4)
//         -l first byte is the least significant byte
//         -m first byte in the most significant byte
//         -c using pairs of consecutive numbers from one file
//         

// *next <- integer made on optb bytes from in, MSB first if optm otherwise LSB first
// returns true if succeeded
bool get_next (int *next, FILE *in, int optb, int optm){
  unsigned char buf[MAXBYTES]; // here the bytes that form the next integer are stored
  int num_read= 0;
  bool ok= true;
  while (ok && (num_read !=optb)){
    int next_char=getc(in);
    if (next_char==EOF){
      ok= false;
    }else{  
      buf[num_read]=(unsigned char) next_char;
      num_read++;
    }
  }
  if ((num_read==optb)&&ok) {
    // buf[0..optb) is filled and should be converted to int according to optm
    if (optm){
      int ext= (int) (char) buf[0]; // should take into account the sign extension
      assert (-128<=ext); assert (ext<=127);
      *next= ext; 
      for (int i=1;i<optb;i++){
        ext = (int) buf[i];
        assert (0<=ext); assert (ext<=255);
        *next=(*next)*256+ext;
      }
    }else{
      int ext= (int) (char) buf[optb-1]; // should take into account the sign extension
      assert (-128<=ext); assert (ext<=127);
      *next= ext; 
      for (int i=optb-2;i>=0;i--){
        ext = (int) buf[i];
        assert (0<=ext); assert (ext<=255);
        *next=(*next)*256+ext;
      }    
    }
    return(true);
  }else{
    return(false);
  }
}

#define CONVERTED_X(z) (int)( ((double)WIN_WIDTH-1)*((double)(z-low_limit))/((double)(high_limit-low_limit)))
#define CONVERTED_Y(z) (int)( ((double)WIN_HEIGHT-1)*((double)(z-low_limit))/((double)(high_limit-low_limit)))


// main program
void main_graph(int argc, char* argv[]) { 

  // read and remember the options
  opterr= 0;
  int c; 
  int optb=0; int optl=0; int optm=0; int optc= 0;
  while ((c= getopt(argc, argv,"b:lmc"))!=-1){
    switch(c){
    case 'b': 
      optb=atoi(optarg);
      break;
    case 'l': optl=1; break;
    case 'm': optm=1; break;    
    case 'c': optc=1; break;
    case '?':
      if (isprint(optopt)){
        printf("Error: option -%c invalid\n", optopt);                                                                                                               
        printf ("Usage: %s -b <numbytes> -l|-m [LSB.. or MSB..] file1 file2\n", argv[0]);
        printf ("or %s -b <numbytes> -l|-m [LSB.. or MSB..] -c file\n", argv[0]);
      }else{
        printf ("Unknown option character `\\x%x'.\n",optopt);
      }
      close(PIPEWRITE);
      exit(1);
    default: assert(false);
    }    
  }
  if ((optb<1)||(optb>4)){
    printf("Error: option -b should be used with argument in 1..4\n");
    close(PIPEWRITE); exit(1);
  }  
  if ((optm+optl!=1)&&(optb>1)){
    printf ("Error: exactly one of options -l (LSB first) or -m (MSB first) should be used if -b arg is not 1\n");
    close(PIPEWRITE); exit(1);
  }	
  if ((optm+optl!=0)&&(optb==1)){
    printf("Warning: -l or -m are irrelevant if -b argument is 1\n");
  }
  if ((optc==0)&&(argc!=optind+2)){
    printf ("Usage: %s -b <numbytes> -l|-m [LSB.. or MSB..] file1 file2\n", argv[0]);
    close(PIPEWRITE); exit(1);    
  }
  if ((optc==1)&&(argc!=optind+1)){
    printf("Usage: %s -b <numbytes> -l|-m [LSB.. or MSB..] -c file\n", argv[0]);
    close(PIPEWRITE); exit(1);
  }
  
  // opening file(s)
  in1= fopen(argv[optind], "rb");
  if (in1==NULL){
    printf("Error: cannot open file: %s\n", argv[optind]);
    close(PIPEWRITE); exit(1);
  }  
  if (optc==0){
    in2= fopen(argv[optind+1], "rb");
    if (in2==NULL){
      printf("Error: cannot open file: %s\n", argv[optind+1]);
      close(PIPEWRITE); exit(1);
    } 
  } 
  // minimal and maximal values for sound data
  long low_limit;
  long high_limit;
  switch (optb) {
    case 1:
      low_limit = -128; high_limit = 127; break;
    case 2:
      low_limit = -32768; high_limit = 32767; break;
    case 3:
      low_limit = -8388608; high_limit = 8388607; break;	
    case 4: 
      low_limit = -2147483648; high_limit = 2147483647; break;
    default: 
      assert(false); // this should not happen
  }
   
  // processing the file and displaying the data
  
  bool ok=true;
  int x,y;
  while (true){
    printf(".");
    ok=get_next(&x, in1, optb, optm);
    if (!ok) {break;}
    if (optc){// one file
      ok=get_next(&y, in1, optb, optm);
    } else {
      ok=get_next(&y, in2, optb, optm);
    }
    if (!ok) {break;}
    // two values ready for showing
    graph_draw(CONVERTED_X(x),CONVERTED_Y(y));
  }
  // no more pairs to show
  fclose(in1);
  if (optc==0){fclose(in2);}
  exit(0);
}
