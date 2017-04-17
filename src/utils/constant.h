/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#include <stddef.h>

#define MPI_CREAL MPI_FLOAT

//typedef float real;
typedef double real;

typedef int ID;

const real ZERO = 1e-10;

const int MAXBLOCKSIZE = 1024;
const int WARPSIZE = 32;

const int DECAY_MULTIPLE_TAU = 5;

#endif /* CONSTANT_H */

