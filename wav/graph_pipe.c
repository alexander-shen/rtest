#include "graph_pipe.h"

// common portion for parent and child

// Pipe is a stream of integers of the format
// ((POINT x y) | (LINE x y nx ny ))* 

int main(int argc, char* argv[]){
    pid_t   childpid;  
    // creating pipe and storing information in fd
    if (pipe(fd)<0){printf("Error:pipe not opened\n");exit(1);}
    if((childpid = fork()) == -1){ // fork call
        perror("fork"); exit(1);
    }
    if(childpid == 0){
        if (fcntl(fd[0], F_SETFL, O_NONBLOCK) < 0){// non-block reading set
          printf("Error: flags non set\n"); exit(1);
        }
        close(fd[1]); // Child process closes up output side of pipe 
        main_child(argc, argv); // calling child behavior
    }else{
        close(fd[0]); // Parent process closes up input side of pipe
        main_graph(argc,argv); // calling parent behavior
    }
}

// parent portion
void graph_draw (int x, int y){
  assert ((0<=x)&&(x<WIN_WIDTH)&&(0<=y)&&(y<WIN_HEIGHT));
  int command= POINT;
  write(PIPEWRITE,&command,sizeof(int));
  write(PIPEWRITE,&x,sizeof(int));
  write(PIPEWRITE,&y,sizeof(int));
}

void graph_draw_line (int x, int y, int nx, int ny){
  assert ((0<=x)&&(x<WIN_WIDTH)&&(0<=y)&&(y<WIN_HEIGHT));
  assert ((0<=nx)&&(nx<WIN_WIDTH)&&(0<=ny)&&(ny<WIN_HEIGHT));
  int command= LINE;
  write(PIPEWRITE,&command,sizeof(int));
  write(PIPEWRITE,&x,sizeof(int));
  write(PIPEWRITE,&y,sizeof(int));
  write(PIPEWRITE,&nx,sizeof(int));
  write(PIPEWRITE,&ny,sizeof(int));
}

void graph_color(int col){
  assert((col==WHITE)||(col==RED)||(col==ORANGE)||(col==YELLOW)||(col==GREEN)||(col==BLUE)||(col==DARKBLUE)||(col==VIOLET));
  int command= COLOR;
  write(PIPEWRITE,&command,sizeof(int));
  write(PIPEWRITE,&col,sizeof(int));
}

void graph_end(){
  close(PIPEWRITE);
}  

// child portion

void display() {
   glFlush();  // Render now
}


// IdleEvent looks at the pipe and draws pairs
// or skips them if they are outside the window, 
// until the pipe in empty (and then returns). 
// It may happen that the number of values is odd,
// then the last value is saved for the next call
void IdleEvent (){
#define MAXBUFLEN 5
  static int buf[MAXBUFLEN];
  static int buflen=0; // at most 5 [<=4 between calls]
  static int pipe_closed= false;
  static bool inside= false; // inside Begin_End_group
  static int regime; // POINT or LINE, valid if inside
  static int cur_color;
  int inp; // place to read from the pipe
  int numread; // number of elements read
 
  numread= read(PIPEREAD, &inp, sizeof(int));
  while (numread==sizeof(int)){
    pipe_closed= false;
    assert(buflen<MAXBUFLEN);
    buf[buflen]=inp; buflen++; // put the new element to buffer
    if ((buflen==2)&&(buf[0]==COLOR)){
      if (buf[1]==WHITE){glColor3f(1.0f, 1.0f, 1.0f);}
      if (buf[1]==RED){glColor3f(1.0f, 0.14f, 0.0f);}
      if (buf[1]==ORANGE){glColor3f(1.0f, 0.6f, 0.0f);}
      if (buf[1]==YELLOW){glColor3f(0.9f, 0.3f, 0.15f);}
      if (buf[1]==GREEN){glColor3f(0.2f, 1.0f, 0.2f);}
      if (buf[1]==BLUE){glColor3f(0.5f, 0.9f, 0.8f);}
      if (buf[1]==DARKBLUE){glColor3f(0.15f, 0.2f, 0.9f);}
      if (buf[1]==VIOLET){glColor3f(0.59f, 0.07f, 0.97f);}      
      buflen= 0;
    } else if ((buflen==3)&&(buf[0]==POINT)){
      // draw a point with buffer data; clear the buffer
      int x= buf[1]; int y= buf[2];
      if ((x>=0)&&(x<WIN_WIDTH)&&(y>=0)&&(y,WIN_HEIGHT)){
        if (!inside){
          inside= true;
          glBegin(GL_POINTS);
          regime= POINT;
        } else if (inside && (regime==LINE)){
          glEnd();
          glBegin(GL_POINTS);
          regime= POINT;
        }   
        assert (inside && (regime==POINT)); 
        glVertex2i(x,y);
      }else{
        //points outside the window are skipped
      }
      buflen= 0; // clear the buffer
    } else if ((buflen==5)&&(buf[0]=LINE)){
    // draw a point with buffer data; clear the buffer
      int x= buf[1]; int y= buf[2]; int nx= buf[3]; int ny=buf[4];
      if ((x>=0)&&(x<WIN_WIDTH)&&(y>=0)&&(y<WIN_HEIGHT)&&
          (nx>=0)&&(nx<WIN_WIDTH)&&(ny>=0)&&(ny<WIN_HEIGHT) ){
        if (!inside){
          inside= true;
          glBegin(GL_LINES); 
          regime= LINE; 
        } else if (inside && (regime==POINT)){
          glEnd();
          glBegin(GL_LINES);
          regime= LINE; 
        } 
        assert (inside && (regime==LINE));
        glVertex2i(x,y);
        glVertex2i(nx,ny);
      }else{
        // both ends should be in the window (skipped otherwise)
      }
      buflen= 0; // clear the buffer
    }
    numread= read(PIPEREAD, &inp, sizeof(int));
  }
  if (inside){
    glEnd();
    inside= false;
    glFlush(); // update the screen
  }  
  if (numread==-1){
    assert(errno == EAGAIN); // pipe is empty
  }else if (numread==0){
    if (!pipe_closed){
      // printf("Pipe closed\n"); // for pipe debugging
      if (buflen!=0){
        printf("Error: pipe closed when buffer not empty\n");
      }  
      pipe_closed= true;
    }  
  }else{
    assert(false); // This should not happen
  }
}
 
void main_child(int argc, char* argv[]){
  // printf("Started\n");
  glutInit(&argc, argv); // could extract options from the command line 
                         //https://www.opengl.org/resources/libraries/glut/spec3/node10.html
  glutInitWindowSize(WIN_WIDTH,WIN_HEIGHT);
  glutCreateWindow("Graph Data");
  glutDisplayFunc(display); // Register display callback handler for window re-paint
  glutMouseFunc(NULL);
  glutIdleFunc(IdleEvent);
  glClearColor(0.3f, 0.3f, 0.3f, 1.0f); // Set background color to black and opaque
  glColor3f(1.0f, 1.0f, 0.6f); // can be updated at any time
  glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer (background)
  gluOrtho2D( 0, WIN_WIDTH, 0, WIN_HEIGHT);
  glPointSize(POINT_SIZE);
  glLineWidth(LINE_WIDTH);
  glutMainLoop();
}


