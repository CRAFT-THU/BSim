
#include "../utils/TypeFunc.h"
#include "mem_op.h"
#include "gpu_func.h"
#include "gpu_utils.h"

GNetwork* copyNetworkToGPU(GNetwork *pCpuNet)
{
	if (pCpuNet == NULL) {
		printf("NULL POINTER: GNETWORK\n");
		exit(-1);
	}

	GNetwork *tmpNet = (GNetwork*)malloc(sizeof(GNetwork));
	memcpy(tmpNet, pCpuNet, sizeof(GNetwork));

	int nTypeNum = pCpuNet->nTypeNum;
	int sTypeNum = pCpuNet->sTypeNum;
	int totalNeuronNum = pCpuNet->neuronNums[pCpuNet->nTypeNum];
	int totalSynapseNum = pCpuNet->synapseNums[pCpuNet->sTypeNum];
	int MAX_DELAY = pCpuNet->MAX_DELAY;

	//Type *g_nTypes = NULL, *g_sTypes = NULL;
	//checkCudaErrors(cudaMalloc((void**)&(g_nTypes), sizeof(Type)*nTypeNum));
	//checkCudaErrors(cudaMemcpy(g_nTypes, pCpuNet->nTypes, sizeof(Type)*nTypeNum, cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(sTypes), sizeof(Type)*sTypeNum));
	//checkCudaErrors(cudaMemcpy(g_sTypes, pCpuNet->sTypes, sizeof(Type)*sTypeNum, cudaMemcpyHostToDevice));

	//int *g_neuronNums = NULL, *g_synapseNums = NULL;
	//checkCudaErrors(cudaMalloc((void**)&(g_neuronNums), sizeof(int)*(nTypeNum+1)));
	//checkCudaErrors(cudaMemcpy(g_neuronNums, pCpuNet->neuronNums, sizeof(int)*(nTypeNum+1), cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(g_synapseNums), sizeof(int)*(sTypeNum+1)));
	//checkCudaErrors(cudaMemcpy(g_synapseNums, pCpuNet->synapseNums, sizeof(int)*(sTypeNum+1), cudaMemcpyHostToDevice));

	//int *g_gNeuronNums = NULL, *g_gSynapseNums = NULL;
	//checkCudaErrors(cudaMalloc((void**)&(g_gNeuronNums), sizeof(int)*(nTypeNum+1)));
	//checkCudaErrors(cudaMemcpy(g_gNeuronNums, pCpuNet->gNeuronNums, sizeof(int)*(nTypeNum+1), cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(g_gSynapseNums), sizeof(int)*(sTypeNum+1)));
	//checkCudaErrors(cudaMemcpy(g_gSynapseNums, pCpuNet->gSynapseNums, sizeof(int)*(sTypeNum+1), cudaMemcpyHostToDevice));

	//int *nOffsets = NULL, *sOffsets = NULL;
	//checkCudaErrors(cudaMalloc((void**)&(g_nOffsets), sizeof(int)*(nTypeNum)));
	//checkCudaErrors(cudaMemcpy(g_nOffsets, pCpuNet->nOffsets, sizeof(int)*(nTypeNum), cudaMemcpyHostToDevice));
	//checkCudaErrors(cudaMalloc((void**)&(g_sOffsets), sizeof(int)*(sTypeNum)));
	//checkCudaErrors(cudaMemcpy(g_sOffsets, pCpuNet->sOffsets, sizeof(int)*(sTypeNum), cudaMemcpyHostToDevice));

	//TODO support multitype N and S
	void **pNs = (void**)malloc(sizeof(void*)*nTypeNum);
	void **pSs = (void**)malloc(sizeof(void*)*sTypeNum);

	for (int i=0; i<nTypeNum; i++) {
		void *pNTmp = createType[pCpuNet->nTypes[i]]();
		memcpy(pNTmp, pCpuNet->pNeurons[i], getTypeSize[pCpuNet->nTypes[i]]());
		cudaAllocType[pCpuNet->nTypes[i]](pCpuNet->pNeurons[i], pNTmp, pCpuNet->neuronNums[i+1]-pCpuNet->neuronNums[i]);
		void *pNGpu;
		checkCudaErrors(cudaMalloc((void**)&(pNGpu), getTypeSize[pCpuNet->nTypes[i]]()));
		checkCudaErrors(cudaMemcpy(pNGpu, pNTmp, getTypeSize[pCpuNet->nTypes[i]](), cudaMemcpyHostToDevice));
		free(pNTmp);
		pNs[i] = pNGpu;
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pSTmp = createType[pCpuNet->sTypes[i]]();
		memcpy(pSTmp, pCpuNet->pSynapses[i], getTypeSize[pCpuNet->sTypes[i]]());
		cudaAllocType[pCpuNet->sTypes[i]](pCpuNet->pSynapses[i], pSTmp, pCpuNet->synapseNums[i+1]-pCpuNet->synapseNums[i]);
		void *pSGpu;
		checkCudaErrors(cudaMalloc((void**)&(pSGpu), getTypeSize[pCpuNet->sTypes[i]]()));
		checkCudaErrors(cudaMemcpy(pSGpu, pSTmp, getTypeSize[pCpuNet->sTypes[i]](), cudaMemcpyHostToDevice));
		free(pSTmp);
		pSs[i] = pSGpu;

	}

	N2SConnection * pConnection = (N2SConnection*)malloc(sizeof(N2SConnection));
	N2SConnection * g_pConnection = NULL;
	int *g_pSynapsesIdx = NULL, *g_delayStart = NULL, *g_delayNum = NULL;
	checkCudaErrors(cudaMalloc((void**)&(g_pSynapsesIdx), sizeof(int)*totalSynapseNum));
	checkCudaErrors(cudaMemcpy(g_pSynapsesIdx, pCpuNet->pN2SConnection->pSynapsesIdx, sizeof(int)*totalSynapseNum, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(g_delayStart), sizeof(int)*totalNeuronNum*MAX_DELAY));
	checkCudaErrors(cudaMemcpy(g_delayStart, pCpuNet->pN2SConnection->delayStart, sizeof(int)*totalNeuronNum*MAX_DELAY, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(g_delayNum), sizeof(int)*totalNeuronNum*MAX_DELAY));
	checkCudaErrors(cudaMemcpy(g_delayNum, pCpuNet->pN2SConnection->delayNum, sizeof(int)*totalNeuronNum*MAX_DELAY, cudaMemcpyHostToDevice));
	pConnection->pSynapsesIdx = g_pSynapsesIdx;
	pConnection->delayStart = g_delayStart;
	pConnection->delayNum = g_delayNum;
	checkCudaErrors(cudaMalloc((void**)&(g_pConnection), sizeof(N2SConnection)));
	checkCudaErrors(cudaMemcpy(g_pConnection, pConnection, sizeof(N2SConnection), cudaMemcpyHostToDevice));
	free(pConnection);

	tmpNet->pNeurons = pNs;
	tmpNet->pSynapses = pSs;
	tmpNet->pN2SConnection = g_pConnection;
	//tmpNet->nOffsets = pCpuNet->nOffsets;
	//tmpNet->sOffsets = pCpuNet->sOffsets;
	tmpNet->neuronNums = pCpuNet->neuronNums;
	tmpNet->synapseNums = pCpuNet->synapseNums;

	tmpNet->nTypes = pCpuNet->nTypes;
	tmpNet->sTypes = pCpuNet->sTypes;
	//tmpNet->gNeuronNums = pCpuNet->gNeuronNums;
	//tmpNet->gSynapseNums = pCpuNet->gSynapseNums;

	return tmpNet;
}

