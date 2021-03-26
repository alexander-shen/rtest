// ksindep.h
//
// ksindep -b num_bytes -l|-m -n num_samples -s skip_samples -c condition_type infile1 infile2
//
// The program considers infile1 and infile2 as sequence of integer samples; each sample
// contains num_bytes bytes and is interpreted as as integer in LSB first (-l) or MSB first (-m) format.
// The ith sample from infile 1 determines (according to conditions) where (to which sequence) the ith sample from 
// infile2 goes. Ties are resolved using the skipped readings

#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define NUM_CONDITIONS 2
#define MAX_BYTES 4
#define MAX_SAMPLES 20000
#define MAX_SKIPPED 9



