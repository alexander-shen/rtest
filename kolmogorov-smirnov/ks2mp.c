#include "ks2.h"
// requires -lgmp while compiling

/* for every sequence with n0 zeros and n1 ones we compute the deviation by taking the maximum of | k0/n0 - k1/n1 |, where k0 and k1 and numbers of zeros and ones in a prefix of a sequence, over all prefixes. The maximal value of deviation is 1 and it is a multiple of n0*n1, so it is given to the function as an integer (assuming n0*n1 as a denominator). We count the fraction of sequences with n0 zeros and n1 ones that have this or bigger deviation */

// -------------------------------------------------------------------------------
// https://gmplib.org/gmp-man-6.2.1.pdf
// mpz_t is a type for Multi Precision Z [integers]
// ----- creation / destruction -----
// mpz_init (mpz_t n) : initialize n
// mpz_clear (mpz_t n) : clear the memory related to n
// mpz_set (mpz_t a,b) : a:=b
// mpz_set_si(mpz_t a, long int b): a:=b
// ----- conversion -----
// mpz_set_si (mpz_t destination, long int source) : copying long integers to Z
// double mpz_get_d (mpz_t n) : returns double representation of n when possible
// ----- arithmetic -----
// mpz_add (mpz_t a,b,c) : a:=b+c
// mpz_add_ui (mpz_t a, b, unsigned long c): a:=b+c
// mpz_sub (mpz_t a,b,c) : a:=b-c
// mpz_sub_ui (mpz_t a, unsigned long b, mpz_t c): a:=b+c
// mpz_mul (mpz_t a, b, c) : a:= b*c
// mpz_mul_si (mpz_t a, b, long c) : a:= b*c
// mpz_neg (mpz_t a,b): a:=-b
// mpz_abs (mpz_t a,b): a:=|b|
// mpz_fdiv_qr (mpz q, r, n, d): q,r = n div d, n mod d
// int mpz_cmp (a,b): returns an integer of the same sign as a-b
// size_t mpz_out_str(FILE *stream, int b, mpz_t x): prints x in base b to stream
/* example for large arithmetic ---
int main(int argc, char * argv[]){
  mpz_t factorial;
  mpz_init (factorial);
  mpz_set_si (factorial, 1);
  for (int i=2; i<=6; i++){
    mpz_mul_si(factorial, factorial, i);
  }
  int nbytes= mpz_out_str(stdout,10,factorial);
  printf("\n");
  mpz_clear (factorial);
}
*/
// ------------------------------------------------------------------------------


