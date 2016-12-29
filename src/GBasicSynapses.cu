
#include "utils/cuda/helper_cuda.h"
#include "GBasicSynapses.h"

int cudaAllocBasicSynapses(void *pCpu, void *pGpu)
{
	GBasicSynapses *pGpuSynapses = (GBasicSynapses*)pGpu;
	GBasicSynapses *p = (GBasicSynapses*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_weight), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_weight, p->p_weight, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->p_delay_steps), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->p_delay_steps, p->p_delay_steps, sizeof(int)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuSynapses->pDst), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuSynapses->pDst, p->pDst, sizeof(int)*p->num, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeBasicSynapses(void *pGpu)
{
	GBasicSynapses *pGpuSynapses = (GBasicSynapses*)pGpu;

	checkCudaErrors(cudaFree(pGpuSynapses->p_weight));
	checkCudaErrors(cudaFree(pGpuSynapses->p_delay_steps));

	checkCudaErrors(cudaFree(pGpuSynapses->pDst));

	return 0;
}
