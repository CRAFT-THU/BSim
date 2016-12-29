/* This header file is writen by qp09
 * usually just for fun
 * Fri December 23 2016
 */
#ifndef GCONSTANTNEURONS_H
#define GCONSTANTNEURONS_H

#include "../utils/macros.h"
#include "../base/constant.h"

struct GConstantNeurons {
	real *p_fire_rate;
	int *p_fire_count;

	NEURON_CONNECT_PARA
};

NEURON_GPU_FUNC_DEFINE(Constant)

#endif /* GCONSTANTNEURONS_H */

