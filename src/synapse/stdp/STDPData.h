
#ifndef STDPDATA_H
#define STDPDATA_H

#include <stdio.h>

#include "../../net/Connection.h"

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct STDPData {
	int *pDst;
	int *pLastUpdate;

	real *pWeight;
	real *pAPre;
	real *pAPost;
	real *pDPre;
	real *pDPost;
	real *pTauPre;
	real *pTauPost;

};


size_t getSTDPSize();
void *mallocSTDP();
void *allocSTDP(int num);
int allocSTDPPara(void *pCPU, int num);
int freeSTDP(void *pCPU);
int freeSTDPPara(void *pCPU);
int saveSTDP(void *pCPU, int num, FILE *f);
void *loadSTDP(int num, FILE *f);
bool isEqualSTDP(void *p1, void *p2, int num);

void *cudaMallocSTDP();
void *cudaAllocSTDP(void *pCPU, int num);
void *cudaAllocSTDPPara(void *pCPU, int num);
int cudaFreeSTDP(void *pGPU);
int cudaFreeSTDPPara(void *pGPU);
int cudaFetchSTDP(void *pCPU, void *pGPU, int num);
int cudaSTDPParaToGPU(void *pCPU, void *pGPU, int num);
int cudaSTDPParaFromGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateSTDP(Connection *conn, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);

int mpiSendSTDP(void *data, int rank, int offset, int size);
int mpiRecvSTDP(void **data, int rank, int size);

#endif /* STDPDATA_H */
