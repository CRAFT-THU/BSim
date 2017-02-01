/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GLIFNEURON_H
#define GLIFNEURON_H

#include "../base/constant.h"
#include "../utils/macros.h"

struct GLIFNeurons {
	real *p_v_reset;
	real *p_v_thresh;
	real *p_i_syn;
	real *p_vm;
	real *p_C1;
	real *p_C2;
	real *p_i_tmp;
	int *p_refrac_step;
	int *p_refrac_time;
};

NEURON_GPU_FUNC_DEFINE(LIF)

#endif /* GLIFNEURON_H */
