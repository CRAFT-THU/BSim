#ifndef GSTATICSYNAPSES_H
#define GSTATICSYNAPSES_H

#include "../base/type.h"
#include "../utils/macros.h"

struct GStaticSynapses {
	real *p_weight;
	int *pDst;
};

SYNAPSE_GPU_FUNC_DEFINE(Static)

#endif /* GSTATICSYNAPSES_H */
