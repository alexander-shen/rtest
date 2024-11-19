#include "general.h"

void print8(byt x){
  int bits[8];
  for (int i=0;i<8;i++){
    bits[i]=x%2;
    x= x>>1;
  }
  for (int i=0; i<8; i++){
    printf("%d",bits[7-i]);
  }
}

void print32 (unsigned u){
  int bits[32];
  for (int i=31; i>=0; i--){
    bits[i]=u%2;
    u=u>>1;
  }
  for (int i=0; i<32; i++){
    printf ("%d",bits[i]);
  }  
}

void print64 (long unsigned u){
  int bits[64];
  for (int i=63; i>=0; i--){
    bits[i]=u%2;
    u=u>>1;
  }
  for (int i=0; i<64; i++){
    printf ("%d",bits[i]);
  } 
}
