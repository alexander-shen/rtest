#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include <sys/param.h> // form MAX(a,b) and MIN(a,b)
#include "graph_pipe.h"

#define MAXLEN 30000 // maximal sample size
#define MAXFILES 8
int predefined[MAXFILES]={WHITE,RED,ORANGE,YELLOW,GREEN,BLUE,DARKBLUE,VIOLET};

int compar (const void *a, const void *b){
  if (*((double*)a)< *((double*)b)) return(-1);
  if (*((double*)a)> *((double*)b)) return(1);
  return(0);
}


void main_graph(int argc, char* argv[]){
  if (argc<2){
    printf("Usage: %s filename_with_sample1 filename_with_sample2...\n", argv[0]);
    exit(1);
  }
  if (argc>MAXFILES+1){
    printf("Error: too many (%d) sample files\n",argc-1);
    exit(1);
  }
  
  int numfiles=argc-1;
  assert (numfiles <= MAXFILES);  
  FILE *infile[MAXFILES];
  for (int i=0;i<numfiles;i++){
    infile[i] = fopen(argv[i+1],"rb");
    if (infile[i] == NULL){printf("Error: cannot open file %s\n", argv[i+1]); exit(1);}
  } 
  // files opened, infile[0..numfiles)are filepointers
  printf("Files opened\n");
  
  double values[MAXFILES][MAXLEN]; // arrays for samples
  long numvals[MAXFILES]; // number of values for two samples

  // reading from file
  for (int numfile=0; numfile<numfiles; numfile++){
    double val;
    long numval= 0;
    int c;
    while ((numval < MAXLEN)&& (fscanf(infile[numfile],"%lf",&val)==1)){
      do {c=getc(infile[numfile]);} while ((((char) c)!='\n')&&(c!=EOF)); // skip to the next line
      values[numfile][numval]= val;
      numval++;
    }
    assert (numval<MAXLEN);
    assert (numval>1);
    numvals[numfile]= numval;
  }
   
  printf("Values read\n"); 
  // values[0][0..numvals[0]), values[1][0..numvals[1]) are the sample arrays
  
  // sorting:
  for (int i=0; i<numfiles; i++){
    qsort(values[i],numvals[i],sizeof(double),compar);
  }

  // values[i][0..numvals[i]) are sorted both for i in [0,MAXFILES)
#define DRAW(x,y,nx,ny) graph_draw_line( (int)(1+(WIN_WIDTH-2)*((x-min)/width)), \
                                         (int)(1+(WIN_HEIGHT-2)*y), \
                                         (int)(1+(WIN_WIDTH-2)*((nx-min)/width)), \
                                         (int)(1+(WIN_HEIGHT-2)*ny) )
  double min = values[0][0];
  double max = values[0][numvals[0]-1];
  int k= 1;
  // min,max are minimal and maximal arrays for the first k files
  while (k!=numfiles){
    if (values[k][0]<min){ min= values[k][0]; }
    if (values[k][numvals[k]-1]>max){ max= values[k][numvals[k]-1]; }
    k++;
  } 
  printf("Horizontal interval: [%lf,%lf]\n", min,max); 
  if (min==max){
    printf("All elements are the same\n"); graph_end(); exit(1);
  }
  double width = max-min;
  for (int numfile=0; numfile<numfiles; numfile++){
    graph_color(predefined[numfile]);
    double vstep=1.0/(double)numvals[numfile];
    // ... drawing the hystogram
    double curx=0.0; double cury=0.0; double newx;
    for (long i=0;i<numvals[numfile];i++){
      if (i>0){
         // horizontal segment from the previous point:
         DRAW (values[numfile][i-1],i*vstep, values[numfile][i],i*vstep);
      }
      // vertical segment:
      DRAW (values[numfile][i], i*vstep, values[numfile][i], (i+1)*vstep);
    }  
  }
}  
