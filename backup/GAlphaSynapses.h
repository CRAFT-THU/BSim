/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GALPHASYNAPSES_H
#define GALPHASYNAPSES_H

#include "../utils/macros.h"
#include "../base/constant.h"

struct GAlphaSynapses {
	real *p_weight;
	int *p_delay_steps;
	real *p_C1;
	real *p_C2;
	real *p__C1;
	real *p__C2;
	real *p_I_syn;
	real *p_I_tmp;
	
	int *pDst;
};

SYNAPSE_GPU_FUNC_DEFINE(Alpha)

#endif /* GALPHASYNAPSES_H */

