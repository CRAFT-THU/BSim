
#ifndef LIFDATA_H
#define LIFDATA_H

#include <stdio.h>

#include "../../net/Connection.h"

#include "../../utils/type.h"
#include "../../utils/BlockSize.h"

struct LIFData {
	real *pV_reset;
	real *pI_i;
	real *pV_e;
	real *pV_i;
	real *pV_thresh;
	real *pV_m;
	real *pC_m;
	real *pCe;
	real *pV_tmp;
	real *pI_e;
	real *pC_i;
	real *pCi;
	real *pC_e;

	int *pRefracStep;
	int *pRefracTime;

};


size_t getLIFSize();
void *mallocLIF();
void *allocLIF(int num);
int allocLIFPara(void *pCPU, int num);
int freeLIF(void *pCPU);
int freeLIFPara(void *pCPU);
int saveLIF(void *pCPU, int num, FILE *f);
void *loadLIF(int num, FILE *f);
bool isEqualLIF(void *p1, void *p2, int num);

void *cudaMallocLIF();
void *cudaAllocLIF(void *pCPU, int num);
void *cudaAllocLIFPara(void *pCPU, int num);
int cudaFreeLIF(void *pGPU);
int cudaFreeLIFPara(void *pGPU);
int cudaFetchLIF(void *pCPU, void *pGPU, int num);
int cudaLIFParaToGPU(void *pCPU, void *pGPU, int num);
int cudaLIFParaFromGPU(void *pCPU, void *pGPU, int num);
void cudaUpdateLIF(Connection *conn, void *data, real *currentE, real *currentI, int *firedTable, int *firedTableSizes, int num, int start_id, int t, BlockSize *pSize);

int mpiSendLIF(void *data, int rank, int offset, int size);
int mpiRecvLIF(void **data, int rank, int size);

#endif /* LIFDATA_H */
