
#include "../third_party/cuda/helper_cuda.h"
#include "../gpu_utils/mem_op.h"
#include "GStaticSynapses.h"

int cudaAllocStatic(void *pCpu, void *pGpu, int num)
{
	GStaticSynapses *pGpuSynapses = (GStaticSynapses*)pGpu;
	GStaticSynapses *p = (GStaticSynapses*)pCpu;
	pGpuSynapses->p_weight = copyToGPU<real>(p->p_weight, num);
	return 0;
}

int cudaFreeStatic(void *pCpu)
{
	GStaticSynapses *pGpuSynapses = (GStaticSynapses*)pGpu;
	gpuFree(pGpuSynapses->p_weight);
	return 0;
}

