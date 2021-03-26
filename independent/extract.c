#include "extract.h"

void b_set_zero (bitarr b){
  for (int i=0; i<ARRLEN; i++){b[i]= (word)0;}
}

void b_copy(bitarr b, bitarr c){ // b<-c
  for (int i=0; i<ARRLEN; i++){b[i]= c[i];}
}

void b_set_bit(bitarr b, int numbit, bool value){
  int numel= numbit/(8*sizeof(word)); int numloc= numbit%(8*sizeof(word));
  word mask = ((word)1)<<numloc;
  b[numel]&= ~mask;
  if (value){b[numel]|= mask;}  
}

bool b_get_bit(bitarr b, int numbit){
  int numel= numbit/(8*sizeof(word)); int numloc= numbit%(8*sizeof(word));
  word mask = ((word)1)<<numloc;
  if (b[numel]&mask){return(1);}else{return(0);}
}

bool b_xor (bitarr x, bitarr a, bitarr b){ // x <- a bitwise-xor b
  for (int i=0; i<ARRLEN; i++){x[i]= a[i]^b[i];}
}

bool b_or (bitarr x, bitarr a, bitarr b){ // x <- a bitwise-or b
  for (int i=0; i<ARRLEN; i++){x[i]= a[i]|b[i];}
}

bool b_and (bitarr x, bitarr a, bitarr b){ // x <- a bitwise-and b
  for (int i=0; i<ARRLEN; i++){x[i]= a[i]&b[i];}
}

bool b_not (bitarr x, bitarr a){ // x <- bitwise-not b
  for (int i=0; i<ARRLEN; i++){x[i]= ~a[i];}
}

int b_count(bitarr x){ // count the number of ones in x
  int count=0;
  assert (sizeof(word)==4);
  for (int i=0; i<ARRLEN; i++){count+= __builtin_popcount(x[i]);}
  return(count);
}

bool b_prod(bitarr x, bitarr y){ // scalar product in the sense of Z_2
  bitarr c;
  b_and(c,x,y);
  return (b_count(c)%2);
}

void b_print(bitarr b){ // print b[0] b[1]... (for debugging)
  for (int i=0; i<ARRLEN; i++){
    //print binary representation of b[i] from lsbit to msbit
    word x=b[i];
    for (int j=0; j<sizeof(word)*8;j++){
       printf("%d",x%2); x/=2;
    }
    if (i%4==3){
      printf("\n");
    }else{
      printf(" ");
    }  
  }
  printf("\n");
}

void b_rshift (bitarr c, bitarr b){ // c<- b shifted one bit to the right
                                    // it is OK if b and c are the same
  static word mask= ((word)1)<<(sizeof(word)*8-1);
  // c[i]=b[i-1] for all i>0; c[0]=0
  // corresponds to _left_ shifts in all words, the msbit of b[i-1] is moved to c[i]
  bool oldmsb = false;
  bool msb;
  for (int i=0; i<ARRLEN; i++){
    msb = ((mask&b[i])!=0);
    c[i]=b[i]<<1;
    c[i]|=oldmsb;
    oldmsb=msb;
  }
}

// polynomial operations:

bool b_correct(bitarr b){ // check that only first DEGREE bits are non-zeros
  for (int i=DEGREE;i<8*sizeof(word)*ARRLEN;i++){
    if (b_get_bit(b,i)!=0) {return 0;}
  }
  return(1);
}

void b_multx (bitarr c, bitarr b){ // c <- bx mod p; it is OK if b and c are the same
  assert(b_correct(b));
  bool correction= b_get_bit(b,DEGREE-1);
  b_set_bit(b,DEGREE-1,0);
  b_rshift(c,b);
  if (correction){
    c[0]^=CORRECTION;
  }
  assert(b_correct(c));
}

void b_mult (bitarr a, bitarr b, bitarr c){// a = b*c mod p; a should NOT be among b,c
  // but b and c can be the same array
  assert (a!=b); assert(a!=c);
  b_set_zero(a);
  assert(b_correct(b));
  assert(b_correct(c));
  bitarr tmp;
  b_copy (tmp, c);
  for (int i=0;i<DEGREE;i++){
    // tmp= c*x^i; if b[i]=1 add c to a; multiply tmp by x
    if (b_get_bit(b,i)) {b_xor(a,a,tmp);}
    b_multx(tmp,tmp);
  }
  assert(b_correct(a));
}

bool b_equalp (bitarr a, bitarr b){ // comparisons of first DEGREE terms. others should be zeros
  bool equal = true;
  for (int i=0; i<DEGREE; i++){
    equal&=(b_get_bit(a,i)==b_get_bit(b,i));
  }
  //checking that other bits are zeros
  for (int i=DEGREE;i<8*sizeof(word)*ARRLEN;i++){
    assert (b_get_bit(a,i)==0); assert(b_get_bit(b,i)==0);
  }
  return(equal);
}

void b_extract(bitarr r, bitarr a, bitarr b){
  bitarr tmp;
  b_set_zero(r);
  b_copy(tmp,a);
  for (int i=0; i<SIZE; i++){
    b_set_bit(r,i,b_prod(tmp,b));
    b_multx(tmp,tmp);
  }
}

/*
// Test for polynomial operations
int main(int argc, char * argv[]){
  for (int i=0; i<65536; i++){
    printf("i=%d\n",i);
    bitarr a,b,tmp;
    b_set_zero(a);
    assert(b_correct(a));
    a[0]= (word) i; // trying all possible combination of 16 bits
    b_copy(b,a);
    // compute b^(2^16); it should be equal to b according to Fermat theorem
    for (int j=0;j<DEGREE;j++){
      assert(b_correct(b));
      // b<- b*b
      bitarr tmp;
      b_mult (tmp,b,b);
      b_copy (b,tmp);
    }
    if (!b_equalp(a,b)){printf("a=%d,b=%d\n",a[0],b[0]);}
  }
  printf ("Test completed\n");
}
*/
