// template from http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/ used
// see also http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html


/**
 * Read and parse a wave file, get data from it and show it as points (for mono [-m] two consequtive
 * data are shown as x and y, for stereo [-s] two readings from the same sample. For [-y] the 1-sample lag
 * in the right channel that happens in Yamaha MW10 is taken into account and corrected
 *
 **/

#include "wave.h"
#include "graph_pipe.h"

int num_channels;

// WAVE header structure
struct HEADER header;

unsigned char buffer4[4];
unsigned char buffer2[2];
FILE *ptr;
bool opened;
char *filename;
bool header_parsed= false;

// minimal and maximal values for sound data
long low_limit;
long high_limit;

long num_samples, size_of_each_tick;

// for stereo case
// get two next measurements from the file as signed integers
// returns true if successful 
// no shift corrections
bool getvalues(int* x, int*y){ 
  assert (header_parsed);
  assert (header.channels==2);
  unsigned char data_buffer[size_of_each_tick]; // data for one measurement (tick)
  assert (size_of_each_tick % header.channels == 0);
  int bytes_in_each_channel = (size_of_each_tick / header.channels);
  int read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
  if (read!=1) return(false);
  unsigned int xchannels = 0;
  int data_in_channel = 0;
  int offset = 0; // move the offset for every iteration in the loop below
  for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
    // convert data from little endian to big endian based on bytes in each channel sample
    if (bytes_in_each_channel == 4) {
      data_in_channel = (int) ( ((unsigned int)data_buffer[offset])              |
                                ( ((unsigned int) data_buffer[offset+1]) <<  8)  | 
                                ( ((unsigned int) data_buffer[offset+2]) << 16)  | 
                                ( ((unsigned int) (int) (char) data_buffer[offset+3]) << 24)  );
    } else if (bytes_in_each_channel == 3) {
      data_in_channel = (int) (  ((unsigned int) data_buffer[offset])          |
                                 (((unsigned int) data_buffer[offset+1]) << 8) |
                                 (((unsigned int) (int) (char) data_buffer[offset+2]) <<16)  );
    } else if (bytes_in_each_channel == 2) {
      data_in_channel = (int) ( ((unsigned int) data_buffer[offset])            |
                                (((unsigned int) (int) (char) data_buffer[offset + 1]) << 8) );
    } else if (bytes_in_each_channel == 1) {
      data_in_channel = (int) ((char) data_buffer[offset]);
    } else {
      assert(false);
    }
    assert ((data_in_channel>=low_limit) && (data_in_channel<=high_limit));
    offset += bytes_in_each_channel;	
    if (xchannels==0){*x = data_in_channel;}else{*y= data_in_channel;}	
  }
  return(true);
}

// for mono case
// get next measurement from the file as signed integer
// returns true if successful 
bool getvalue(int* x){ 
  assert (header_parsed);
  assert (header.channels==1);
  unsigned char data_buffer[size_of_each_tick]; // data for one measurement (tick)
  assert (size_of_each_tick % header.channels == 0);
  int bytes_in_each_channel = (size_of_each_tick / header.channels);
  int read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
  if (read!=1) return(false);
  unsigned int xchannels = 0;
  int data_in_channel = 0;
  int offset = 0; // move the offset for every iteration in the loop below
  for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
    // convert data from little endian to big endian based on bytes in each channel sample
    if (bytes_in_each_channel == 4) {
      data_in_channel = (int) ( ((unsigned int)data_buffer[offset])              |
                                ( ((unsigned int) data_buffer[offset+1]) <<  8)  | 
                                ( ((unsigned int) data_buffer[offset+2]) << 16)  | 
                                ( ((unsigned int) (int) (char) data_buffer[offset+3]) << 24)  );
    } else if (bytes_in_each_channel == 3) {
      data_in_channel = (int) (  ((unsigned int) data_buffer[offset])          |
                                 (((unsigned int) data_buffer[offset+1]) << 8) |
                                 (((unsigned int)(int)(char) data_buffer[offset+2]) <<16)  );
    } else if (bytes_in_each_channel == 2) {
      data_in_channel = (int) ( ((unsigned int) data_buffer[offset])            |
                                (((unsigned int) (int) (char) data_buffer[offset + 1]) << 8) );
    } else if (bytes_in_each_channel == 1) {
      data_in_channel = (int) ((char) data_buffer[offset]);
    } else {
      assert(false);
    }  
    if (!((data_in_channel>=low_limit) && (data_in_channel<=high_limit))){printf("data: %d\n",data_in_channel);}
    assert ((data_in_channel>=low_limit) && (data_in_channel<=high_limit));
    offset += bytes_in_each_channel;	
    *x = data_in_channel;
  }
  return(true);
}

int scmp(char* test, char *etalon){
   int k=0;
   while ((etalon[k]!='\0')&&(etalon[k]==test[k])){k++;}
   return (!(etalon[k]=='\0'));
}


