/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../../third_party/cuda/helper_cuda.h"
#include "../../gpu_utils/mem_op.h"
#include "GMaxNeurons.h"

int cudaAllocMax(void *pCpu, void *pGpu, int num)
{
	GMaxNeurons *pGpuNeurons = (GMaxNeurons*)pGpu;
	GMaxNeurons *p = (GMaxNeurons*)pCpu;

	pGpuNeurons->p_N = copyToGPU<int>(p->p_N, num);
	pGpuNeurons->p_count = gpuMalloc<int>(num);
	pGpuNeurons->p_record = gpuMalloc<int>(num*(p->max_N));

	return 0;
}

int cudaFreeMax(void *pGpu)
{
	GMaxNeurons *pGpuNeurons = (GMaxNeurons*)pGpu;

	gpuFree(pGpuNeurons->p_N);
	gpuFree(pGpuNeurons->p_count);
	gpuFree(pGpuNeurons->p_record);

	return 0;
}
