/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "../../gpu_utils/mem_op.h"
#include "../../gpu_utils/gpu_func.h"
#include "../../gpu_utils/runtime.h"
#include "GPoissonNeurons.h"

int cudaAllocPoisson(void *pCpu, void *pGpu, int num)
{
	GPoissonNeurons *pGpuNeurons = (GPoissonNeurons*)pGpu;
	GPoissonNeurons *p = (GPoissonNeurons*)pCpu;

	pGpuNeurons->p_rate = copyToGPU<real>(p->p_rate, num);
	pGpuNeurons->p_fire_cycle = copyToGPU<int>(p->p_fire_cycle, num);
	pGpuNeurons->p_end_cycle = copyToGPU<int>(p->p_end_cycle, num);
	pGpuNeurons->p_refrac_step = copyToGPU<int>(p->p_refrac_step, num);
	pGpuNeurons->p_state = gpuMalloc<curandState>(num);
	int block_num = (num+511)/512;
	curand_setup_kernel<<<block_num, 512>>>(pGpuNeurons->p_state, num);

	return 0;
}

int cudaFreePoisson(void *pGpu)
{
	GPoissonNeurons *pGpuNeurons = (GPoissonNeurons*)pGpu;

	gpuFree(pGpuNeurons->p_rate);
	gpuFree(pGpuNeurons->p_fire_cycle);
	gpuFree(pGpuNeurons->p_end_cycle);
	gpuFree(pGpuNeurons->p_refrac_step);
	gpuFree(pGpuNeurons->p_state);

	return 0;
}
