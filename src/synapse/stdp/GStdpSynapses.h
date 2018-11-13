#ifndef GSTDPSYNAPSES_H
#define GSTDPSYNAPSES_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GStdpSynapses {
	real *p_weight;
	//int *p_delay;
	real *p_apre;
	real *p_apost;
	real *p_d_apre;
	real *p_d_apost;
	real *p_tau_pre;
	real *p_tau_post;
	int *p_last_update;
	int *p_src;
	int *p_dst;
};

SYNAPSE_GPU_FUNC_DEFINE(Stdp)

#endif /* GSTDPSYNAPSES_H */
