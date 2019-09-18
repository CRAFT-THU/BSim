
#ifndef STATICDATA_H
#define STATICDATA_H

#include <stdio.h>

#include "../../net/Connection.h"

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct StaticData {
	int *pDst;

	real *pWeight;

};


size_t getStaticSize();
void *mallocStatic();
void *allocStatic(int num);
int allocStaticPara(void *pCPU, int num);
int freeStatic(void *pCPU);
int freeStaticPara(void *pCPU);
void updateStatic(Connection *, void *, real *, real *, int *, int*, int, int, int);
int saveStatic(void *pCPU, int num, FILE *f);
void *loadStatic(int num, FILE *f);
bool isEqualStatic(void *p1, void *p2, int num);

void *cudaMallocStatic();
void *cudaAllocStatic(void *pCPU, int num);
void *cudaAllocStaticPara(void *pCPU, int num);
int cudaFreeStatic(void *pGPU);
int cudaFreeStaticPara(void *pGPU);
int cudaFetchStatic(void *pCPU, void *pGPU, int num);
int cudaStaticParaToGPU(void *pCPU, void *pGPU, int num);
int cudaStaticParaFromGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateStatic(Connection *conn, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);

int mpiSendStatic(void *data, int rank, int offset, int size);
int mpiRecvStatic(void **data, int rank, int size);

#endif /* STATICDATA_H */
