
#include <stdlib.h>

#include "../gpu_utils/mem_op.h"
#include "Connection.h"

N2SConnection * copyConnectionToGPU(N2SConnection * pCPU)
{
	int n_num = pCPU->nNum;
	int maxDelay = pCPU->maxDelay;

	N2SConnection * pGPU = NULL;
	N2SConnection * pTmp = (N2SConnection*)malloc(sizeof(N2SConnection));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->delayStart), sizeof(int)*n_num*maxDelay));
	checkCudaErrors(cudaMemcpy(pTmp->delayStart, pCPU->delayStart, sizeof(int)*n_num*maxDelay, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pTmp->delayNum), sizeof(int)*n_num*maxDelay));
	checkCudaErrors(cudaMemcpy(pTmp->delayNum, pCPU->delayNum, sizeof(int)*n_num*maxDelay, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGPU), sizeof(N2SConnection)));
	checkCudaErrors(cudaMemcpy(pGPU, pTmp, sizeof(N2SConnection), cudaMemcpyHostToDevice));
	free(pTmp);
	pTmp = NULL;

	return pGPU;
}
