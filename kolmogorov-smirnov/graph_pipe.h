#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#ifdef linux
#include<GL/glut.h>
#else
#include<GLUT/glut.h>
#endif

// size of the window to be displayed
#define WIN_WIDTH 2000 // window size
#define WIN_HEIGHT 2000
#define POINT_SIZE 2 // point size (1 seems hardly visible)
#define LINE_WIDTH 3 // wider lines for histograms

#define POINT (-1)
#define LINE (-2)
#define COLOR (-3)
#define WHITE 100
#define RED 101
#define ORANGE 102
#define YELLOW 103
#define GREEN 104
#define BLUE 105
#define DARKBLUE 106
#define VIOLET 107
#define MAGENTA 108
#define GREY 109


void main_graph(int argc, char* argv[]); // parent
void main_child(int argc, char* argv[]); // child
void graph_draw(int x, int y); // to draw a point (x,y); 0<=x<WIN_WIDTH, 0<=y<WIN_HEIGHT
void graph_draw_line(int x, int y, int nx, int ny); // draw a line from (x,y) to (nx,ny), 0<=x,nx<WIN_WIDTH, 0<=y,ny<WIN_HEIGHT
void graph_color(int col); // change color to col
void graph_end(); // finish drawing

// data for pipe, internal

int  fd[2]; // should be external to be shared between parent and child
#define PIPEWRITE fd[1]
#define PIPEREAD fd[0]
