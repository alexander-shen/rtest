#include "ks2.h"
#include "graph_pipe.h"
void main_graph(int argc, char* argv[]){
  if (argc!=1){
    printf("Usage: %s < filename_of_a_string_with_two_characters\n", argv[0]);
    exit(1);
  }
  
  #define MAXSTRINGLEN 1000000 
  char let[256];
  long count[256];
  int numlet=0;
  // read the argument string, put the letters in let[numlet] and their number of appearences in count[numlet]
  int next;
  long length = 0;
  char* str = malloc(MAXSTRINGLEN*sizeof(char));
  if (str == NULL){printf("Not enough memory\n"); exit(1);}
  while ((next= getchar())!=EOF){
    char nextchar = (char) next;
    //find or add the letter nextchar and increase the count
    if ((nextchar !='\n') && (nextchar != ' ') && (nextchar != '\t')){
      int j=0;
      while ((j<numlet)&&(let[j]!=nextchar)){j++;}
      if ((j<numlet)&&(let[j]==nextchar)){
        count[j]++;
      }else if (j==numlet){
        numlet++;
        assert (numlet<256);
        let[j]= nextchar;
        count[j]= 1;
      }else{
        assert(false);
      }
      assert (length < MAXSTRINGLEN);
      str[length]= nextchar;
      length++;
    }
  }
  printf("Profile:\n");
  printf("  length: %ld\n",length);
  for (int i=0; i<numlet; i++){
     printf("  character [%c] appears [%ld] times\n", let[i], count[i]);
  }
  if ((numlet==2) && (count[0]>0) && (count[1]>0)){
    //printf("Data suitable for KStest\n");
    long n0=count[0]; long n1=count[1];
    long pref= 0;
// drawing a line from (x,y) to (nx,ny)
// entire window is n0*n1    

// drawing line (x,y)--(nx,ny) in coordinates where window has size n0*n1
#define DRAW(x,y,nx,ny) graph_draw_line( (int)(1+(WIN_WIDTH-2)*(x/(float)n0)), \
                                         (int)(1+(WIN_HEIGHT-2)*(y/(float)n1)), \
                                         (int)(1+(WIN_WIDTH-2)*(nx/(float)n0)), \
                                         (int)(1+(WIN_HEIGHT-2)*(ny/(float)n1)))
    long max_dev= 0;
    long k0=0;
    long k1=0;
    int curx= 0; int cury= 0;
    // max_dev = n0*n1*(maximal deviation for prefixes up to length pref)
    // k0, k1 = number of let[0], let[1] among first pref letters
    while (pref!=length){
      int lastx= curx; int lasty= cury;
      if (str[pref]==let[0]){
        k0++; curx++;
      }else if(str[pref]==let[1]){
        k1++; cury++;
      }else{
        assert(false);
      }  
      DRAW(lastx,lasty,curx,cury);
      pref++;
      if (abs(k0*n1-k1*n0)>max_dev){max_dev=abs(k0*n1-k1*n0);}
    }
    printf("Maximal deviation: %f\n", ((float)max_dev/(((float)n0)*((float)n1))));
    printf("KSvalue: %20.15lf\n",ks2bar(n0,n1,max_dev));
    DRAW (0,0,n0,n1);
  }else{
    assert(numlet!=2);
    printf("Error: data should contain exactly two letters\n");
  }
  free(str);
}
