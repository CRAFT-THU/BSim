/* This header file is writen by qp09
 * usually just for fun
 * Fri December 23 2016
 */
#ifndef GCONSTANTNEURONS_H
#define GCONSTANTNEURONS_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GConstantNeurons {
	real *p_fire_rate;
	int *p_fire_count;
};

NEURON_GPU_FUNC_DEFINE(Constant)

#endif /* GCONSTANTNEURONS_H */

