
#ifndef GSTATICSYNAPSES_H
#define GSTATICSYNAPSES_H

#include <stdio.h>

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct GStaticSynapses {
	int *pDst;

	real *pWeight;

};


size_t getStaticSize();
void *mallocStatic();
void *allocStatic(int num);
int allocStaticPara(void *pCPU, int num);
int freeStatic(void *pCPU);
int freeStaticPara(void *pCPU);
int saveStatic(void *pCPU, int num, FILE *f);
void *loadStatic(int num, FILE *f);

void *cudaMallocStatic();
void *cudaAllocStatic(void *pCPU, int num);
void *cudaAllocStaticPara(void *pCPU, int num);
int cudaFreeStatic(void *pGPU);
int cudaFreeStaticPara(void *pGPU);
int cudaStaticParaToGPU(void *pCPU, void *pGPU, int num);
int cudaStaticParaFromGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateStatic(void *data, void *conn, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);

int mpiSendStatic(void *data, int rank, int offset, int size);
int mpiRecvStatic(void **data, int rank, int size);

#endif /* GSTATICSYNAPSES_H */
