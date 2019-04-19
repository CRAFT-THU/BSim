
#ifndef GLIFNEURONS_H
#define GLIFNEURONS_H

#include <stdio.h>

#include "../../utils/type.h"

struct GLIFNeurons {
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

};


void *mallocLIF();
void *allocLIF(int num);
int freeLIF(void *pCPU, int num);
int allocLIFPara(void *pCPU, int num);
int freeLIFPara(void *pCPU, int num);
int saveLIF(void *pCPU, int num, FILE *f);
int loadLIF(int num, FILE *f);

void *cudaAllocLIF(void *pCPU, int num);
int cudaLIFToGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateLIF(void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);
int cudaFreeLIF(void *pGPU);

int mpiSendLIF(void *data, int rank, int offset, int size);
int mpiRecvLIF(void *data, int rank, int offset, int size);

#endif // GLIFNEURONS_H
