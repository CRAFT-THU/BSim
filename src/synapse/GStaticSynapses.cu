
#include "../third_party/cuda/helper_cuda.h"
#include "../gpu_utils/mem_op.h"
#include "GStaticSynapses.h"

int cudaAllocStatic(void *pCpu, void *pGpu, int num)
{
	GStaticSynapses *pGpuSynapses = (GStaticSynapses*)pGpu;
	GStaticSynapses *p = (GStaticSynapses*)pCpu;
	pGpuSynapses->p_weight = copyToGPU<real>(p->p_weight, num);
	pGpuSynapses->p_dst = copyToGPU<int>(p->p_dst, num);
	return 0;
}

int cudaFreeStatic(void *pGpu)
{
	GStaticSynapses *pGpuSynapses = (GStaticSynapses*)pGpu;
	gpuFree(pGpuSynapses->p_weight);
	gpuFree(pGpuSynapses->p_dst);
	return 0;
}