double ks2bar(int n0, int n1, long deviation_times_n0_n1){
  //printf("n0=%d, n1=%d, deviation_times_n0_n_1=%ld\n", n0,n1, deviation_times_n0_n1);

/* Each sequence is a path on the integer grid going through (k0,k1) values for all prefixes. It starts at (0,0) and ends at (n0,n1). For a node (n,k) that does not exceed the given deviation we compute the number of paths that go to this node not being all the time smaller than deviation (it could be 0). This is done by dynamic programming; only two diagonals are kept in the memory for optimization reasons */

  long n0l=n0; long n1l=n1;
  #define good(k0,k1) (abs((k0)*n1l-(k1)*n0l)<deviation_times_n0_n1)
  assert(n0l>0); assert(n1l>0); 
  assert (deviation_times_n0_n1 >=0); assert (deviation_times_n0_n1<=n0l*n1l);
    if (deviation_times_n0_n1==0){return((double) 1.0);} 
  /* all paths have this or bigger deviation */ 
  assert(good(0,0));
  assert(good(n0,n1));


  /* the nodes are indexed by kt=k0+k1, the length of the prefix, and k0 (we could also use k1). Two arrays of length n0+1 of long integers are therefore needed (for old and new diagonals). Similar arrays are used for binomial coefficients, for the total number of paths */

  if (deviation_times_n0_n1==0){return((double) 1.0);} 
  /* all paths have this or bigger deviation */

  mpz_t curr[n0l+1], nw[n0l+1], currtot[n0l+1], newtot[n0l+1] ; 
  for (long i=0; i<=n0l; i++){
    mpz_init(curr[i]); mpz_set_si(curr[i],0l);  
    mpz_init(currtot[i]); mpz_set_si(currtot[i],0l); 
    mpz_init(nw[i]); mpz_set_si(nw[i],0l); 
    mpz_init(newtot[i]);  mpz_set_si(newtot[i],0l); 
  } 
  // arrays of length n0+1 are fully initialized
  // for all the diagonals only parts when k0=0,1,2,...,n0 is needed
  
  // curr[0l]= 1l; currtot[0l]= 1l;
  mpz_set_si(curr[0],1); mpz_set_si(currtot[0],1);
  long currsum=0; // the sum of coordinates for the diagonal where we perform the computation
 
  // for the the diagonal with sum currsum  the number of paths to (k0,k1) 
  // in the allowed zone is in curr[k0] and of all paths is in currtot[k0]  
  while (currsum != n0l+n1l) {
    for (long i=0; i<=n0l; i++){
      if (i>currsum+1){ // positions outside the actual diagonal
        mpz_set_si (nw[i],0l); 
        mpz_set_si (newtot[i],0l);
      }else{ // newtot[i]=currtot[i]+ (i>0 ? currtot[i-1] : 0);
        if (i==0){
          mpz_set(newtot[i],currtot[i]); // in fact newtot[i]=currtot[i]=1
        }else{
          mpz_add(newtot[i],currtot[i],currtot[i-1]);
        } // this is just Pascal's triangle
        if (good(i,currsum+1-i)){ // nw[i]=curr[i]+ (i>0 ? curr[i-1] : 0);
          if (i==0){
            mpz_set(nw[i],curr[i]);
          }else{ //i>0   
            mpz_add (nw[i],curr[i],curr[i-1]);
          }
        }else{ // nw[i]= 0l;      
           mpz_set_si (nw[i],0l);
        }  
      }
    }
      /* nw and newtot array are filled for diagonal currsum+1 */
    for (long i=0; i<=n0l; i++){ // curr[i]= nw[i]; currtot[i]= newtot[i];}
      mpz_set(curr[i],nw[i]);
      mpz_set(currtot[i],newtot[i]);
    }
    currsum++;
  }
  assert (currsum == n0l+n1l);

 
  //double ans = ((double) (currtot[n0l]-curr[n0l])) / ((double) currtot[n0l]);
  mpz_t numerator;
  mpz_init(numerator);
  mpz_t denominator;
  mpz_init(denominator);
  mpz_sub(numerator,currtot[n0l],curr[n0l]);
  mpz_set(denominator,currtot[n0l]);
  // double ans = (mpz_get_d (numerator))/(mpz_get_d(denominator));
  // dealing with longer sequences needs adjusting numerator and denominator before
  // converting to double
  mpz_t two128; mpz_init(two128); mpz_set_si(two128,1); mpz_mul_2exp(two128,two128,128);
  while (mpz_cmp(denominator,two128)>0){ // denominator too large for conversion
    mpz_fdiv_q_2exp (numerator,numerator,2);
    mpz_fdiv_q_2exp (denominator,denominator,2);
  } 
  double ans = (mpz_get_d (numerator))/(mpz_get_d(denominator));
 
  mpz_clear(numerator);
  mpz_clear(denominator);
  mpz_clear(two128);
  
  // free(curr); free(nw); free(currtot); free(newtot); 
  for (long i=0; i<=n0l;i++){
    mpz_clear(curr[i]);
    mpz_clear(nw[i]);
    mpz_clear(currtot[i]);
    mpz_clear(newtot[i]);
  }
  
  return (ans);
}

// sites that provide similar data http://www.physics.csbsju.edu/stats/KS-test.n.plot_form.html
// https://scistatcalc.blogspot.com/2013/11/kolmogorov-smirnov-test-calculator.html?m=0


