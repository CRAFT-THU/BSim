/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#include <string.h>

#define MPI_CREAL MPI_FLOAT
typedef float real;
const real ZERO = 1e-10;

const int MAXBLOCKSIZE = 1024;
const int WARPSIZE = 32;

enum Type { 
	Constant = 0, LIF = 1, 
	/*Basic, Alpha,*/ Exp, 
	Possion, Input,
	/*Nengo, Probe, 
	Lowpass,*/ 
	TypeSize
}; 

enum SpikeType { Excitatory = 0, Inhibitory = 1, None = 2};

#endif /* CONSTANT_H */

