#include "../general/general.h"
// assumes that long ints are at least 64 bits and ints are at least 32 bits

// pseudorandom generators are identified by their "labels" of type PRG (currently int type)
// that are used to access them in different ways 
// generators are never destroyed and labels are never reassigned (at least for now)

typedef int PRG;

// GENERATOR TYPES
#define GEN_FILE 0
#define GEN_XOR 1
#define GEN_BAD  2
// #define GEN_OTHER 3
// #define GEN_YET_ANOTHER 4

// ...
// here for every generator its type could be defined

#define MAX_NUM_PRG 30 // maximal allowed number of different registered PRG

//  HOW TO CREATE A GENERATOR

bool g_create_file (PRG *new_generator, char *filename);
// create new generator that will get bits from the given file
// several generators can be attached to the same file; they open the file independently
// (and generate the same stream, each having its own position in the file)
// *new_generator= label; true returned if no error


#define INIT_LENGTH 10 
bool g_create_synthetic (PRG *new_generator, int *init);
// create new generator of pseudo-random bits; init is an array of integers of size INIT_LENGTH;
// where init[0] is the type of the generator, and the rest of init[] is used as seed


bool g_create_xor (PRG *new_generator, PRG gen1, PRG gen2);
// create a new generator that is XOR of two existing generators with labels gen1, gen2
// *new_generator= its label
// It calls the existing generators (like any other calls) and combines their output
// the existing generators remain available and calls to them can be
// interleaved with calls to XOR-generator and never use the same bits


// TODO: use some attached devices as generators, use transformed WAV files


// HOW TO ACCESS THE BIT STREAM

// Access to the generator as a byte stream

bool g_byte (byt *b, PRG gen);
// *b= the next byte from generator with label gen
// for now the main access unit is byte; all other ways of getting 
// bits take bytes from the byte stream (e.g. file)

bool g_int32_lsb(unsigned int *i, PRG gen);
// *i= the next four bytes as LSB integer (first + 256*second +...)

bool g_int16_lsb(unsigned int *i, PRG gen);
// *i= the next two bytes as LSB integer (first + 256* second)


// Access to the generator as a bit stream

bool g_getbit(bool *b, PRG gen);
// *b = next bit from the generator (considered as concatenated sequence of 32-bit integers
// that are read from left to right (not bytes!)

bool g_getword(unsigned *w, int wlen, PRG gen);
// *w = next wlen(<32) bits from PRG placed in the least significant bits (in the same order
// as in the integers in the stream); equivalent to using getbit wlen times

// WARNING: The different types of request could be mixed 
// (no bit in included in the output twice), but bit and word
// requests uses static buffers and this affects the output bit ordering

// Statistics

unsigned long g_num_read (PRG gen);
// number of bytes obtained from the generator gen (in all direct/indirect ways)
