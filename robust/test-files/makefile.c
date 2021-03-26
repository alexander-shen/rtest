#include <stdio.h>

int main(int argc, char *argv[]){
  FILE *f;
  f= fopen("test.dat","wb");
  for (int i=1; i<16; i++){
    fwrite(&i, sizeof(int), 1, f);
  } 
  fclose(f);
}
