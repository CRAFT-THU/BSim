
#include "../../third_party/cuda/helper_cuda.h"
#include "../../gpu_utils/mem_op.h"
#include "GLIFNeurons.h"

int cudaAllocLIF(void *pCpu, void *pGpu, int num)
{
	GLIFNeurons *pGpuNeurons = (GLIFNeurons*)pGpu;
	GLIFNeurons *p = (GLIFNeurons*)pCpu;
	pGpuNeurons->p_vm = copyToGPU<real>(p->p_vm, num);
	pGpuNeurons->p_CI = copyToGPU<real>(p->p_CI, num);
	pGpuNeurons->p_CE = copyToGPU<real>(p->p_CE, num);
	pGpuNeurons->p_C_I = copyToGPU<real>(p->p_C_I, num);
	pGpuNeurons->p_C_E = copyToGPU<real>(p->p_C_E, num);
	pGpuNeurons->p_v_tmp = copyToGPU<real>(p->p_v_tmp, num);
	pGpuNeurons->p_i_I = copyToGPU<real>(p->p_i_I, num);
	pGpuNeurons->p_i_E = copyToGPU<real>(p->p_i_E, num);
	pGpuNeurons->p_v_thresh = copyToGPU<real>(p->p_v_thresh, num);
	pGpuNeurons->p_v_reset = copyToGPU<real>(p->p_v_reset, num);
	pGpuNeurons->p_Cm = copyToGPU<real>(p->p_Cm, num);
	pGpuNeurons->p_refrac_step = copyToGPU<int>(p->p_refrac_step, num);
	pGpuNeurons->p_refrac_time = copyToGPU<int>(p->p_refrac_time, num);
	//pGpuNeurons->p_start_E = copyToGPU<int>(p->p_start_E, num);
	//pGpuNeurons->p_start_I = copyToGPU<int>(p->p_start_I, num);
	//pGpuNeurons->p_end = copyToGPU<int>(p->p_end, num);
	return 0;
}

int cudaFetchLIF(void *pGpu, void *pCpu, int num)
{
	GLIFNeurons *pGpuNeurons = copyFromGPU(static_cast<GLIFNeurons*>(pGpu), 1);
	GLIFNeurons *p = (GLIFNeurons*)pCpu;
	copyFromGPU<real>(p->p_vm, pGpuNeurons->p_vm, num);
	copyFromGPU<real>(p->p_i_E, pGpuNeurons->p_i_E, num);
	copyFromGPU<real>(p->p_i_I, pGpuNeurons->p_i_I, num);
	copyFromGPU<int>(p->p_refrac_step, pGpuNeurons->p_refrac_step, num);
	free(pGpuNeurons);

	return 0;
}

int cudaFreeLIF(void *pGpu)
{
	GLIFNeurons *pGpuNeurons = (GLIFNeurons*)pGpu;
	gpuFree(pGpuNeurons->p_vm);
	gpuFree(pGpuNeurons->p_CI);
	gpuFree(pGpuNeurons->p_CE);
	gpuFree(pGpuNeurons->p_C_I);
	gpuFree(pGpuNeurons->p_C_E);
	gpuFree(pGpuNeurons->p_v_tmp);
	gpuFree(pGpuNeurons->p_i_I);
	gpuFree(pGpuNeurons->p_i_E);
	gpuFree(pGpuNeurons->p_v_thresh);
	gpuFree(pGpuNeurons->p_v_reset);
	gpuFree(pGpuNeurons->p_Cm);
	gpuFree(pGpuNeurons->p_refrac_step);
	gpuFree(pGpuNeurons->p_refrac_time);
	//gpuFree(pGpuNeurons->p_start_E);
	//gpuFree(pGpuNeurons->p_start_I);
	//gpuFree(pGpuNeurons->p_end);
	return 0;
}

