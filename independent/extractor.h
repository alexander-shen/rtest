// using extract.c library to generate a file out of two presumably independent ones.
//
// Each file is considered as a sequence of 2-byte samples; each sample in converted to one byte by xor 
// (seems reasonable for 16 bit audio files)

#include "extract.h"
typedef unsigned char byte;
