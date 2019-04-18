
#ifndef LIFDATA_H 
#define LIFDATA_H 

#include "../../utils/type.h"
#include "../../utils/macros.h"

struct LIFData {
	int *pRefrac_step;
	int *pRefrac_time;

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

}

DATA_FUNC_DEFINE(LIF);

#endif // LIFDATA_H