// opens file, returns true if success
bool readwav_start(char *filename){
  assert (sizeof(long)>=8);
  assert (sizeof(int)==4);
  ptr = fopen(filename, "rb");
  if (ptr == NULL){
    printf("Error: no file %s\n", filename);
    fflush(stdout);
    opened= false; 
    return(false);
  }
  int read;
  // read header parts
  
  read = fread(header.riff, sizeof(header.riff), 1, ptr);
  //(1-4 bytes should be literally [RIFF]
  if ((read !=1) || (strcmp(header.riff,"RIFF")!=0)){
    printf ("Error: not a RIFF file %s\n", filename); 
    fflush(stdout);
    opened=false;
    return(false);
  }  
  
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  //(5-8, length of the RIFF data in bytes, LSB..MSB: %u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
  // the number of remaining bytes 
  // (so normally it is 8 bytes less than the raw file size) 
  // this is 32bit unsigned integer (lsb...msb in the byte sequence) 
  // max size of wav file is about 4Gb
  // convert little endian to big endian 4 byte int
  if (read!=1){
    printf("Error reading RIFF length in file %s\n",filename);
    fflush(stdout);
    opened=false;
    return(false);
  }     
  header.overall_size  = buffer4[0] | (buffer4[1]<<8) | (buffer4[2]<<16) | (buffer4[3]<<24);
  // length converted to a 32 bit unsigned int

  read = fread(header.wave, sizeof(header.wave), 1, ptr);
  // (9-12, should be literally [WAVE])
  if (scmp(header.wave,"WAVE")!=0){printf ("Error: not a WAVE file\n"); exit(1);}
  if ((read !=1) || (scmp(header.wave,"WAVE")!=0)){
    printf ("Error: not a WAVE file %s\n", filename); 
    fflush(stdout);
    opened=false;
    return(false);
  } 

  read = fread(header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), 1, ptr);
  // (13-16, should be literally [fmt ])
  if ((read !=1) || (scmp(header.fmt_chunk_marker,"fmt ")!=0)){
    printf ("Error: bat fmt marker in file %s\n", filename); 
    fflush(stdout);
    opened=false;
    return(false);
  } 
  
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  // (17-20, length of format description, 4 bytes, LSB..MSB, should be 16)
  // there are also versions of format that use  18 or 40 bytes, not 16, but they are not covered
  // convert little endian to big endian 4 byte integer
  header.length_of_fmt = buffer4[0] | (buffer4[1] << 8) | (buffer4[2] << 16) | (buffer4[3] << 24);
  if ((read!=1)||(header.length_of_fmt!=16)){
    printf("Error: format description should be 16 bytes in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
 
  read = fread(buffer2, sizeof(buffer2), 1, ptr); 
  // (21-22, Format type and name: should contain 1 for PCM)
  header.format_type = buffer2[0] | (buffer2[1] << 8);
  if ((read!=1)||(header.format_type!=1)){
    printf("Error: not PCM file type in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  
  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  // (23-24, number of channels, LSB, MSB)
  header.channels = buffer2[0] | (buffer2[1] << 8);
  if ((read!=1)||(header.channels<1)||(header.channels>2)){
    printf("Error: expected 1 or 2 channels in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }

  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  // (25-28, sample rate, 4 bytes, LSB..MSB)
  if (read!=1){
    printf("Error reading sample rate in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  header.sample_rate= buffer4[0]|(buffer4[1]<<8)|(buffer4[2]<<16)|(buffer4[3]<<24);
  // number of samples per second (and per channel)
  // any sample rate is OK for now
  
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  // (29-32, byte rate, 4 bytes, LSB..MSB)
  if (read!=1){
    printf("Error reading byte rate in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  header.byterate= buffer4[0]|(buffer4[1]<<8)|(buffer4[2]<<16)|(buffer4[3]<<24);
  // any byte rate is OK for now
  
  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  //(33-34, block alignment, LSB, MSB)
  if (read!=1){
    printf("Error reading block alignment in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  header.block_align = buffer2[0] | (buffer2[1] << 8);
  // any block alignment OK for now

  read = fread(buffer2, sizeof(buffer2), 1, ptr);
  //(35-36, bits per sample
  header.bits_per_sample = buffer2[0] | (buffer2[1] << 8);
  // each sample should fill an integer number of bytes 
  if ((read!=1) || ((header.bits_per_sample/8)*8!=header.bits_per_sample)){
    printf("Error in bits_per_sample in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  // the valid amplitude range for values based on the bits per sample
  switch (header.bits_per_sample) {
    case 8:
      low_limit = -128; high_limit = 127; break;
    case 16:
      low_limit = -32768; high_limit = 32767; break;
    case 24:
      low_limit = -8388608; high_limit = 8388607; break;	
    case 32: 
      low_limit = -2147483648; high_limit = 2147483647; break;
    default: 
      printf("Error: number of bits per sample is not 8,16,24,32 in %s\n", filename); 
      fflush(stdout);
      opened= false; 
      return(false);
  }
  // End of format description block reached
    
  // Consistency check: sample rate * number of channels * bit per sample = 8* byte rate
  if ((unsigned long)header.byterate * 8 != ((unsigned long) header.sample_rate) *
                                       ((unsigned long) header.bits_per_sample) *
                                       ((unsigned long) header.channels)) {
    printf ("Inconsistent rate parameters in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);                                  
  }
 
 // now data header
 
  read = fread(header.data_chunk_header, sizeof(header.data_chunk_header), 1, ptr);
  // (37-40, data marker, should be literally [data])
  if ((read!=1)||(scmp(header.data_chunk_header,"data")!=0)){
    printf ("Incorrect data marker in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);                                  
  }
  
  read = fread(buffer4, sizeof(buffer4), 1, ptr);
  // (41-44, size of data chunks as 4 bytes: LSB..MSB)
  header.data_size= buffer4[0]|(buffer4[1]<<8)|(buffer4[2]<<16)|(buffer4[3]<<24);
  // consistency check:
  if((unsigned long)header.data_size+36 != (unsigned long)header.overall_size){
    printf ("Inconsistent length data in %s\n", filename); 
    //fflush(stdout);
    //opened= false; 
    //return(false);                                  
  }

  // calculate no.of samples
  num_samples = (8 * (long)header.data_size) / (header.channels * header.bits_per_sample);
  // Number of ticks (samples for different channels count as one tick)
  size_of_each_tick = (header.channels * header.bits_per_sample) / 8;
  // Size of sample data for all channels in bytes
  
  header_parsed= true;
  return(true);
}

#define CONVERTED_X(z) (int)( ((double)WIN_WIDTH-1)*((double)(z-low_limit))/((double)(high_limit-low_limit)))
#define CONVERTED_Y(z) (int)( ((double)WIN_HEIGHT-1)*((double)(z-low_limit))/((double)(high_limit-low_limit)))
// options: -m mono -s stereo -y stereo with yamaha MW10 correction (taking into account 1 value delay in the right channel)
// only one of them should be used

// main program
void main_graph(int argc, char* argv[]) { 

  opterr= 0;
  int c; 
  int optm=0; int opts=0; int opty=0;
  while ((c= getopt(argc, argv,"msy"))!=-1){
    switch(c){
    case 'm': optm=1; break;
    case 's': opts=1; break;
    case 'y': opty=1; break;    
    case '?':
      if (isprint(optopt)){
        printf("Error: option -%c invalid\n", optopt);                                                                                                               
        printf ("Usage: %s [-m(mono),-s(stereo),-y(yamaha mw10 corrected stereo)] filename.wav\n", argv[0]);
      }else{
        printf ("Unknown option character `\\x%x'.\n",optopt);
      }
      close(PIPEWRITE);
      exit(1);
    default: assert(false);
    }    
  }
  // printf("argc = %d, optind = %d, optm = %d, opts = %d, opty = %d\n", argc, optind, optm, opts, opty);
  if ((optind!=argc-1)||(optm+opts+opty!=1)){
    printf ("Usage: %s -m(mono),-s(stereo),-y(yamaha mw10 corrected stereo) filename.wav\n", argv[0]);
    close(PIPEWRITE); exit(1);
  }	

  filename = (char*) malloc(sizeof(char) * 1024);
  if (filename == NULL) {
    printf("Error in malloc\n");
    close(PIPEWRITE); exit(1);
  }
  // get file path
  char cwd[1024]; // current work directory
  if (getcwd(cwd, sizeof(cwd)) != NULL) {  
    strcpy(filename, cwd);
    strcat(filename, "/");
    strcat(filename, argv[optind]);
  }else{
    printf("Error while reading current work directory\n");
    close(PIPEWRITE); exit(1);
  }
   
  // processing the file and displaying the data
  if (!readwav_start(filename)){
    printf("Error in parsing %s\n",filename);
    close(PIPEWRITE); exit(1);
  }
  int x,y;

  if (optm==1){
    if (header.channels!=1){
      printf("File %s is not a mono file\n", filename);
      close(PIPEWRITE); exit(1);
    }
    bool ok=getvalue(&x); 
    if (ok){ok=getvalue(&y);}
    while (ok){
      graph_draw(CONVERTED_X(x),CONVERTED_Y(y));
      ok=getvalue(&x); 
      if (ok){ok=getvalue(&y);}
    }
  } else if (opts==1){
    if (header.channels!=2){
      printf("File %s is not a stereo file\n", filename);
      close(PIPEWRITE); exit(1);
    }
    while (getvalues(&x,&y)){
      graph_draw(CONVERTED_X(x),CONVERTED_Y(y));
    }   
  } else if (opty==1){
    if (header.channels!=2){
      printf("File %s is not a stereo file\n", filename);
      close(PIPEWRITE); exit(1);
    }
    int oldx,oldy;
    bool ok=getvalues(&oldx,&oldy);
    while (ok & getvalues(&x,&y)){
      graph_draw(CONVERTED_X(oldx),CONVERTED_Y(y));
      oldx= x; oldy= y;
    }
  }else{
    assert(false);
  }    
  // Closing file
  fclose(ptr);
  // cleanup before quitting
  free(filename);
  exit(0);
}
