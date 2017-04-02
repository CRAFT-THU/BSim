
#include "../third_party/cuda/helper_cuda.h"
#include "../gpu_utils/mem_op.h"
#include "GLIFENeurons.h"

int cudaAllocLIFE(void *pCpu, void *pGpu, int num)
{
	GLIFENeurons *pGpuNeurons = (GLIFENeurons*)pGpu;
	GLIFENeurons *p = (GLIFENeurons*)pCpu;
	pGpuNeurons->p_CI = copyToGPU<real>(p->p_CI, num);
	pGpuNeurons->p_vm = copyToGPU<real>(p->p_vm, num);
	pGpuNeurons->p_CE = copyToGPU<real>(p->p_CE, num);
	pGpuNeurons->p_refrac_step = copyToGPU<int>(p->p_refrac_step, num);
	pGpuNeurons->p_refrac_time = copyToGPU<int>(p->p_refrac_time, num);
	pGpuNeurons->p_i_tmp = copyToGPU<real>(p->p_i_tmp, num);
	pGpuNeurons->p_i_I = copyToGPU<real>(p->p_i_I, num);
	pGpuNeurons->p_i_E = copyToGPU<real>(p->p_i_E, num);
	pGpuNeurons->p_v_thresh = copyToGPU<real>(p->p_v_thresh, num);
	pGpuNeurons->p_v_reset = copyToGPU<real>(p->p_v_reset, num);
	pGpuNeurons->p_C2 = copyToGPU<real>(p->p_C2, num);
	pGpuNeurons->p_C1 = copyToGPU<real>(p->p_C1, num);
	return 0;
}

int cudaFreeLIFE(void *pCpu)
{
	GLIFENeurons *pGpuNeurons = (GLIFENeurons*)pGpu;
	gpuFree(pGpuNeurons->p_CI);
	gpuFree(pGpuNeurons->p_vm);
	gpuFree(pGpuNeurons->p_CE);
	gpuFree(pGpuNeurons->p_refrac_step);
	gpuFree(pGpuNeurons->p_refrac_time);
	gpuFree(pGpuNeurons->p_i_tmp);
	gpuFree(pGpuNeurons->p_i_I);
	gpuFree(pGpuNeurons->p_i_E);
	gpuFree(pGpuNeurons->p_v_thresh);
	gpuFree(pGpuNeurons->p_v_reset);
	gpuFree(pGpuNeurons->p_C2);
	gpuFree(pGpuNeurons->p_C1);
	return 0;
}

