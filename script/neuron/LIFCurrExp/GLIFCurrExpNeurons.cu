
#include "../third_party/cuda/helper_cuda.h"
#include "../gpu_utils/mem_op.h"
#include "GLIFCurrExpNeurons.h"

int cudaAllocLIFCurrExp(void *pCpu, void *pGpu, int num)
{
	GLIFCurrExpNeurons *pGpuNeurons = (GLIFCurrExpNeurons*)pGpu;
	GLIFCurrExpNeurons *p = (GLIFCurrExpNeurons*)pCpu;
	pGpuNeurons->p_i_offset = copyToGPU<real>(p->p_i_offset, num);
	pGpuNeurons->p_i_injection = copyToGPU<real>(p->p_i_injection, num);
	pGpuNeurons->p_cm = copyToGPU<real>(p->p_cm, num);
	pGpuNeurons->p_tau_m = copyToGPU<real>(p->p_tau_m, num);
	pGpuNeurons->p_tau_e = copyToGPU<real>(p->p_tau_e, num);
	pGpuNeurons->p_tau_i = copyToGPU<real>(p->p_tau_i, num);
	pGpuNeurons->p_vm = copyToGPU<real>(p->p_vm, num);
	pGpuNeurons->p_v_thresh = copyToGPU<real>(p->p_v_thresh, num);
	pGpuNeurons->p_v_reset = copyToGPU<real>(p->p_v_reset, num);
	pGpuNeurons->p_v_reset = copyToGPU<real>(p->p_v_reset, num);
	pGpuNeurons->p_i_E = copyToGPU<real>(p->p_i_E, num);
	pGpuNeurons->p_i_I = copyToGPU<real>(p->p_i_I, num);
	pGpuNeurons->p_refrac_time = copyToGPU<int>(p->p_refrac_time, num);
	pGpuNeurons->p_refrac_step = copyToGPU<int>(p->p_refrac_step, num);
	return 0;
}

int cudaFreeLIFCurrExp(void *pGpu)
{
	GLIFCurrExpNeurons *pGpuNeurons = (GLIFCurrExpNeurons*)pGpu;
	gpuFree(pGpuNeurons->p_i_offset);
	gpuFree(pGpuNeurons->p_i_injection);
	gpuFree(pGpuNeurons->p_cm);
	gpuFree(pGpuNeurons->p_tau_m);
	gpuFree(pGpuNeurons->p_tau_e);
	gpuFree(pGpuNeurons->p_tau_i);
	gpuFree(pGpuNeurons->p_vm);
	gpuFree(pGpuNeurons->p_v_thresh);
	gpuFree(pGpuNeurons->p_v_reset);
	gpuFree(pGpuNeurons->p_v_reset);
	gpuFree(pGpuNeurons->p_i_E);
	gpuFree(pGpuNeurons->p_i_I);
	gpuFree(pGpuNeurons->p_refrac_time);
	gpuFree(pGpuNeurons->p_refrac_step);
	return 0;
}