int freeGPUNetwork(GNetwork *pGpuNet)
{
	GNetwork *pTmpNet = pGpuNet;

	int nTypeNum = pTmpNet->nTypeNum;
	int sTypeNum = pTmpNet->sTypeNum;

	//Type * pTmpNT = (Type*)malloc(sizeof(Type)*nTypeNum);
	//checkCudaErrors(cudaMemcpy(pTmpNT, pTmpNet->nTypes, sizeof(Type)*nTypeNum, cudaMemcpyDeviceToHost));
	//Type * pTmpST = (Type*)malloc(sizeof(Type)*sTypeNum);
	//checkCudaErrors(cudaMemcpy(pTmpST, pTmpNet->sTypes, sizeof(Type)*sTypeNum, cudaMemcpyDeviceToHost));

	for (int i=0; i<nTypeNum; i++) {
		void *pTmpN = createType[pTmpNet->nTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpN, pTmpNet->pNeurons[i], getTypeSize[pTmpNet->nTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmpNet->nTypes[i]](pTmpN);
		checkCudaErrors(cudaFree(pTmpNet->pNeurons[i]));
		free(pTmpN);
	}

	for (int i=0; i<sTypeNum; i++) {
		void *pTmpS = createType[pTmpNet->sTypes[i]]();
		checkCudaErrors(cudaMemcpy(pTmpS, pTmpNet->pSynapses[i], getTypeSize[pTmpNet->sTypes[i]](), cudaMemcpyDeviceToHost));
		cudaFreeType[pTmpNet->sTypes[i]](pTmpS);
		checkCudaErrors(cudaFree(pTmpNet->pSynapses[i]));
		free(pTmpS);
	}

	N2SConnection * pConnection = (N2SConnection*)malloc(sizeof(N2SConnection));
	checkCudaErrors(cudaMemcpy(pConnection, pTmpNet->pN2SConnection, sizeof(N2SConnection), cudaMemcpyDeviceToHost));
	checkCudaErrors(cudaFree(pConnection->pSynapsesIdx));
	checkCudaErrors(cudaFree(pConnection->delayStart));
	checkCudaErrors(cudaFree(pConnection->delayNum));
	checkCudaErrors(cudaFree(pTmpNet->pN2SConnection));
	free(pConnection);

	//checkCudaErrors(cudaFree(pTmpNet->nTypes));
	//checkCudaErrors(cudaFree(pTmpNet->sTypes));
	//free(pTmpNT);
	//free(pTmpST);

	//checkCudaErrors(cudaFree(pTmpNet->gNeuronNums));
	//checkCudaErrors(cudaFree(pTmpNet->gSynapseNums));

	//checkCudaErrors(cudaFree(pTmpNet->neuronNums));
	//checkCudaErrors(cudaFree(pTmpNet->synapseNums));

	//checkCudaErrors(cudaFree(pTmpNet->nOffsets));
	//checkCudaErrors(cudaFree(pTmpNet->sOffsets));

	free(pTmpNet->pNeurons);
	free(pTmpNet->pSynapses);
	//free(pTmpNs);
	//free(pTmpSs);
	free(pTmpNet);

	return 0;
}
