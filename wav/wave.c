// template from http://truelogic.org/wordpress/2015/09/04/parsing-a-wav-file-in-c/ used
// see also http://www-mmsp.ece.mcgill.ca/Documents/AudioFormats/WAVE/WAVE.html

/**
 * Read and parse a wave file
 *
 **/
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wave.h"
#include "graph_pipe.h"
#define TRUE 1 
#define FALSE 0

#define CONVERTED_X(z) (int)( ((double)WIN_WIDTH-1)*((double)(z-low_limit))/((double)(high_limit-low_limit)))
#define CONVERTED_Y(z) (int)( ((double)WIN_HEIGHT-1)*((double)(z-low_limit))/((double)(high_limit-low_limit)))


// WAVE header structure

unsigned char buffer4[4];
unsigned char buffer2[2];

char* seconds_to_time(float seconds); // defined later in the file


FILE *ptr;
char *filename;
struct HEADER header;

// main program
void main_graph(int argc, char* argv[]) { 

 // we need long to be long enough

 printf("Size of long variables: %ld\n", sizeof (long));
 if (sizeof(long)<8){printf("Error: should be at least 8\n");exit(1);}
 fflush(stdout);
 
 filename = (char*) malloc(sizeof(char) * 1024);
 if (filename == NULL) {
   printf("Error in malloc\n");
   exit(1);
 }

 // get file path
 char cwd[1024];
 if (getcwd(cwd, sizeof(cwd)) != NULL) {
   
	strcpy(filename, cwd);

	// get filename from command line
	if (argc < 2) {
	  printf("No wave file specified\n");
	  exit(1);
	}
	
	strcat(filename, "/");
	strcat(filename, argv[1]);
 }

 // open file
 printf("Opening WAV file:\n %s\n", filename);
 ptr = fopen(filename, "rb");
 if (ptr == NULL) {
	printf("Error opening file\n");
	exit(1);
 }
 
 int read = 0;
 
 // read header parts


 read = fread(header.riff, sizeof(header.riff), 1, ptr);
 printf("(1-4 bytes should be literally [RIFF]): %s \n", header.riff);
 if (strcmp(header.riff,"RIFF")!=0){printf ("Error: not a RIFF file\n"); exit(1);} 
 // first 4 bytes: normally this should contain "RIFF"

 read = fread(buffer4, sizeof(buffer4), 1, ptr);
 printf("(5-8, length of the RIFF data in bytes, LSB..MSB): %u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
 // the number of remaining bytes 
 // (so normally it is 8 bytes less than the raw file size) 
 // this is 32bit unsigned integer (lsb...msb in the byte sequence) 
 // max size of wav file is about 4Gb
 
 // convert little endian to big endian 4 byte int
 header.overall_size  = buffer4[0] | 
						(buffer4[1]<<8) | 
						(buffer4[2]<<16) | 
						(buffer4[3]<<24);
 // length converted to a 32 bit unsigned int

 printf("(5-8 converted, byte size of the data part of RIFF): bytes:%u, Kb:%u \n", header.overall_size, header.overall_size/1024);

 read = fread(header.wave, sizeof(header.wave), 1, ptr);
 printf("(9-12, should be literally [WAVE]):%s\n", header.wave);
  if (strcmp(header.wave,"WAVE")!=0){printf ("Error: not a WAVE file\n"); exit(1);}
 // header of the WAVE part, should contain "WAVE"

 read = fread(header.fmt_chunk_marker, sizeof(header.fmt_chunk_marker), 1, ptr);
 printf("(13-16, should be literally [fmt ]) Fmt marker: [%s]\n", header.fmt_chunk_marker);
  if (strcmp(header.fmt_chunk_marker,"fmt ")!=0){printf ("Error: bad fmt marker\n"); exit(1);}
 // format marker, should be "fmt "

 read = fread(buffer4, sizeof(buffer4), 1, ptr);
 printf("(17-20, length of format description, 4 bytes, LSB..MSB): %u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);
 
 // convert little endian to big endian 4 byte integer
 header.length_of_fmt = buffer4[0] |
							(buffer4[1] << 8) |
							(buffer4[2] << 16) |
							(buffer4[3] << 24);
 printf("((17-20) length of fmt description, converted, should be 16): %u \n", header.length_of_fmt);
 if (header.length_of_fmt!=16){printf("Error: we assume that format description contains 16 bytes\n"); exit(1);}
 // there are also versions of format that use  18 or 40 bytes, not 16, but they are not covered
 
 read = fread(buffer2, sizeof(buffer2), 1, ptr); 
 printf("(21-22, format name code): %u %u\n", buffer2[0], buffer2[1]);
 
 header.format_type = buffer2[0] | (buffer2[1] << 8);
 char format_name[10] = "";
 if (header.format_type == 1)
   strcpy(format_name,"PCM"); 
 else if (header.format_type == 6)
  strcpy(format_name, "A-law");
 else if (header.format_type == 7)
  strcpy(format_name, "Mu-law");
 else{ 
  printf("Error: unknown format\n"); exit(1);
 }
 // there exist also 3 (IEEE float) and 0xFFFE for "WAVE_FORMAT_EXTENSIBLE"
 printf("((21-22) Format type and name): %u %s \n", header.format_type, format_name);
 if (header.format_type!=1){printf("Error: only PCM format supported\n"); exit(1);}
 
 read = fread(buffer2, sizeof(buffer2), 1, ptr);
 printf("(23-24, number of channels, LSB, MSB):%u %u \n", buffer2[0], buffer2[1]);

 header.channels = buffer2[0] | (buffer2[1] << 8);
 printf("((23-24) number of channels): %u \n", header.channels);
// number of interleaved channels

 read = fread(buffer4, sizeof(buffer4), 1, ptr);
 printf("(25-28, sample rate, 4 bytes, LSB..MSB): %u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

 header.sample_rate = buffer4[0] |
						(buffer4[1] << 8) |
						(buffer4[2] << 16) |
						(buffer4[3] << 24);

 printf("((25-28) sample rate): %u\n", header.sample_rate);
// number of samples per second (and per channel)

 read = fread(buffer4, sizeof(buffer4), 1, ptr);
 printf("(29-32, byte rate, 4 bytes, LSB..MSB): %u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

 header.byterate  = buffer4[0] |
						(buffer4[1] << 8) |
						(buffer4[2] << 16) |
						(buffer4[3] << 24);
 printf("(29-32), byte Rate: %u , bit Rate: %u\n", header.byterate, header.byterate*8);

 read = fread(buffer2, sizeof(buffer2), 1, ptr);
 printf("(33-34, block alignment, LSB, MSB): %u %u \n", buffer2[0], buffer2[1]);

 header.block_align = buffer2[0] | (buffer2[1] << 8);
 printf("((33-34), block alignment: %u \n", header.block_align);

 read = fread(buffer2, sizeof(buffer2), 1, ptr);
 printf("(35-36, bits per sample, LSB, MSB): %u %u \n", buffer2[0], buffer2[1]);

 header.bits_per_sample = buffer2[0] | (buffer2[1] << 8);
 printf("((35-36), bits per sample): %u \n", header.bits_per_sample);
 // each sample should fill an integer number of bytes
 if ((header.bits_per_sample/8)*8!=header.bits_per_sample){
   printf("Error: each sample should use integer number of bytes\n"); exit(1);
 }  
 
 printf("End of format description block reached\n");
 
// Consistency check: sample rate * number of channels * bit per sample = 8* byte rate

 if ((unsigned long)header.byterate * 8 != ((unsigned long) header.sample_rate) *
                                       ((unsigned long) header.bits_per_sample) *
                                       ((unsigned long) header.channels)) {
   printf ("Inconsistent rate parameters\n"); exit(1);                                    
 }
 
 read = fread(header.data_chunk_header, sizeof(header.data_chunk_header), 1, ptr);
 printf("(37-40, data marker, should be literally [data]): %s \n", header.data_chunk_header);
 if (strcmp(header.data_chunk_header,"data")!=0){printf("Error: incorrect data header\n");exit(1);}

 read = fread(buffer4, sizeof(buffer4), 1, ptr);
 printf("(41-44, size of data chunks as 4 bytes: LSB..MSB): %u %u %u %u\n", buffer4[0], buffer4[1], buffer4[2], buffer4[3]);

 header.data_size = buffer4[0] |
				(buffer4[1] << 8) |
				(buffer4[2] << 16) | 
				(buffer4[3] << 24 );
 printf("((41-44) size of data chunk in bytes): %u \n", header.data_size);
 // consistency check:
 if ((unsigned long)header.data_size+36 != (unsigned long)header.overall_size){
  printf("Warning: length data inconsistent\n");
 }


 // calculate no.of samples
 long num_samples = (8 * (long)header.data_size) / (header.channels * header.bits_per_sample);
 printf("Number of ticks (samples for different channels count as one):%lu \n", num_samples);

 long size_of_each_tick = (header.channels * header.bits_per_sample) / 8;
 printf("Size of sample data for all channels :%ld bytes\n", size_of_each_tick);

 // calculate duration of file
 float duration_in_seconds = (double) header.overall_size / header.byterate;
 printf("Approx.Duration in seconds=%f\n", duration_in_seconds);
 printf("Approx.Duration in h:m:s=%s\n", seconds_to_time(duration_in_seconds));
 fflush(stdout);

 // read each sample from data chunk if PCM
 #define BUF_MAX_LEN 1024
 long buf[BUF_MAX_LEN];
 int buf_free = 0;
 long processed= 0;
    // printf("Dump sample data? Y/N?");
    //char c = 'y';
    //scanf("%c", &c);
    if (1) { // (c == 'Y' || c == 'y') { 
	long i =0;
	char data_buffer[size_of_each_tick]; // data for one measurement (tick)
	long bytes_in_each_channel = (size_of_each_tick / header.channels);

	// the valid amplitude range for values based on the bits per sample
	long low_limit;
	long high_limit;

	switch (header.bits_per_sample) {
		case 8:
			low_limit = -128;
			high_limit = 127;
			break;
		case 16:
			low_limit = -32768;
			high_limit = 32767;
			break;
		case 24:
			low_limit = -16777216;
			high_limit = 16777215;
			break;	
		case 32:
			low_limit = -2147483648;
			high_limit = 2147483647;
			break;
		default: 
		        printf("Error: number of bits per sample is not 8,16,24,32\n"); 
		        exit(1);	
	}					

	printf("\nValid range for data values : %ld to %ld \n", low_limit, high_limit);
	
	for (i =1; i <= num_samples; i++) {
		// printf("==========Sample %ld / %ld=============\n", i, num_samples);
		read = fread(data_buffer, sizeof(data_buffer), 1, ptr);
		if (read == 1) {
			// dump the data read
			unsigned int  xchannels = 0;
			int data_in_channel = 0;
			int offset = 0; // move the offset for every iteration in the loop below
			for (xchannels = 0; xchannels < header.channels; xchannels ++ ) {
				// printf("Channel#%d : ", (xchannels+1));
				// convert data from little endian to big endian based on bytes in each channel sample
				if (bytes_in_each_channel == 4) {
					data_in_channel = (data_buffer[offset] & 0x00ff) | 
								((data_buffer[offset + 1] & 0x00ff) <<8) | 
								((data_buffer[offset + 2] & 0x00ff) <<16) | 
								(data_buffer[offset + 3] <<24);
				} else if (bytes_in_each_channel == 3) {
					data_in_channel = (data_buffer[offset] & 0x00ff) |
								((data_buffer[offset + 1] & 0x00ff) << 8) | 
								(data_buffer[offset + 2] <<16);
				} else if (bytes_in_each_channel == 2) {
					data_in_channel = (data_buffer[offset] & 0x00ff) |
								(data_buffer[offset + 1] << 8);
				} else if (bytes_in_each_channel == 1) {
					data_in_channel = data_buffer[offset] & 0x00ff;
					data_in_channel -= 128; //in wave, 8-bit are unsigned, so shifting to signed
				}
				offset += bytes_in_each_channel;		
				// printf("%d ", data_in_channel);
				buf[buf_free]= data_in_channel;
				buf_free= (buf_free+1)%BUF_MAX_LEN;
				processed++;
#define SHIFT 1				
				if ((processed>=2*SHIFT+2) && (processed%2==0)){
				   graph_draw(CONVERTED_X(buf[(buf_free+BUF_MAX_LEN-2-2*SHIFT)%BUF_MAX_LEN]),
				              CONVERTED_Y(buf[(buf_free+BUF_MAX_LEN-1)%BUF_MAX_LEN]));
				}
				
				// check if value was in range
				if (data_in_channel < low_limit || data_in_channel > high_limit)
					printf("**value out of range\n");
				// printf(" | ");
			}
                        
                        
                        
                        
			//printf("\n");
		}else{
			printf("Error reading file. %d bytes\n", read);
			break;
		}

	} // 	for (i =1; i <= num_samples; i++) {
    } // if (c == 'Y' || c == 'y') { 


 printf("Closing file..\n");
 fclose(ptr);

  // cleanup before quitting
 free(filename);
}

/**
 * Convert seconds into hh:mm:ss format
 * Params:
 *	seconds - seconds value
 * Returns: hms - formatted string
 **/
 char* seconds_to_time(float raw_seconds) {
  char *hms;
  int hours, hours_residue, minutes, seconds, milliseconds;
  hms = (char*) malloc(100);

  sprintf(hms, "%f", raw_seconds);

  hours = (int) raw_seconds/3600;
  hours_residue = (int) raw_seconds % 3600;
  minutes = hours_residue/60;
  seconds = hours_residue % 60;
  milliseconds = 0;

  // get the decimal part of raw_seconds to get milliseconds
  char *pos;
  pos = strchr(hms, '.');
  int ipos = (int) (pos - hms);
  char decimalpart[15];
  memset(decimalpart, ' ', sizeof(decimalpart));
  strncpy(decimalpart, &hms[ipos+1], 3);
  milliseconds = atoi(decimalpart);	

  
  sprintf(hms, "%d:%d:%d.%d", hours, minutes, seconds, milliseconds);
  return hms;
}

