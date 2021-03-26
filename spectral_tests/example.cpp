//Compilation: g++ test.cpp spectral_test.cpp specgraph.cpp rand.cpp

#include "spectral_test.h"
using namespace std;

#define DEFAULT_GAP 0.1
#define MIN_GAP 0.00001
#define DEFAULT_ERROR 0.0001
#define MIN_ERROR 0.00001
#define DEFAULT_SIZE 65536
#define MAX_SIZE 131072
#define DEFAULT_DEGREE 8
#define MAX_DEGREE 1024
#define DEFAULT_ITERATIONS 100000
#define DEFAULT_NUMBER 500


/*
 
 This is a simplistic playground of the spectral_test.cpp that estimates spctral radii for
 graphs from a source of "random" bytes. It reads the file named in the command line and 
 puts the information about the spectral parameters to the stdout
 
 */

char *filename;
FILE *fp = NULL;

// a function that reads bytes from a file
// it is used as function that provides bytes from a "random source"
// it throws the exception 10 when the end of the file is reached
unsigned char get_byte() {
    if ( fp == NULL) {
        fp = fopen(filename, "rb");
    }
    int b = getc(fp);
    if (feof(fp)) { 
        assert (b==EOF); // no more characters in the file
        fclose(fp);
        throw EOF_EXCEPTION; // exception: cannot read another byte, the file seems to be over
    }
    return b;
}


/* options: 
  -g gap (float)  [specgraph_stopthreshold=0.1; difference between low and upper bounds for spectral radius]
  -e error (float) [specgraph_probaerror=0.0001; probability of get wrong bounds]
  -s graph_size (int) [n = 1024*64 ; number of vertices; 
     should be a power of 2 not exceeding 65536 to get uniform distribution for edges]
  -d degree (int)     [const int d=4; was half-degree, not degree, so default was actually 8]
  -i iterations (int) [specgraph_maxiter=100000]
  -n number (int) [number of graphs to produce and analyze]
  -l print lower bound
  -u print upper bound
  -h print hash 
*/  

int main (int argc, char* argv[])
{
  opterr= 0; errno= 0;
  
  // setting default values
  long double gap= DEFAULT_GAP;
  long double error= DEFAULT_ERROR;
  long size= DEFAULT_SIZE;
  long degree= DEFAULT_DEGREE;
  long iterations= DEFAULT_ITERATIONS;
  long number= DEFAULT_NUMBER;
  bool printlow= false;
  bool printup= false;
  bool printhash= false;
  bool verbose= false;
  
  // processing options
  int c;
  while ((c=getopt(argc, argv, "g:e:s:d:i:n:luhv"))!=-1){
    switch(c){
    case 'g': 
      gap=strtold(optarg,NULL);
      if ((gap==0.0L)||(errno==ERANGE)){printf("Invalid -g (gap) option\n"); exit(1);}
      if (gap<MIN_GAP){printf("Gap (-g) should be at least %f\n", MIN_GAP); exit(1);}
      break;
    case 'e': 
      error=strtold(optarg,NULL);
      if ((error==0.0L)||(errno==ERANGE)){printf("Invalid -e (error probability) option\n"); exit(1);}
      if (error<MIN_ERROR){printf("Error (-e) should be at least %f\n", MIN_ERROR); exit(1);}
      break;      
    case 's':
      size= strtol(optarg,NULL,10);
      if ((errno==EINVAL)||(errno==ERANGE)||(size<=0)||(size>MAX_SIZE)){
        printf("Invalid graph size option; recommended values are powers of 2 not exceeding 65536\n"); exit(1);  
      }
      break;
    case 'd':
      degree= strtol(optarg,NULL,10);
      if ((errno==EINVAL)||(errno==ERANGE)||(degree<=0)||(degree>MAX_DEGREE)){
        printf("Invalid -d (degree) option. Maximal value: %d\n",MAX_DEGREE); exit(1);  
      }
      break;
    case 'i':
      iterations= strtol(optarg,NULL,10);
      if ((errno==EINVAL)||(errno==ERANGE)||(iterations<=0)){
        printf("Invalid -i (iterations) option\n"); exit(1);  
      }
      break;      
    case 'n':
      number= strtol(optarg,NULL,10);
      if ((errno==EINVAL)||(errno==ERANGE)||(number < 1)){
        printf("Invalid -n (number of graphs) option\n"); exit(1);  
      }
      break;      
    case 'l':
      printlow= true;
      break;
    case 'u':
      printup= true;
      break;
    case 'h':
      printhash= true;
      break; 
    case 'v':
      verbose= true;
      break;
    case '?':
      if (isprint(optopt)){
        printf("Error: option -%c invalid\n", optopt);
        printf ("Usage: %s [-e error -s size -d degree -n maximal_number_of_graphs -l -u -h -v]  filename\n", argv[0]);
        printf ("-l / -u / -h : print lower bound / upper bound / hash value\n");
        printf ("-v : print the parameter values\n");
      }else{
        printf ("Unknown option character `\\x%x'.\n",optopt);
      }
      exit(1); 
    }
  }
  if ((optind!=argc-1)){
    printf ("Usage: %s [-g gap -e error -s size -d degree -o iterations -n number_of_graphs -l -u -h] filename\n", argv[0]);
    printf ("-l / -u / -h : print lower bound / upper bound / hash value\n");
    exit(1);
  }
  if (!printlow && !printup && !printhash){
    printf("You should include at least one of -l -u -h options\n");
    exit(1);
  }
  
  filename=argv[optind];
  if(verbose){ 
    printf("Parameters:\n");
    printf("gap %Lf\n", gap);
    printf("error %Lf\n", error);
    printf("size %ld\n", size);
    printf("degree %ld\n", degree);
    printf("iterations %ld\n", iterations);
    printf("number %ld\n", number);
    printf("lower bound: %s\n", (printlow? "yes":"no"));
    printf("upper bound: %s\n", (printup? "yes":"no"));
    printf("hash: %s\n", (printhash? "yes":"no"));
  }
 
  process_graphs(&get_byte, gap, error, size, degree, iterations, number, printlow, printup, printhash);
  /* 
  defined in spectral_test.cpp;
  create random graphs with given parameters using calls to get_byte to get random bytes;
  at most max_number of graphs are created, if not enough bytes, an error message is added
  boolean values printlow, printup, printhash are used to request printing the lower bound,
  the upper bound, and the hash value from the file (to make lines more unique)
  */
  return 0;
}
