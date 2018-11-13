
#include "../../third_party/cuda/helper_cuda.h"
#include "../../gpu_utils/mem_op.h"
#include "GStaticSynapses.h"

int cudaAllocStatic(void *pCpu, void *pGpu, int num)
{
	GStaticSynapses *pGpuSynapses = (GStaticSynapses*)pGpu;
	GStaticSynapses *p = (GStaticSynapses*)pCpu;
	pGpuSynapses->p_weight = copyToGPU<real>(p->p_weight, num);
	pGpuSynapses->p_apre = copyToGPU<real>(p->p_apre, num);
	pGpuSynapses->p_apost = copyToGPU<real>(p->p_apost, num);
	pGpuSynapses->p_d_apre = copyToGPU<real>(p->p_d_apre, num);
	pGpuSynapses->p_d_apost = copyToGPU<real>(p->p_d_apost, num);
	pGpuSynapses->p_tau_apre = copyToGPU<real>(p->p_tau_apre, num);
	pGpuSynapses->p_tau_apost = copyToGPU<real>(p->p_tau_apost, num);
	//pGpuSynapses->p_delay = copyToGPU<int>(p->p_delay, num);
	pGpuSynapses->p_src = copyToGPU<int>(p->p_src, num);
	pGpuSynapses->p_dst = copyToGPU<int>(p->p_dst, num);
	return 0;
}

int cudaFreeStatic(void *pGpu)
{
	GStaticSynapses *pGpuSynapses = (GStaticSynapses*)pGpu;
	gpuFree(pGpuSynapses->p_weight);
	gpuFree(pGpuSynapses->p_apre);
	gpuFree(pGpuSynapses->p_apost);
	gpuFree(pGpuSynapses->p_d_apre);
	gpuFree(pGpuSynapses->p_d_apost);
	gpuFree(pGpuSynapses->p_tau_apre);
	gpuFree(pGpuSynapses->p_tau_apost);
	//gpuFree(pGpuSynapses->p_delay);
	gpuFree(pGpuSynapses->p_src);
	gpuFree(pGpuSynapses->p_dst);
	return 0;
}

