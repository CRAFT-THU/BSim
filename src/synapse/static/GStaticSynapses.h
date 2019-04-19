
#ifndef GSTATICSYNAPSES_H
#define GSTATICSYNAPSES_H

#include <stdio.h>

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct GStaticSynapses {
	int *pDst;

	real *pWeight;

};


void *mallocStatic();
void *allocStatic(int num);
int freeStatic(void *pCPU, int num);
int allocStaticPara(void *pCPU, int num);
int freeStaticPara(void *pCPU, int num);
int saveStatic(void *pCPU, int num, FILE *f);
int loadStatic(int num, FILE *f);

void *cudaAllocStatic(void *pCPU, int num);
int cudaStaticToGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateStatic(void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);
int cudaFreeStatic(void *pGPU);

int mpiSendStatic(void *data, int rank, int offset, int size);
int mpiRecvStatic(void *data, int rank, int offset, int size);

#endif // GSTATICSYNAPSES_H
