
#ifndef GLIFNEURONS_H
#define GLIFNEURONS_H

#include <stdio.h>

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct GLIFNeurons {
	real *pI_i;
	real *pV_reset;
	real *pCe;
	real *pC_e;
	real *pC_m;
	real *pI_e;
	real *pV_tmp;
	real *pCi;
	real *pV_thresh;
	real *pV_m;
	real *pV_i;
	real *pV_e;
	real *pC_i;

	int *pRefracTime;
	int *pRefracStep;

};


size_t getLIFSize();
void *mallocLIF();
void *allocLIF(int num);
int freeLIF(void *pCPU);
int allocLIFPara(void *pCPU, int num);
int freeLIFPara(void *pCPU);
int saveLIF(void *pCPU, int num, FILE *f);
void *loadLIF(int num, FILE *f);

void *cudaAllocLIF(void *pCPU, int num);
int cudaLIFToGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateLIF(void *data, void *conn, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);
int cudaFreeLIF(void *pGPU);

int mpiSendLIF(void *data, int rank, int offset, int size);
int mpiRecvLIF(void **data, int rank, int size);

#endif /* GLIFNEURONS_H */
