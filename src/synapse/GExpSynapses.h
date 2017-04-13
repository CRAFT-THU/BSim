/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GEXPSYNAPSES_H
#define GEXPSYNAPSES_H

#include "../utils/macros.h"
#include "../base/type.h"

struct GExpSynapses {
	real *p_weight;
	int *p_delay_steps;

	int *p_active_steps;

	real *p_C1;
	real *p__C1;
	real *p_I_syn;
	
	int *pDst;
};

SYNAPSE_GPU_FUNC_DEFINE(Exp)

#endif /* GEXPSYNAPSES_H */

