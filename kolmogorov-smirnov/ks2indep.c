#include "ks2indep.h"
#include "ksmirnov.h"

#define CONDITION0(x)    (x>0)
#define CONDITION1(x)    (x%2==0)

// needs to visible from functions
int num_skipped=0; // how many samples should be skipped (used only for tie breaking)

// copied from drawpair.c
// *next <- integer made on optb bytes from in, MSB first if optm otherwise LSB first
// returns true if succeeded
bool get_next (int *next, FILE *in, int optb, int optm){
  unsigned char buf[MAX_BYTES]; // here the bytes that form the next integer are stored
  int num_read= 0;
  bool ok= true;
  while (ok && (num_read !=optb)){
    int next_char=getc(in);
    if (next_char==EOF){
      ok= false;
    }else{  
      buf[num_read]=(unsigned char) next_char;
      num_read++;
    }
  }
  if ((num_read==optb)&&ok) {
    // buf[0..optb) is filled and should be converted to int according to optm
    if (optm){
      int ext= (int) (char) buf[0]; // should take into account the sign extension
      assert (-128<=ext); assert (ext<=127);
      *next= ext; 
      for (int i=1;i<optb;i++){
        ext = (int) buf[i];
        assert (0<=ext); assert (ext<=255);
        *next=(*next)*256+ext;
      }
    }else{
      int ext= (int) (char) buf[optb-1]; // should take into account the sign extension
      assert (-128<=ext); assert (ext<=127);
      *next= ext; 
      for (int i=optb-2;i>=0;i--){
        ext = (int) buf[i];
        assert (0<=ext); assert (ext<=255);
        *next=(*next)*256+ext;
      }    
    }
    return(true);
  }else{
    return(false);
  }
}

// comparing lexicographically two arrays of integers of
// length num_skipped+1, given their initial positions in memory
int comp (const void *elem1, const void *elem2){
  int ans=0;
  int i=0; // number of compared elements
  while ((ans==0)&&(i<num_skipped+1)){
    if (((int*)elem1)[i]<((int*)elem2)[i]){ans=-1;}
    if (((int*)elem1)[i]>((int*)elem2)[i]){ans= 1;}
    i++;
  }
  return(ans);
}

// Debug: printing a sequence of num_skipped+1 integers
int printarr(void *elem){
  int *arr;
  arr= elem;
  printf("[ ");
  for (int i=0; i<num_skipped+1; i++){
    printf("%d ", arr[i]);
  }  
  printf(" ]\n");
}
  
