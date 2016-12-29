/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../third_party/cuda/helper_cuda.h"
#include "GConstantNeurons.h"

int cudaAllocConstantNeurons(void *pCpu, void *pGpu)
{
	GConstantNeurons *pGpuNeurons = (GConstantNeurons*)pGpu;
	GConstantNeurons *p = (GConstantNeurons*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_fire_rate), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_fire_rate, p->p_fire_rate, sizeof(real)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_fire_count), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_fire_count, p->p_fire_count, sizeof(int)*p->num, cudaMemcpyHostToDevice));

	NEURON_CONNECT_PARA_ALLOC(pGpuNeurons, p)

	return 0;
}

int cudaFreeConstantNeurons(void *pGpu)
{
	GConstantNeurons *pGpuNeurons = (GConstantNeurons*)pGpu;

	checkCudaErrors(cudaFree(pGpuNeurons->p_fire_rate));
	checkCudaErrors(cudaFree(pGpuNeurons->p_fire_count));

	NEURON_CONNECT_PARA_FREE(pGpuNeurons)

	return 0;
}
