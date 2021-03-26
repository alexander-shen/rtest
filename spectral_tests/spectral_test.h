#include <stdexcept>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "specgraph.h"
#include "rand.h"

#define EOF_EXCEPTION 10


void process_graphs (unsigned char (*read_random_source)(), 
                    long double gap, long double error, 
                    long size, long degree, long iterations, long number,
                    bool printlow, bool printup, bool printhash);

