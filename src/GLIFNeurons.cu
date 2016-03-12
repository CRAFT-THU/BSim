/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "./utils/cuda/helper_cuda.h"
#include "GLIFNeurons.h"

int cudaAllocLIFNeurons(void *pCpu, void *pGpu)
{
	GLIFNeurons *pGpuNeurons = (GLIFNeurons*)pGpu;
	GLIFNeurons *p = (GLIFNeurons*)pCpu;

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pID), sizeof(ID)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pID, p->pID, sizeof(ID)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pType), sizeof(Type)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pType, p->pType, sizeof(Type)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_init), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_init, p->p_v_init, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_rest), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_rest, p->p_v_rest, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_reset), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_reset, p->p_v_init, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_cm), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_cm, p->p_cm, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_m), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_m, p->p_tau_m, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_refrac), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_refrac, p->p_tau_refrac, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_syn_E), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_syn_E, p->p_tau_syn_E, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_syn_I), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_syn_I, p->p_tau_syn_E, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_thresh), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_thresh, p->p_v_thresh, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_offset), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_offset, p->p_i_offset, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_syn), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_syn, p->p_i_syn, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_vm), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_vm, p->p_vm, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p__dt), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p__dt, p->p__dt, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_C1), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_C1, p->p_C1, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_C2), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_C2, p->p_C2, sizeof(real)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_tmp), sizeof(real)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_tmp, p->p_i_tmp, sizeof(real)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_refrac_step), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_refrac_step, p->p_refrac_step, sizeof(int)*p->num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pSynapsesNum), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pSynapsesNum, p->pSynapsesNum, sizeof(int)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pSynapsesLoc), sizeof(int)*p->num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pSynapsesLoc, p->pSynapsesLoc, sizeof(int)*p->num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pSynapsesIdx), sizeof(int)*p->synapsesNum));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pSynapsesIdx, p->pSynapsesIdx, sizeof(int)*p->synapsesNum, cudaMemcpyHostToDevice));

	return 0;
}

int cudaFreeLIFNeurons(void *pGpu)
{
	GLIFNeurons *pGpuNeurons = (GLIFNeurons*)pGpu;

	checkCudaErrors(cudaFree(pGpuNeurons->pID));
	checkCudaErrors(cudaFree(pGpuNeurons->pType));

	checkCudaErrors(cudaFree(pGpuNeurons->p_v_init));
	checkCudaErrors(cudaFree(pGpuNeurons->p_v_rest));
	checkCudaErrors(cudaFree(pGpuNeurons->p_v_reset));
	checkCudaErrors(cudaFree(pGpuNeurons->p_cm));
	checkCudaErrors(cudaFree(pGpuNeurons->p_tau_m));
	checkCudaErrors(cudaFree(pGpuNeurons->p_tau_refrac));
	checkCudaErrors(cudaFree(pGpuNeurons->p_tau_syn_E));
	checkCudaErrors(cudaFree(pGpuNeurons->p_tau_syn_I));
	checkCudaErrors(cudaFree(pGpuNeurons->p_v_thresh));
	checkCudaErrors(cudaFree(pGpuNeurons->p_i_offset));
	checkCudaErrors(cudaFree(pGpuNeurons->p_i_syn));
	checkCudaErrors(cudaFree(pGpuNeurons->p_vm));
	checkCudaErrors(cudaFree(pGpuNeurons->p__dt));
	checkCudaErrors(cudaFree(pGpuNeurons->p_C1));
	checkCudaErrors(cudaFree(pGpuNeurons->p_C2));
	checkCudaErrors(cudaFree(pGpuNeurons->p_i_tmp));

	checkCudaErrors(cudaFree(pGpuNeurons->p_refrac_step));

	checkCudaErrors(cudaFree(pGpuNeurons->pSynapsesNum));
	checkCudaErrors(cudaFree(pGpuNeurons->pSynapsesLoc));
	checkCudaErrors(cudaFree(pGpuNeurons->pSynapsesIdx));

	return 0;
}
