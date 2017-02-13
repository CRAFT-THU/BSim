/* This header file is writen by qp09
 * usually just for fun
 * Thu February 02 2017
 */
#ifndef GPOSSIONNEURONS_H
#define GPOSSIONNEURONS_H

#include <curand_kernel.h>
#include <curand.h>

#include "../base/constant.h"
#include "../utils/macros.h"

struct GPossionNeurons {
	real *p_rate;
	int *p_fire_cycle;
	int *p_end_cycle;
	int *p_refrac_step;
	curandState *p_state;
};

NEURON_GPU_FUNC_DEFINE(Possion)

#endif /* GPOSSIONNEURONS_H */

