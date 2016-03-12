/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef CONSTANT_H
#define CONSTANT_H

#include <string.h>
typedef float real;
const real ZERO = 1e-10;

enum Type { 
	LIF = 0, Alpha = 1, Exp,
	Nengo, Lowpass, 
	Input, Possion, 
	Probe
}; 

enum SpikeType { Excitatory = 0, Inhibitory = 1, None = 2};

#endif /* CONSTANT_H */

