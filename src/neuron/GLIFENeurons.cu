
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
	gpuFreepGpuNeurons->p_CI);
	gpuFreepGpuNeurons->p_vm);
	gpuFreepGpuNeurons->p_CE);
	gpuFreepGpuNeurons->p_refrac_step);
	gpuFreepGpuNeurons->p_refrac_time);
	gpuFreepGpuNeurons->p_i_tmp);
	gpuFreepGpuNeurons->p_i_I);
	gpuFreepGpuNeurons->p_i_E);
	gpuFreepGpuNeurons->p_v_thresh);
	gpuFreepGpuNeurons->p_v_reset);
	gpuFreepGpuNeurons->p_C2);
	gpuFreepGpuNeurons->p_C1);
	return 0;
}

