//abcdef data for testing

#include "generators.h"

PRG test;

void main(int argc, char *argv[]){
  assert(g_create_file (&test, "test-generators.c"));
  printf ("Byte reading\n");
  printf ("PRG label: %d\n", (int) test);
  for (int i=0; i<8; i++){
    byt b;
    assert (g_byte(&b, test));
    printf ("%3u ",(unsigned int)b);
  }
  printf("\n\n");
  
  assert(g_create_file (&test, "test-generators.c"));
  printf("Integer32 reading\n");
  printf ("PRG label: %d\n", (int) test);
  for (int i=0; i<2; i++){
    unsigned int in;
    assert (g_int32_lsb (&in, test));
    printf ("%10u = [",(unsigned int)in);
    byt buf[4];
    for (int j=3; j>=0; j--){
      buf[j]= in%256; in /=256;
    }
    for (int j=0; j<4; j++){
      printf("%3u ", (unsigned int)buf[j]);
    }  
    printf ("] (base 256)\n");
  }
  printf("\n");
  
  assert(g_create_file (&test, "test-generators.c"));
  printf("Integer16 reading\n");
  printf ("PRG label: %d\n", (int) test);
  for (int i=0; i<4; i++){
    unsigned int in;
    assert (g_int16_lsb (&in, test));
    printf ("%10u = [",(unsigned int)in);
    byt buf[2];
    for (int j=1; j>=0; j--){
      buf[j]= in%256; in /=256;
    }
    for (int j=0; j<2; j++){
      printf("%3u ", (unsigned int)buf[j]);
    }  
    printf ("] (base 256)\n");
  }
  printf("\n");  

  assert(g_create_file (&test, "test-generators.c"));
  printf("Bit reading\n");
  printf ("PRG label: %d\n", (int) test);
  for (int count=0; count<2; count++){
    for (int i=0; i<32; i++){
      bool b;
      assert (g_getbit (&b, test));
      if (i%8==0){printf(" ");}
      printf ("%1u",(unsigned int)b);
    }
    printf(" (base 2) ");
  }
  printf("\n\n");    
  
  assert(g_create_file (&test, "test-generators.c"));
  printf("Word reading\n");
  printf ("PRG label: %d\n", (int) test);
  // reading 9 blocks, each contains 7 bits 
  for (int count=0; count<9; count++){
    unsigned int wrd;
    g_getword (&wrd, 7, test);
    byt buf[7];
    for (int i=6; i>=0; i--){
      buf[i]= wrd%2;
      wrd /= 2;
    } // buf[0..6] are the bits from left to right  
    for (int i=0; i<7; i++){
      printf ("%1u",(unsigned int)buf[i]);
    }
    printf(" + ");
  }
  printf("\n");      
}
