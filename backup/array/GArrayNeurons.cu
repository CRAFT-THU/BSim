/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../../third_party/cuda/helper_cuda.h"
#include "../../gpu_utils/mem_op.h"
#include "GArrayNeurons.h"

int cudaAllocArray(void *pCpu, void *pGpu, int num)
{
	GArrayNeurons *pGpuNeurons = (GArrayNeurons*)pGpu;
	GArrayNeurons *p = (GArrayNeurons*)pCpu;

	pGpuNeurons->p_start = copyToGPU<int>(p->p_start, num);
	pGpuNeurons->p_end = copyToGPU<int>(p->p_end, num);;
	pGpuNeurons->p_fire_time = copyToGPU<int>(p->p_fire_time, p->p_end[num-1]);

	return 0;
}

int cudaFreeArray(void *pGpu)
{
	GArrayNeurons *pGpuNeurons = (GArrayNeurons*)pGpu;
	//TODO delete fire_time arrays

	gpuFree(pGpuNeurons->p_start);
	gpuFree(pGpuNeurons->p_end);
	gpuFree(pGpuNeurons->p_fire_time);

	return 0;
}