int main(int argc, char* argv[]){

  bool optl= 0;
  bool optm= 0;
  bool opti= 0;
  bool optv= 0;
  long num_samples=1000; // default: total number of reading in both arrays
  int num_bytes=0; // how many bytes in each sample
  int condition=0; // which condition should be used for selection
  
  int c;
  while ((c= getopt(argc, argv,"lmivb:n:s:c:"))!=-1){
    switch(c){
    case 'l': 
      optl= true;
      break;
    case 'm':
      optm= true;
      break;  
    case 'i':
      opti= true;
      break;  
    case 'v':
      optv= true;
      break;  
    case 'b': 
      num_bytes=atoi(optarg);
      if ((num_bytes<1)||(num_bytes>8)){
        printf("-b option argument should be in 1..%d\n", MAX_BYTES);
        exit(1);
      }
      break;
    case 'n': 
      num_samples=atoi(optarg);
      if ((num_samples<1)||(num_samples>MAX_SAMPLES)){
        printf("-n option should be in 1..%d\n", MAX_SAMPLES);
        exit(1);
      }
      break;
    case 's':
      num_skipped=atoi(optarg);
      if ((num_skipped<0)||(num_skipped>MAX_SKIPPED)){
        printf("-s option should be in 1..%d\n", MAX_SKIPPED);
      }
      break;     
    case 'c':
      condition=atoi(optarg);
      if ((condition <0)||(condition>=NUM_CONDITIONS)){
        printf("-c option should be in 0..%d\n", NUM_CONDITIONS-1);
      } 
      break; 
    case '?':
      if (isprint(optopt)){
        printf("Error: option -%c invalid\n",optopt);                                                            
        printf ("Usage: %s -l|-m -n num_samples -b bytes_per_sample -s num_skipped samples -c condition] file1 file2\n", argv[0]); 
        printf ("or %s -l|-m -i -n num_samples -b bytes_per_sample -s num_skipped samples -c condition] file\n", argv[0]);
      }else{
        printf ("Unknown option character `\\x%x'.\n",optopt);
      }
      exit(1);
    default: assert(false);
    }    
  }
  if (optm+optl!=1){
    printf("Exactly one of -l|-m options should be used (first byte as Least/Most significant one)");
    exit(1);
  }  
  if ((!opti)&&(argc!=optind+2)){
    printf ("Error: two file argument required if -i is not used\n");
    exit(1);
  }
    if ((opti)&&(argc!=optind+1)){
    printf ("Error: one file argument required if -i is used\n");
    exit(1);
  }
  
  // options are consistent; opening files:
  // opening file(s)
  FILE *in1, *in2;
  in1= fopen(argv[optind], "rb");
  if (in1==NULL){
    printf("Error: cannot open file: %s\n", argv[optind]);
    exit(1);
  }
  if (!opti){  
    in2= fopen(argv[optind+1], "rb");
    if (in2==NULL){
      printf("Error: cannot open file: %s\n", argv[optind+1]);
      exit(1);
    }  
  }
  // arrays to keep the samples from the second file (+ subsequent skipped samples for 
  // tie breaking)
  int sample[2][num_samples][num_skipped+1];
 
  long histp[10]; long total=0;
  for (int i=0;i<10;i++){histp[i]= 0;}
  
  while(true){ // loop that breaks when there is not enough data 
    //DEBUG
    //printf("Getting two samples pair:\n");
    
    long num_total;
    long num_item[2];
    
  
    num_total= 0; num_item[0]= 0; num_item[1]= 0;
    // num_samples are read from the second file (together with skipped samples after them)
    // and put to one of the two arrays sample[0] and sample[1] according to the condition
    bool ok= true;
    while (ok && (num_total<num_samples)){
      // get data from both files
      // classify the second data according to the value of the first one
      int next1, next2;
      ok = get_next (&next1, in1, num_bytes, optm);
      if (ok) {ok = get_next (&next2, (opti?in1:in2), num_bytes, optm);}
      if (!ok) break;
      
      //DEBUG
      //printf("Next two integers obtained: %d, %d\n", next1, next2);
      
      bool cond;
      switch (condition){
      case 0:
        cond= CONDITION0 (next1);
        break;
      case 1:
        cond= CONDITION1 (next1);
        break;
      default:
        assert(false);  
      }
      sample[cond][num_item[cond]][0]=next2;
      
      //DEBUG
      //printf("Condition value for %d: %d\n",next1,cond);
      
      // now we have to fill the data for skipped samples (for tie breaking)
      for (int i=1, ok= true; (i<=num_skipped)&&ok; i++){
        if (!opti) {ok= get_next(&next1, in1, num_bytes, optm);}
        if (ok){ ok= get_next(&next2, (opti?in1:in2), num_bytes, optm); }
        if (ok) {sample[cond][num_item[cond]][i]=next2;}
        //DEBUG
        //printf("Next two integers skipped: %d, %d\n", next1, next2);
      }
      if (ok) {num_item[cond]++; num_total++;}
    }
    assert(num_total==num_item[0]+num_item[1]);  
    if (num_total < num_samples){break;} // not enough data     
    // two arrays are filled with the samples from the second file split according 
    // to the corresponding values in the first file and conditions
    
    //DEBUG
    //printf("Sample 0, size %ld, unsorted:\n", num_item[0]);
    //for (int i=0; i<num_item[0]; i++){
    //   printarr(sample[0][i]);
    //   //sleep(1);
    //}   
    //printf("Sample 1, side %ld, unsorted:\n", num_item[1]);
    //for (int i=0; i<num_item[1]; i++){
    //   printarr(sample[1][i]);
    //   //sleep(1);
    //} 
    
    qsort(sample[0], num_item[0], sizeof(int)*(num_skipped+1), comp);
    //DEBUG
    //printf("First sample sorted\n");
    
    qsort(sample[1], num_item[1], sizeof(int)*(num_skipped+1), comp);
    //DEBUG
    //printf("Second sample sorted\n");
        
    //DEBUG
    //printf("Sample 1, sorted:\n");
    //for (int i=0; i<num_item[0]; i++){
    //   printarr(sample[0][i]);
    //   //sleep(1);
    //}   
    //printf("Sample 2, sorted:\n");
    //for (int i=0; i<num_item[1]; i++){
    //   printarr(sample[1][i]);
    //   //sleep(1);
    //} 
    
    // arrays are sorted
    //virtual merging the arrays creating a 0-1-sequence
    bool kstring[num_samples]; // is the element from the sample[1] array?
    int pos0= 0;
    int pos1= 0;
    // pos0 and pos1 elements are "merged" in sample[0] and sample[1]
    while (pos0+pos1<num_samples){//there are still elements to merge
    
      //DEBUG
      //printf("Merged %d and %d elements\n",pos0,pos1);
      //sleep(1);
      
      
      if (pos0==num_item[0]){ // first array exhausted
        assert(pos1<num_item[1]);
        kstring[pos0+pos1]= true; pos1++;
      } else if (pos1==num_item[1]){
        assert(pos0<num_item[0]);
        kstring[pos0+pos1]= false; pos0++;
      } else {
        assert(pos0<num_item[0]); assert(pos1<num_item[1]);
        int cmp= comp(sample[0][pos0],sample[1][pos1]);
        if (cmp<0){kstring[pos0+pos1]= false; pos0++;}
        if (cmp==0){
           fprintf(stderr, "Warning: unresolved tie\n");   
           kstring[pos0+pos1]= false; pos0++;
        }        
        if (cmp>0){kstring[pos0+pos1]= true; pos1++;}
      }
    }
          
    // computing deviation
    long n0=num_item[0]; long n1=num_item[1];
    long pref= 0;
    long max_dev= 0;
    long k0=0;
    long k1=0;
    // max_dev = n0*n1*(maximal deviation for prefixes up to length pref)
    // k0, k1 = number of let[0], let[1] among first pref letters
    while (pref!=num_samples){
      if (kstring[pref]){
        k1++;
      }else{
        k0++;
      }  
      pref++;
      if (abs(k0*n1-k1*n0)>max_dev){max_dev=abs(k0*n1-k1*n0);}
    }
    double deviation=(double)max_dev/(((double)n0)*((double)n1));
    if (optv){
      printf("Maximal deviation: %f\n", (float) deviation);
    }  
    double ksval=1-psmirnov2x(deviation,(int) n0, (int) n1);
    if (optv){
      printf("KSvalue: %20.15lf\n",ksval);
    }else{
      printf("%20.15lf\n",ksval);
    }  
    total++;
    histp[(int)(ksval*10)]++;
    // p-value is computed, printed and counted
  }
  if (optv){
    printf("Total number of ks-values: %ld \n", total);
    for (int i=0; i<10; i++){
      printf(" %6ld ", histp[i]);
    }
    printf("\n");
  }
}
