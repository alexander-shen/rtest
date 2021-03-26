// template from http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/ used
// see also http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

/**
 * Read and parse a wave file, get data from it
 * getdatawav [-s -y -m] source.wav dest1 [dest2] : extracts raw data from source.wav
 * and produces one (for mono) or two (for stereo) files dest1 and dest 2 
 * with no changes in the byte sequence (LSB..MSB is the same as in the wav file) 
 * For [-y] the 1-sample lag in the right channel that happens in Yamaha MW10 
 * is taken into account and corrected
 *
 **/


#include "wave.h"

int num_channels;

// WAVE header structure
struct HEADER header;

typedef unsigned char byte;
byte buffer4[4];
byte buffer2[2];
FILE *in, *out1, *out2;
bool opened;
char *infile, *outfile1, *outfile2; // for wav file and two output data files 
bool header_parsed= false;

long num_samples, size_of_each_tick;

// for stereo case
// get two next measurements from the file as array of bytes
// returns true if successful 
// no shift corrections
bool getbytes2(byte *x, byte *y){//array of bytes where data should be placed 
  assert (header_parsed);
  assert (header.channels==2);
  byte data_buffer[size_of_each_tick]; // data for one measurement (tick)
  assert (size_of_each_tick % header.channels == 0);
  int bytes_in_each_channel = (size_of_each_tick / header.channels);
  int read = fread(data_buffer, sizeof(data_buffer), 1, in);
  if (read!=1) return(false);
  int offset = 0; // move the offset for every iteration in the loop below
  for (int k = 0; k < header.channels; k++ ) {
    // convert data from little endian to big endian based on bytes in each channel sample
    for (int i=0; i<bytes_in_each_channel;i++){
      (k==0?x:y)[i]=data_buffer[offset+i];
    }
    offset += bytes_in_each_channel;	
  }
  return(true);
}

// for mono case
// get next measurement from the file as an array of bytes
// returns true if successful 
bool getbytes(byte* x){ 
  assert (header_parsed);
  assert (header.channels==1);
  byte data_buffer[size_of_each_tick]; // data for one measurement (tick)
  assert (size_of_each_tick % header.channels == 0);
  int bytes_in_each_channel = (size_of_each_tick / header.channels);
  int read = fread(data_buffer, sizeof(data_buffer), 1, in);
  if (read!=1) return(false);
  for (int i=0; i<bytes_in_each_channel; i++){
    x[i]=data_buffer[i];
  }
  return(true);
}

int scmp(char* test, char *etalon){
   int k=0;
   while ((etalon[k]!='\0')&&(etalon[k]==test[k])){k++;}
   return (!(etalon[k]=='\0'));
}


