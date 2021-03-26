/* ks2.h */

#include "../general/general.h"
#include "ksmirnov.h"

// requires -lgmp -lm while compiling

// exact long integer computation (ks2.mp.c)
double ks2bar(int n0, int n1, long deviation_times_n0_n1);
