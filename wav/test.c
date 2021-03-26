
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>


// checking that int in memory is kept as LSB..MSB and has length 4 bytes
bool lsbmsb4(){
  int i=((5*256+7)*256+9)*256+1;
  if (sizeof(int)!=4){return(false);}
  unsigned char *bytei = (unsigned char*)&i;
  return ((bytei[0]==1) && (bytei[1]==9) && (bytei[2]==7) && (bytei[3]==5)); 
}

int main(int argc, char* argv[]){
 assert (lsbmsb4());
 printf("ok\n");
}