// opens wav file, returns true if success
bool readwav_start(char *filename){
  assert (sizeof(long)>=8);
  assert (sizeof(int)==4);
  in = fopen(filename, "rb");
  if (in == NULL){
    printf("Error: no file %s\n", filename);
    fflush(stdout);
    opened= false; 
    return(false);
  }
  int read;
  // read header parts
  
  read = fread(header.riff, sizeof(header.riff), 1, in);
  //(1-4 bytes should be literally [RIFF]
  if ((read !=1) || (strcmp(header.riff,"RIFF")!=0)){
    printf ("Error: not a RIFF file %s\n", filename); 
    fflush(stdout);
    opened=false;
    return(false);
  }  
  
  read = fread(buffer4, sizeof(buffer4), 1, in);
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

  read = fread(header.wave, sizeof(header.wave), 1, in);
  // (9-12, should be literally [WAVE])
  if (scmp(header.wave,"WAVE")!=0){printf ("Error: not a WAVE file\n"); exit(1);}
  if ((read !=1) || (scmp(header.wave,"WAVE")!=0)){
    printf ("Error: not a WAVE file %s\n", filename); 
    fflush(stdout);
    opened=false;
    return(false);
  } 

  read = fread(header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), 1, in);
  // (13-16, should be literally [fmt ])
  if ((read !=1) || (scmp(header.fmt_chunk_marker,"fmt ")!=0)){
    printf ("Error: bat fmt marker in file %s\n", filename); 
    fflush(stdout);
    opened=false;
    return(false);
  } 
  
  read = fread(buffer4, sizeof(buffer4), 1, in);
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
 
  read = fread(buffer2, sizeof(buffer2), 1, in); 
  // (21-22, Format type and name: should contain 1 for PCM)
  header.format_type = buffer2[0] | (buffer2[1] << 8);
  if ((read!=1)||(header.format_type!=1)){
    printf("Error: not PCM file type in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  
  read = fread(buffer2, sizeof(buffer2), 1, in);
  // (23-24, number of channels, LSB, MSB)
  header.channels = buffer2[0] | (buffer2[1] << 8);
  if ((read!=1)||(header.channels<1)||(header.channels>2)){
    printf("Error: expected 1 or 2 channels in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }

  read = fread(buffer4, sizeof(buffer4), 1, in);
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
  
  read = fread(buffer4, sizeof(buffer4), 1, in);
  // (29-32, byte rate, 4 bytes, LSB..MSB)
  if (read!=1){
    printf("Error reading byte rate in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  header.byterate= buffer4[0]|(buffer4[1]<<8)|(buffer4[2]<<16)|(buffer4[3]<<24);
  // any byte rate is OK for now
  
  read = fread(buffer2, sizeof(buffer2), 1, in);
  //(33-34, block alignment, LSB, MSB)
  if (read!=1){
    printf("Error reading block alignment in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);
  }
  header.block_align = buffer2[0] | (buffer2[1] << 8);
  // any block alignment OK for now

  read = fread(buffer2, sizeof(buffer2), 1, in);
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
  if (!((header.bits_per_sample==8)||
          (header.bits_per_sample==16)||
          (header.bits_per_sample==24)||
          (header.bits_per_sample==32))){
    printf ("Invalid number of bits per sample in %s (should be 8,16,24, or 32)\n", filename); 
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
 
  read = fread(header.data_chunk_header, sizeof(header.data_chunk_header), 1, in);
  // (37-40, data marker, should be literally [data])
  if ((read!=1)||(scmp(header.data_chunk_header,"data")!=0)){
    printf ("Incorrect data marker in %s\n", filename); 
    fflush(stdout);
    opened= false; 
    return(false);                                  
  }
  
  read = fread(buffer4, sizeof(buffer4), 1, in);
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

// options: -m mono -s stereo -y stereo with yamaha MW10 correction 
// (taking into account 1 value delay in the right channel)
// only one of them should be used

// main program
int main(int argc, char* argv[]) { 

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
        printf ("Usage: %s [-m(mono),-s(stereo),-y(yamaha mw10 corrected stereo)] infile.wav outfile1 [outfile2]\n", argv[0]);
      }else{
        printf ("Unknown option character `\\x%x'.\n",optopt);
      }
      exit(1);
    default: assert(false);
    }    
  }
  if (optm+opts+opty!=1){
    printf ("Usage: %s [-m(mono),-s(stereo),-y(yamaha mw10 corrected stereo)] infile.wav outfile1 [outfile2]\n", argv[0]);
    printf("Only one of -m, -s, -y options could be used\n");
    exit(1);
  }
  if (optm==1){
    if (optind!=argc-2){
      printf ("Usage: %s -m infile.wav outfile1\n",argv[0]);
      exit(1);
    }
  }  
  if (opts+opty==1){
    if (optind!=argc-3){
      printf ("Usage: %s -s|-y infile.wav outfile1 outfile2\n", argv[0]);
      exit(1);
    }
  } 
  
  infile= argv[optind]; // copying the pointer
  outfile1= argv[optind+1];
  if (opts+opty==1){ outfile2= argv[optind+2]; } 
   
  // processing the file and displaying the data
  if (!readwav_start(infile)){
    printf("Error in parsing %s\n",infile);
    exit(1);
  }

  byte buffer1[4], buffer2[4]; // buffers for two channels, maximal length 4 bytes
  int bytes_in_each_channel = (size_of_each_tick / header.channels);
  if (optm==1){
    if (header.channels!=1){
      printf("File %s is not a mono file\n", infile);
      exit(1);
    }
    out1 = fopen(outfile1, "wb");
    if (out1 == NULL){
      printf("Error: cannot open %s for writing\n", outfile1);
      fflush(stdout);
      exit(1);
    }     
    while (getbytes(buffer1)){
      for (int i=0; i<bytes_in_each_channel; i++){fputc(buffer1[i],out1);}
    }
    fclose(out1);
  } else if (opts==1){
    if (header.channels!=2){
      printf("File %s is not a stereo file\n", infile);
      exit(1);
    }
    out1 = fopen(outfile1, "wb");
    if (out1 == NULL){
      printf("Error: cannot open %s for writing\n", outfile1);
      fflush(stdout);
      exit(1);
    } 
    out2 = fopen(outfile2, "wb");
    if (out2 == NULL){
      printf("Error: cannot open %s for writing\n", outfile2);
      fflush(stdout);
      exit(1);
    }   
    while (getbytes2(buffer1,buffer2)){
      for (int i=0; i<bytes_in_each_channel; i++){
        fputc(buffer1[i],out1);
        fputc(buffer2[i],out2);
      }   
    }   
    fclose(out1); fclose(out2);
  } else if (opty==1){
    if (header.channels!=2){
      printf("File %s is not a stereo file\n", infile);
      exit(1);
    }
    out1 = fopen(outfile1, "wb");
    if (out1 == NULL){
      printf("Error: cannot open %s for writing\n", outfile1);
      fflush(stdout);
      exit(1);
    } 
    out2 = fopen(outfile2, "wb");
    if (out2 == NULL){
      printf("Error: cannot open %s for writing\n", outfile2);
      fflush(stdout);
      exit(1);
    } 
    bool first_sample= true;
    while (getbytes2(buffer1,buffer2)){
      for (int i=0; i<bytes_in_each_channel; i++){
        fputc(buffer1[i],out1);
        if(!first_sample){fputc(buffer2[i],out2);}
      }
      first_sample= false;     
    }
    fclose(out1); fclose(out2);
  }else{
    assert(false);
  }    
  // Closing input file
  fclose(in);
  exit(0);
}
