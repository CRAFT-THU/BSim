/* This header file is writen by qp09
 * usually just for fun
 * Fri December 23 2016
 */
#ifndef GDECIDENEURONS_H
#define GDECIDENEURONS_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GDecideNeurons {
	real *p_tmp_rate;
	real *p_fire_rate;
	int *p_fire_count;
};

NEURON_GPU_FUNC_DEFINE(Decide)

#endif /* GDECIDENEURONS_H */

