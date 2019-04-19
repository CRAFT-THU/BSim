#ifndef GLIFNEURONS_H
#define GLIFNEURONS_H

#include "../../utils/type.h"
#include "../../utils/macros.h"

struct GLIFNeurons {
	int *p_refrac_step;
	int *p_refrac_time;

	real *pVm;
	real *pCi;
	real *pCe;
	real *pC_i;
	real *pC_e;
	real *pVtmp;
	real *pIi;
	real *pIe;
	real *pVthresh;
	real *pVreset;
	real *pCm;
};

int cudaFetchLIF(void *pGpu, void *pCpu, int num);

DATA_FUNC_DEFINE(LIF)

#endif /* GLIFNEURONS_H */
