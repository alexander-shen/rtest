#include <stdio.h>

int main(int argc, char *argv[]){
  FILE *f;
  f= fopen("test.dat","wb");
  int i= 0x54a700f0;
  fwrite(&i, sizeof(int), 1, f);
  i= 0x1ff10ff0;
  fwrite(&i, sizeof(int), 1, f);
  fclose(f);
}
