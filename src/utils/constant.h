/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#include <stddef.h>

#define MPI_CREAL MPI_FLOAT

#ifndef USE_DOUBLE
typedef float real;
#else
typedef double real;
#endif

typedef int ID;

const real ZERO = 1e-10;

const int MAX_BLOCK_SIZE = 1024;
const int WARP_SIZE = 32;

const int DECAY_MULTIPLE_TAU = 5;

#endif /* CONSTANT_H */

