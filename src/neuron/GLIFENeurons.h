#ifndef GLIFENEURONS_H
#define GLIFENEURONS_H

#include "../base/type.h"
#include "../utils/macros.h"

struct GLIFENeurons {
	real *p_vm;
	real *p_CI;
	real *p_CE;
	real *p_C_I;
	real *p_C_E;
	real *p_v_tmp;
	real *p_i_I;
	real *p_i_E;
	real *p_v_thresh;
	real *p_v_reset;
	real *p_Cm;
	int *p_refrac_step;
	int *p_refrac_time;
	int *p_start_E;
	int *p_start_I;
	int *p_end;
};

NEURON_GPU_FUNC_DEFINE(LIFE)

#endif /* GLIFENEURONS_H */
