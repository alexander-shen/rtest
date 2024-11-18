#include "test_func.h"
#include "../kolmogorov-smirnov/ksmirnov.h" // psmirnov2x (approximate KS-computation)
#include "../kolmogorov-smirnov/ks2.h" // ks2bar (exact KS-computation)

// recall the definition from test_func.h:
// typedef bool test_func(long double *value, unsigned long *hash, 
//                        PRG gen, int *param, double *real_param, bool debug);

extern test_function functions_list[]; // defined in test_func.c
extern int len_func_list;              // defined in test_func.c

// qsort applied to an array of integers [0..len) but the comparison function works
// diferrently and uses some global variables:
long double *v; unsigned long int *h; // pointers to the arrays of values and hashes used in comparison
int dimension; // number of values returned by each call to the test function
int coord; // current coordinate to be processed [0..dimension-1]

// Auxiliary tools for qsort call:
// comparing pairs of test value and hash values lexicographically for sorting
int lexcomp(long double v1, long double v2, unsigned long int h1, unsigned long int h2){
  if (v1<v2) {return(-1);}
  if (v1>v2) {return(1);}
  if (h1 < h2){return(-1);}
  if (h1 > h2){return(1);}
  return(0);
}

// used as qsort argument() in the test_p_value function
int comp (const void *elem1, const void *elem2){
  int ref1 = *((int*) elem1);
  int ref2 = *((int*) elem2);
  return(lexcomp(v[ref1*dimension+coord],v[ref2*dimension+coord],h[ref1],h[ref2]));
}

