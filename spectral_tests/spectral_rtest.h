#include <stdexcept>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>

#include "specgraph.h"
#include "rand.h"

typedef unsigned char byt;
typedef bool bytegen(byt *b);

bool process_graph (bytegen random_source, 
                    long double gap, long double error, 
                    long size, long degree, long iterations,
                    long double *lower, long double *upper);
