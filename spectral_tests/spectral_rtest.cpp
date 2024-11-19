// adaptation for spectral_test.cpp for inclusion into robust test suite

#include "spectral_rtest.h"


#define MIN(a,b) ((a)<(b)?(a):(b))

bool process_graph (bytegen r, 
                    long double gap, long double error, 
                    long size, long degree, long iterations,
                    long double *lower, long double *upper) {
  
  // bool r(byt *b) puts random byte in b and returns True,
  // or gives up and returns False
                       
  //A random graph of average degree 2(degree/2)
  int d= degree/2; // integer division; d outgoing edges from each vertex
 
   
    
  Graph* G;
    
  G = new Graph(size);
  
  // create a random graph:          
  if (size==65536){
    for (long i=0; i<size; i++){ // adding edges to ith vertex
      for (long k=0; k<d; k++) { // adding kth edge
        byt b1, b2;
        if (!r(&b1)){delete G; return(false);} if (!r(&b2)){delete G; return(false);}
        unsigned int j = ((unsigned int)b1)+256*((unsigned int)b2); 
        G->AddSymEdge(i, j);
      }
    }  
  } else if (size==256) {
    for (long i=0; i<size; i++){ // adding edges to ith vertex
      for (long k=0; k<d; k++) { // adding kth edge
        byt b;
        if (!r(&b)){delete G; return(false);} 
        unsigned int j = b; 
        G->AddSymEdge(i, j);
      }
    }
  } else {// using input as a sequence of bits
    int nb= 0; long powernb= 1;
    while (powernb < size) {nb++; powernb*=2;}
    // nb is the minimal natural number such that 2^nb=powernb>=size
    // so we need to use nb bits from the input sequence to specify the endpoint of each edge
    assert (nb<=32); 
    unsigned char buf= 0; // for keeping the unused portion of the previous byte
    int numbits=0; 
    // number of bits left in the buf [kept as least significant bits of buf; others bits are zeros]
    for (long i=0; i<size; i++){ // adding edges to ith vertex
      for (long k=0; k<d; k++) { // adding kth edge
        // unsigned int j= next nb bits :
        int nj= 0; // number of bits already in j as least significant bits
        unsigned int j= 0;  // other bits in j are zeros
        while (true){
          assert (nj<=nb);
          if (nj==nb){ // all the required bits are already in j 
            break;
          } else if ((nj < nb) && (numbits==0)) {
            // more bits are needed but no bits in the buffer (unnsigned char buf)  
            byt b; 
            if (!r(&b)){delete G; return(false);} 
            buf = b; numbits= 8; continue;
          } else { 
            assert((nj < nb) && (numbits>0));
            unsigned char portion = MIN(numbits,nb-nj);
            // we need to move portion bits from the buffer to j 
            // moving the existing bits of j to the left
            assert ((portion > 0)&&(portion < sizeof(j)*8)); // full shift is not guaranteed to work
            j= j << portion; // moved part
            // now we need to take portion bits from the left of buf 
            // and move them to the least significant bits of j
            unsigned int sft= numbits-portion; 
            // right shift for the bits to take = number of bits that will remain in the buf
            assert ((sft>=0)&&(sft<sizeof(buf)*8));
            j= j | (buf>>sft); // j is ready now
            byt mask= ((unsigned int)1<<sft)-1; // mask contains sft 1's on the right
            buf=buf & mask; // deleting the moved bits
            numbits= sft;
            nj+= portion;
            continue;
          }
        }
        // making a random edge from i. 
        // NB: if size is not a power of two, then the edge is still valid,
        // but the distribution is not uniform
        G->AddSymEdge(i,j%size);
      }
    }
  }
  myrand_rewind(); // all estimates for the spectral radius should use the same pseudo-random bits
  G->SpecRad (gap, error, iterations, lower, upper);
  delete G;
  return(true);
}