// parameters conventions:
// param[0] number of values in the first sample (test generator)
// param[1] number of values in the second sample (etalon generators)
// func: tested function that computes value/hash given the access to a generator
// uses param[2..] convention: param[2] is number of values produced by 
//     the test function (the "dimension" of its output)
// param[3] specifies the amount of bits to be used if it makes sense for a test
// (different test could measure it in different ways, so the interpretation
// is test-dependent, and some could determine themselves how many data they
// need and ignore this parameter)
// real_param[i] are used by some tests (error bounds etc.)
//
bool test_p_value(long double pvalue[], // here the array of p-values 
                                        // of length param[2] is returned 
      test_func f, // which test to use
      PRG test, PRG etalon, // two generators that are compared
      int *param, double *real_param, // parameters for the test (int, real)
      bool output,  // whether output files are needed 
      char *output1, char *output2, // and how they should be named 
      bool debug, // whether debug information should be printed to stout
      bool ksexact) // whether exact KS-value computation should be used
      // returns false if some error (not enough bits etc.)
{      
  int n0= param[0]; int n1= param[1]; // number of test values for the 
      // test and etalon generators
  dimension= param[2]; // number of values returned by the test function
  assert(dimension>0);
  long double value0[n0*dimension]; // each call to the test function fills dimension 
  long double value1[n1*dimension]; // consecutive positions in these arrays
       // value0 is for the test, value1 is for the etalon
  unsigned long hash0[n0], hash1[n1]; // and one hash value (64 bits)
       // hash0 is for the test, hash1 is for the etalon
  int ref0[n0], ref1[n1]; // we sort references, not the arrays itself
  
  // each call fills [dimension] values and one hash for test generator:
  for (int i=0; i<n0; i++){
    if (!f(&(value0[i*dimension]),&(hash0[i]),test,param,real_param,debug)){
      return(false);
    }
  }
  if(debug){printf("Test generator sampled\n");}
  
  // each call fills [dimension] values and one hash for etalon generator: 
  for (int i=0; i<n1; i++){
    if (!f(&(value1[i*dimension]),&(hash1[i]),etalon,param,real_param,debug)){
      return(false);
    }  
  }
  if(debug){printf("Etalon generator sampled\n");}
  
  // values and hashes filled, now for each coordinate we compute p-value
  // and put the result into p_value[0..dimension-1]
  for (coord=0; coord<dimension; coord++){
    if (debug){printf("Processing coordinate %d...\n",coord);}

    // Sorting values for the tested generator:
    v= value0; h= hash0; // setting global variables for comp() calls
    for (int i=0; i<n0; i++){ref0[i]= i;}
    qsort(ref0, n0, sizeof(int), comp);
    // initially ref0=[0,n0); now it is sorted according to p-values

    // Sorting values for the etalon generator:
    v= value1; h= hash1; // setting again global variables for comp()
    for (int i=0; i<n1; i++){ref1[i]= i;}
    qsort(ref1, n1, sizeof(int), comp);
    // both arrays ref0, ref1 are sorted according to value+hash pairs
    
    if (debug) { // printing the values+hash arrays if requested
      printf("First array:\n");
      for (int i=0; i<n0; i++){
        printf("[ %20.18Lf, ",value0[ref0[i]*dimension+coord]);
        print64(hash0[ref0[i]]);
        printf("]\n");
      }
      printf("Second array:\n");
      for (int i=0; i<n1; i++){
        printf("[ %20.18Lf, ",value1[ref1[i]*dimension+coord]);
        print64(hash1[ref1[i]]);
        printf("]\n");
      }
    }  
    if (output){
      // For each coordinate create two files named 
      // output1.coordinate, output2.coordinate:
      char filetest[512], fileetal[512];
      FILE *outtest, *outetal;
      strncpy(filetest,output1,256);
      strncpy(fileetal,output2,256);
      char coord_str[8];
      sprintf(coord_str, ".%04d", coord); 
      strncat(filetest,coord_str,6); strncat(fileetal,coord_str,6);  
      if (debug){
        printf("Output file names: [%s] and [%s]\n",filetest,fileetal);
      }
      // output values (without hashes for now) to these files:
      outtest= fopen(filetest, "w"); outetal= fopen(fileetal,"w");
      for (int i=0; i<n0; i++){
        fprintf(outtest, "%20.18Lf\n", value0[ref0[i]*dimension+coord]);
      }  
      for (int i=0; i<n1; i++){
        fprintf(outetal, "%20.18Lf\n", value1[ref1[i]*dimension+coord]);
      }        
      fclose(outtest); fclose(outetal);
    }
    
    // Now we prepare a Boolean array of length n0+n1 that indicates the 
    // source: kstring[i] bits = false/true if from ref0/ref1 array 
    // (input to KS test)
    bool kstring[n0+n1]; 
    int pos0= 0;
    int pos1= 0;
    // pos0 and pos1 elements are "virtually merged" in ref0/ref1
    while (pos0+pos1<n0+n1){//there are still elements to merge
      if (pos0==n0){ // ref0 exhausted 
        assert(pos1<n1);
        kstring[pos0+pos1]= true; pos1++;
      } else if (pos1==n1){ // ref1 exhausted 
        assert(pos0<n0);
        kstring[pos0+pos1]= false; pos0++;
      } else {
        assert(pos0<n0); assert(pos1<n1);
        if (debug){
          printf("Comparing: <%Lf,%ld>?<%Lf, %ld>\n",
          value0[ref0[pos0]*dimension+coord], hash0[ref0[pos0]], 
          value1[ref1[pos1]*dimension+coord], hash1[ref1[pos1]]);
        }
        int cmp= lexcomp(value0[ref0[pos0]*dimension+coord],
                         value1[ref1[pos1]*dimension+coord],
                         hash0[ref0[pos0]],hash1[ref1[pos1]]);
        if (cmp<0){kstring[pos0+pos1]= false; pos0++;}
        if (cmp==0){
          fprintf(stderr, "Warning: unresolved tie\n");
          // considered as ref0 array but invalidates the result!   
          kstring[pos0+pos1]= false; pos0++;
        }        
        if (cmp>0){kstring[pos0+pos1]= true; pos1++;}
      }
    }  
    if (debug){
      printf("KS string: ");
      for (int i=0; i<n0+n1; i++){printf("%d",kstring[i]);}        
      printf("\n");      
    }
    // kstring[n0+n1] is a boolean string to be KS-analyzed
            
    // computing deviation as required by KS-test
    long pref= 0;
    long max_dev= 0;
    long k0=0;
    long k1=0;
    // max_dev = n0*n1*(maximal deviation for prefixes up to length pref)
    // k0, k1 = number of let[0], let[1] among first pref letters
    while (pref!=n0+n1){
      if (kstring[pref]){
        k1++;
      }else{
        k0++;
      }  
      pref++;
      if (abs(k0*n1-k1*n0)>max_dev){max_dev=abs(k0*n1-k1*n0);}
    }
    // max_dev = KS-deviation*n0*n1
    long double deviation=(double)max_dev/(((double)n0)*((double)n1));
    if (debug){printf("Maximal deviation on coordinate %d: %Lf\n",coord, deviation);}  
    long double ksval;
    // Calling KS-test (exact uses integer max_dev = n0*n1*maximal deviation;
    // approximate version uses deviation as a long double
    if (ksexact){
      ksval= ks2bar(n0,n1,max_dev); // exact computation
    }else{
      ksval =1-psmirnov2x(deviation, n0, n1); // with long double numbers
    }  
    if (debug){printf("KSvalue on coordinate %d: %20.15Lf\n",coord,ksval);}
    // now KS-value for coordinate coord is in ksval
    pvalue[coord]= ksval;
  } // end for (coord=0; coord<dimension; coord++)
  return(true);
} // end of test_p_value () definition

