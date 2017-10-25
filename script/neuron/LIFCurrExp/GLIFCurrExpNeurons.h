#ifndef GLIFCURREXPNEURONS_H
#define GLIFCURREXPNEURONS_H

#include "../base/type.h"
#include "../utils/macros.h"

struct GLIFCurrExpNeurons {
	real *p_i_offset;
	real *p_i_injection;
	real *p_cm;
	real *p_tau_m;
	real *p_tau_e;
	real *p_tau_i;
	real *p_vm;
	real *p_v_thresh;
	real *p_v_reset;
	real *p_v_reset;
	real *p_i_E;
	real *p_i_I;
	int *p_refrac_time;
	int *p_refrac_step;
};

NEURON_GPU_FUNC_DEFINE(LIFCurrExp)

#endif /* GLIFCURREXPNEURONS_H */
