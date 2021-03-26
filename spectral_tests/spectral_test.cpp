#include "spectral_test.h"

#define MIN(a,b) ((a)<(b)?(a):(b))

void process_graphs (unsigned char (*read_random_source)(), 
                    long double gap, long double error, 
                    long size, long degree, long iterations, long number,
                    bool printlow, bool printup, bool printhash) {
                       
  //A random graph of average degree 2(degree/2)
  int d= degree/2; // integer division; d outgoing edges from each vertex
    
  // pointer to a function that reads a byte from a random source
  // unsigned char  (*read_random_source)();
    
  Graph* G;
    
  try {       
    for (long numgraph = 0; numgraph < number; numgraph++) { 
      G = new Graph(size);
      unsigned int hashvalue = (unsigned int)read_random_source()+256*(unsigned int)read_random_source(); 
      // random hash to break the ties
            
      if (size==65536){
        for (long i=0; i<size; i++){ // adding edges to ith vertex
          for (long k=0; k<d; k++) { // adding kth edge
            unsigned int j = ((unsigned int)read_random_source()+256*(unsigned int)read_random_source()); 
            G->AddSymEdge(i, j);
          }
        }  
      } else if (size==256) {
        for (long i=0; i<size; i++){ // adding edges to ith vertex
          for (long k=0; k<d; k++) { // adding kth edge
            unsigned int j = read_random_source(); 
            G->AddSymEdge(i, j);
          }
        }
      } else {
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
            // unsigned long j= next nb bits :
            int nj= 0; // number of bits already in j as least significant bits
            unsigned int j= 0;  // other bits in j are zeros
            while (true){
              assert (nj<=nb);
              if (nj==nb){ // all the required bits are already in j 
                break;
              } else if ((nj < nb) && (numbits==0)) {
                // more bits are needed but no bits in the buffer (unnsigned char buf)   
                buf = read_random_source(); numbits= 8; continue;
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
                unsigned char mask= ((unsigned int)1<<sft)-1; // mask contains sft 1's on the right
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
      long double low,high;
      G->SpecRad (gap, error, iterations, &low, &high);
      if (printlow){printf("%012.11Lf", low);}
      if (printlow){printf(" %012.11Lf", high);}
      if (printhash){printf(" %05i",  hashvalue);}
      printf("\n");
      fflush(stdout);
      delete G;
    }
  }
    
  catch (int e) {
    assert (e==EOF_EXCEPTION);
    printf("cannot read next byte from the random source: error %2i \n", e);
  }
}
