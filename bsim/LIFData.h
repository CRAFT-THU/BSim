
#ifndef LIFDATA_H
#define LIFDATA_H

#include <stdio.h>

#include "../../utils/type.h"

struct LIFData {
	int *pRefracStep;
	int *pRefracTime;

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


void *mallocLIF();
void *allocLIF(int n);
int freeLIF(void *pCpu, int n);
int allocLIFPara(void *pCpu, int n);
int freeLIFPara(void *pCpu, int n);
int saveLIF(void *pCpu, int n, FILE *f);
int loadLIF(int n, FILE *f);

void* cudaAllocLIF(void *pCpu, int num);
int cudaLIFToGPU(void *pCpu, void *pGPU, int num);
void cudaUpdateLIF(void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);
int cudaFreeLIF(void *pGpu);

int mpiSendLIF(void *data, int rank, int offset, int size);
int mpiRecvLIF(void *data, int rank, int offset, int size);

#endif // LIFDATA_H
