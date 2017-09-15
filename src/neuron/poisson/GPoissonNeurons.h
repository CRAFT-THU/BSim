/* This header file is writen by qp09
 * usually just for fun
 * Thu February 02 2017
 */
#ifndef GPOISSONNEURONS_H
#define GPOISSONNEURONS_H

#include <curand_kernel.h>
#include <curand.h>

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GPoissonNeurons {
	real *p_rate;
	int *p_fire_cycle;
	int *p_end_cycle;
	int *p_refrac_step;
	curandState *p_state;
};

NEURON_GPU_FUNC_DEFINE(Poisson)

#endif /* GPOISSONNEURONS_H */

