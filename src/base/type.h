/* This header file is writen by qp09
 * usually just for fun
 * Thu February 16 2017
 */
#ifndef TYPE_H
#define TYPE_H

#include "../utils/constant.h"

enum Type { 
	Constant = 0, Poisson = 1,
	Array,
	Max,
	LIF, 
	Exp, /*Basic, Alpha,*/
	/*Nengo, Probe, Lowpass,*/ 
	TYPESIZE
}; 

enum SpikeType { Excitatory = 0, Inhibitory = 1, SPIKETYPESIZE = 2};

#endif /* TYPE_H */

