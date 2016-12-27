/* This header file is writen by qp09
 * usually just for fun
 * Sat December 24 2016
 */
#ifndef MACROS_H
#define MACROS_H

#define NEURON_CONNECT_PARA \
	int *pSynapsesNum; \
	int *pSynapsesLoc; \
	int *pSynapsesIdx; \
	int num;           \
	int synapsesNum; 

#define SYNAPSE_CONNECT_PARA \
	int *pDst; \
	int num;

#define NEURON_GPU_FUNC_DEFINE(name) \
	void* create##name##Neurons(); \
	size_t get##name##Size(); \
	int alloc##name##Neurons(void *pCpu, int N); \
	int alloc##name##Connects(void *pCpu, int *pSynapsesNum, int *pSynapsesLoc, int *pSynapsesIdx, int S); \
	void send##name##Neurons(void *data, int rank, int offset, int size); \
	void recv##name##Neurons(void **data, int rank, int size); \
	int cudaAlloc##name##Neurons(void *pCpu, void *pGpu); \
	int cudaFree##name##Neurons(void *pGpu); 

#define SYNAPSE_GPU_FUNC_DEFINE(name) \
	void *create##name##Synapses(); \
	size_t get##name##Size(); \
	int alloc##name##Synapses(void *pSynapses, int S); \
	int alloc##name##Connects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3); \
	void send##name##Synapses(void *data, int rank, int offset, int size); \
	void recv##name##Synapses(void **data, int rank, int size); \
	int cudaAlloc##name##Synapses(void *pCpu, void *pGpu); \
	int cudaFree##name##Synapses(void *pGpu);

#define NEURON_GPU_FUNC_BASIC(name) \
void* create##name##Neurons() \
{ \
	return malloc(sizeof(G##name##Neurons)); \
} \
 \
size_t get##name##Size() \
{ \
	return sizeof(G##name##Neurons); \
} \
 \
int alloc##name##Connects(void *pCpu, int *pSynapsesNum, int *pSynapsesLoc, int *pSynapsesIdx, int S) \
{ \
	G##name##Neurons *p = (G##name##Neurons *)pCpu; \
	p->synapsesNum = S; \
	p->pSynapsesIdx = pSynapsesIdx; \
	p->pSynapsesNum = pSynapsesNum; \
	p->pSynapsesLoc = pSynapsesLoc; \
 \
	return 0; \
}

#define SYNAPSE_GPU_FUNC_BASIC(name) \
void *create##name##Synapses() \
{ \
	return malloc(sizeof(G##name##Synapses)); \
} \
 \
size_t get##name##Size() \
{ \
	return sizeof(G##name##Synapses); \
} \
 \
int alloc##name##Connects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3) \
{ \
	G##name##Synapses *p = (G##name##Synapses*)pCpu; \
	p->pDst = pSynapsesDst; \
 \
	return 0; \
}

#define NEURON_CONNECT_PARA_FREE(para) \
	checkCudaErrors(cudaFree(para->pSynapsesNum)); \
	checkCudaErrors(cudaFree(para->pSynapsesLoc)); \
	checkCudaErrors(cudaFree(para->pSynapsesIdx));

#define NEURON_CONNECT_PARA_ALLOC(gpu, cpu) \
	checkCudaErrors(cudaMalloc((void**)&(gpu->pSynapsesNum), sizeof(int)*(cpu->num))); \
	checkCudaErrors(cudaMemcpy(gpu->pSynapsesNum, cpu->pSynapsesNum, sizeof(int)*(cpu->num), cudaMemcpyHostToDevice)); \
	checkCudaErrors(cudaMalloc((void**)&(gpu->pSynapsesLoc), sizeof(int)*(cpu->num))); \
	checkCudaErrors(cudaMemcpy(gpu->pSynapsesLoc, cpu->pSynapsesLoc, sizeof(int)*(cpu->num), cudaMemcpyHostToDevice)); \
	checkCudaErrors(cudaMalloc((void**)&(gpu->pSynapsesIdx), sizeof(int)*(cpu->synapsesNum))); \
	checkCudaErrors(cudaMemcpy(gpu->pSynapsesIdx, cpu->pSynapsesIdx, sizeof(int)*(cpu->synapsesNum), cudaMemcpyHostToDevice)); 

#endif /* MACROS_H */

