#include <stdio.h>

int main(int argc, char *argv[]){
  FILE *f;
  f= fopen("test.dat","wb");
  int i= (((int)1<<16)-1)<<16;
  fwrite(&i, sizeof(int), 1, f);
  i= ((int)1<<16)-1; 
  fwrite(&i, sizeof(int), 1, f);
  fclose(f);
}
