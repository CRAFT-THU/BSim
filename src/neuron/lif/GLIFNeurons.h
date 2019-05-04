
#ifndef GLIFNEURONS_H
#define GLIFNEURONS_H

#include <stdio.h>

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct GLIFNeurons {
	real *pCe;
	real *pV_m;
	real *pI_i;
	real *pC_e;
	real *pCi;
	real *pV_i;
	real *pI_e;
	real *pC_m;
	real *pV_reset;
	real *pV_e;
	real *pV_thresh;
	real *pV_tmp;
	real *pC_i;

	int *pRefracTime;
	int *pRefracStep;

};


size_t getLIFSize();
void *mallocLIF();
void *allocLIF(int num);
int allocLIFPara(void *pCPU, int num);
int freeLIF(void *pCPU);
int freeLIFPara(void *pCPU);
int saveLIF(void *pCPU, int num, FILE *f);
void *loadLIF(int num, FILE *f);

void *cudaMallocLIF();
void *cudaAllocLIF(void *pCPU, int num);
void *cudaAllocLIFPara(void *pCPU, int num);
int cudaFreeLIF(void *pGPU);
int cudaFreeLIFPara(void *pGPU);
int cudaFetchLIF(void *pCPU, void *pGPU, int num);
int cudaLIFParaToGPU(void *pCPU, void *pGPU, int num);
int cudaLIFParaFromGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateLIF(void *data, void *conn, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);

int mpiSendLIF(void *data, int rank, int offset, int size);
int mpiRecvLIF(void **data, int rank, int size);

#endif /* GLIFNEURONS_H */