// global array for parameters communicated to tests
// TODO: real_parameters not used yet
#define NUM_PARAM 10
int int_parameters[NUM_PARAM];
double real_parameters[NUM_PARAM];

// Printing usage message for a wrong formatted call
void usageif(bool x, char* progname){
  if (x){
    fprintf(stderr, "Usage: %s [-k -r -x -v] -d dimension -f file -e etalon -p num_samples_file -q num_samples_etalon -t test_number -m modifier -n num_tested -o output_directory \n", progname);
    exit(1);
  }
}

// Main program: analyzes options and calls testing
int main(int argc, char *argv[]){
  bool ok;
  byte next;
  int opt; // return value for getopt function
  
  // TODO: long option names (now mentioned but not implemented
  // Variables to keep option parameters
  // -x (use xoring for the etalon) [--xor]
  bool use_xor= false;
  
  // -f tested_file_name [--file]
  // -e reference_file_name [--etalon]
  char *tested, *etalon;
  bool tested_specified= false;
  bool etalon_specified= false;
  
  // -o output_directory_name (for sorted pvalues) [--output]
  char *output_directory;
  bool output= false;
  
  // -v debug option [--debug]
  bool debug= false;
  
  // -k use exact Kolmogorov--Smirnov computation [--ksexact]
  bool ksexact= false;
  
  // -p n0 -q n1 sample sizes for 2-sample KS-testing [--ptest --petalon]
  int n0,n1; 
  
  // -t test_number [--test]
  int num_func= -1; // no default test function
  
  // -d number of values returned by the test, 1 by default [--dimension]
  // should be given as option in the command line otherwise 
  int dimension= 1; 
  
  // -r number of repetitions requested, 0 means "as much as data allow"
  // [--repetitions]
  int repetitions= 1;
  
  // -n how much data should be tested [--ntest]
  // interpreted by each test in its own way 
  int num_tested= 0;
  
  // -m modifier [--modifier], changes the behavior of some tests
  int modifier= 0;
  
  // options reading loop using getopt library function:
  while ((opt = getopt(argc, argv, "kvxr:o:n:d:f:e:p:q:t:m:")) != -1) {
    switch (opt) {
      case 'k':
        ksexact= true;
        break;
      case 'v':
        debug= true;
        break;
      case 'x':
        use_xor = true;
        break;
      case 'o':
        output= true;
        output_directory= optarg;
        break;
      case 'd':
        dimension=atoi(optarg);  
        break;
      case 'n':
        num_tested=atoi(optarg);
        break;  
      case 'r':
        repetitions= atoi(optarg);
        break;        
      case 'f':
        tested= optarg;
        tested_specified= true;
        break;
      case 'e':
        etalon= optarg;
        etalon_specified= true;
        break;        
      case 'p':
        n0= atoi(optarg);
        break;
      case 'q':
        n1= atoi(optarg);    
        break;
      case 't':
        num_func= atoi(optarg); 
        break; 
      case 'm':
        modifier= atoi(optarg);  
      default: /* '?' */
        usageif(true,argv[0]);
    }
  }  // end of reading-options loop
  
  #define SAMPLE_SIZE_BOUND 10000
  // checking correctness of the options:
  usageif((optind!=argc)||(n0<=0)||(n1<=0)||(!tested_specified)||(!etalon_specified)||(num_func<0), argv[0]);
  if ((n0>SAMPLE_SIZE_BOUND)||(n1>SAMPLE_SIZE_BOUND)){    
    fprintf(stderr,"Too big sample size in, check -p/-q option arguments\n"); 
    exit(1); 
  }
  if (repetitions<0){
    fprintf(stderr,"Number of repetitions [-r argument] should be non-negative\n"); 
    exit(1);
  }
  if (num_func>=len_func_list){
    fprintf(stderr,"Test function with this number does not exist\n");
    exit(1);
  }
  if (output){
    if (mkdir(output_directory,0755)!=0){
      fprintf(stderr,"Output directory requested but cannot be created\n");
      exit(1);
    }
    // output directory created 
  }
  
  // creating generators
  PRG test;
  ok=g_create_file(&test, tested);
  if (!ok){fprintf (stderr, "File for testing [-f] not found\n"); exit(1);}
  if (debug){printf("PRG test generator = %d\n", test);}

  PRG etal;
  ok= g_create_file(&etal, etalon);
  if (!ok){fprintf (stderr, "Etalon file [-e] not found\n"); exit(1);}
  if (debug){printf("PRG etalon generator = %d\n", etal);}

  PRG compare_etalon;
  if (use_xor){
    ok=g_create_xor(&compare_etalon, test, etal);
    if (!ok){fprintf (stderr, "Internal error: XOR generator not created\n");}
  }else{
    compare_etalon= etal;
  }  
  if (debug){printf("PRG generator for comparison = %d\n", compare_etalon);}
  // generators test and compare_etalon are ready
  
  // preparing global arrays TODO: only integer parameters for now
  int_parameters[0]= n0;
  int_parameters[1]= n1;
  int_parameters[2]= dimension;
  int_parameters[3]= num_tested;
  int_parameters[4]= modifier; 
  if (debug&&!use_xor){
     printf("Comparing files %s and %s\n", tested, etalon);
  }
  if (debug&&use_xor){
     printf("Comparing files %s and (%s xor %s)\n", tested, tested, etalon);
  }
  if (debug){
    printf("using samples of size %d and %d respectively\n", n0,n1);
    printf("Test function used: %s\n", functions_list[num_func].description);
    printf("Dimension (number of returned values): %d\n", dimension);
    printf("Test size: %d\n", num_tested);
  }
  
  ok=true;
  int filecount= 0;
  char output1[256], output2[256]; // we create filenames and they are not long
  while(ok){ // repeated according to num_repetitions
    long double p[dimension];
    // creating prefixes for output files if requested
    if (output){
      // create filenames for p-values from the tested and etalon files
      assert(strlen(output_directory)<128);
      strncpy(output1,output_directory,128);
      strncpy(output2,output_directory,128);
      strncat(output1,"/",2); strncat(output2,"/",2);
      char ordnum[64];
      sprintf(ordnum, "%06d", filecount);
      filecount++;
      strncat(output1,ordnum,64); strncat(output2,ordnum,64);
      strncat(output1,".test",7); strncat(output2,".etal",7);
      if (debug){
        printf("Output files prefixes: [%s] and [%s]\n", output1, output2);
      }
    }  
    ok= test_p_value (p, functions_list[num_func].reference, test, compare_etalon, 
              int_parameters, real_parameters, output, output1, output2, debug, ksexact);
    if (ok){
      for(int i=0; i<dimension; i++){
        printf("%20.18Lf ",p[i]);
        if ((i+1)%5==0){printf("\n");}
      }
      printf("\n"); 
      if (repetitions==1) {ok= false;}
      if (repetitions>1)  {repetitions--;} 
      // if repetitions==0, it is unchanged, so data are tested as long as possible
    }
    if (ok){printf("Used %lu bytes from the test generator [%d]\n", g_num_read(test), test);}
    if (ok){printf("Used %lu bytes from the etalon generator [%d]\n", g_num_read(etal), etal);}
  }
  exit(0);
}
