#include "graph_pipe.h"

void main_graph (int argc, char* argv[]){
 int i;
 graph_color(GREEN);
 for (i=0;i<WIN_WIDTH;i++){
   graph_draw(i,i);
   usleep (100);
 }
 graph_color(RED);
 graph_draw_line(0,700,700,0);
 
 graph_end();
}

