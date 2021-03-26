#include "ks2.h"

/* for every sequence with n0 zeros and n1 ones we compute the deviation by taking the maximum of | k0/n0 - k1/n1 |, where k0 and k1 and numbers of zeros and ones in a prefix of a sequence, over all prefixes. The maximal value of deviation is 1 and it is a multiple of n0*n1, so it is given to the function as an integer (assuming n0*n1 as a denominator). We count the fraction of sequences with n0 zeros and n1 ones that have this or bigger deviation */

double ks2bar(int n0, int n1, long deviation_times_n0_n1){

/* Each sequence is a path on the integer grid going through (k0,k1) values for all prefixes. It starts at (0,0) and ends at (n0,n1). For a node (n,k) that does not exceed the given deviation we compute the number of paths that go to this node not exceeding the deviation (it could be 0). This is done by dynamic programming; only two diagonals are kept in the memory for optimization reasons */

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

  long *curr = (long *) malloc((n0l+1)*sizeof(long));
  long *new = (long *) malloc((n0l+1)*sizeof(long));
  long *currtot = (long *) malloc((n0l+1)*sizeof(long));
  long *newtot = (long *) malloc((n0l+1)*sizeof(long));
  long currsum=0; 
  curr[0l]= 1l; currtot[0l]= 1l;
  for (long i=1; i<=n0l; i++){curr[i]=0l; currtot[i]=0l;}
  /* the diagonal with sum currsum is filled with the number of paths in the allowed zone (for curr) and of all paths (for currtot)  */  
  while (currsum != n0l+n1l) {
    for (long i=0; i<=n0l; i++){
      if (i>currsum+1){
        new[i]=0l; newtot[i]=0l;
      }else{
        newtot[i]=currtot[i]+ (i>0 ? currtot[i-1] : 0);
        if (good(i,currsum+1-i)){
          new[i]=curr[i]+ (i>0 ? curr[i-1] : 0);
        }else{
          new[i]= 0l;
        }  
      }
    }
      /* new and newtot array are filled for diagonal currsum+1 */
    for (long i=0; i<=n0l; i++){curr[i]= new[i]; currtot[i]= newtot[i];}
    printf("currsum = %ld, n0l= %ld\n", currsum, n0l);
    for (int s=0; s<=n0l; s++){printf("%4ld ", newtot[s]);} printf("\n");
    currsum++;
  }
  assert (currsum == n0l+n1l);
  double ans = ((double) (currtot[n0l]-curr[n0l])) / ((double) currtot[n0l]);
  free(curr); free(new); free(currtot); free(newtot);
  return (ans);
}
