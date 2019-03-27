
#include <stdlib.h>

#include "../gpu_utils/mem_op.h"
#include "Connection.h"

N2SConnection * copyConnectionToGPU(N2SConnection * pCPU)
{
	int n_num = pCPU->n_num;
	int MAX_DELAY = pCPU->MAX_DELAY;

	N2SConnection * pTmp = (N2SConnection*)malloc(sizeof(N2SConnection));
	N2SConnection * pGPU = NULL;
	int *g_delayStart = NULL, *g_delayNum = NULL;
	//int *g_pSynapsesIdx = NULL;
	//checkCudaErrors(cudaMalloc((void**)&(g_pSynapsesIdx), sizeof(int)*conn_s_num));
	//checkCudaErrors(cudaMemcpy(g_pSynapsesIdx, pCPU->pSynapsesIdx, sizeof(int)*conn_s_num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(g_delayStart), sizeof(int)*n_num*MAX_DELAY));
	checkCudaErrors(cudaMemcpy(g_delayStart, pCPU->delayStart, sizeof(int)*n_num*MAX_DELAY, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(g_delayNum), sizeof(int)*n_num*MAX_DELAY));
	checkCudaErrors(cudaMemcpy(g_delayNum, pCPU->delayNum, sizeof(int)*n_num*MAX_DELAY, cudaMemcpyHostToDevice));
	//pTmp->pSynapsesIdx = g_pSynapsesIdx;
	pTmp->delayStart = g_delayStart;
	pTmp->delayNum = g_delayNum;
	checkCudaErrors(cudaMalloc((void**)&(pGPU), sizeof(N2SConnection)));
	checkCudaErrors(cudaMemcpy(pGPU, pTmp, sizeof(N2SConnection), cudaMemcpyHostToDevice));
	free(pTmp);

	return pGPU;
}
