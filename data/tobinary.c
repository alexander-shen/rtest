#include <stdio.h>
#include <assert.h>

int main (int argc, char *argv[]){
  int c;
  int count=0;
  unsigned int curr= 0;
  int currc= 0;
  while ((c=getchar())!=EOF){
    char cc = c;
    if (cc=='0'){
      curr= (curr<<1);
      currc++;
      count++;
    } else if (cc=='1'){
      curr= (curr<<1)+1;
      currc++;
      count++;
    } else {
      assert ((cc==' ')||(cc=='\n')||(cc=='\r'));
    }
    if (currc==32){
      //output curr to stdout (four bytes in LSB order)
      for (int i=0; i<4; i++){
        putchar(curr%256);
        curr/=256;
      }
      assert (curr==0);
      currc= 0;
    }
  }
  fprintf(stderr, "count=%d\n",count);
}
