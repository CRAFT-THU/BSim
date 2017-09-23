/* This header file is writen by qp09
 * usually just for fun
 * Fri December 23 2016
 */
#ifndef GMEMNEURONS_H
#define GMEMNEURONS_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GMemNeurons {
	real *p_fire_rate;
	int *p_fire_count;
};

NEURON_GPU_FUNC_DEFINE(Mem)

#endif /* GMEMNEURONS_H */

