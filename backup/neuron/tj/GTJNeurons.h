#ifndef GTJNEURONS_H
#define GTJNEURONS_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GTJNeurons {
	real *p_vm;
	real *p_v_thresh;
	real *p_v_reset;
	real *p_i_tmp;
	real *p_cm;
	int *p_refrac_step;
	int *p_refrac_time;
};

NEURON_GPU_FUNC_DEFINE(TJ)

#endif /* GTJENEURONS_H */
