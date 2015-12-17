/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */


#include "./utils/cuda/helper_cuda.h"
#include "GLIFNeurons.h"

int GLIFNeurons::allocGNeurons(GLIFNeurons * pGpuNeurons)
{
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pID), sizeof(ID)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pID, pID, sizeof(ID)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pType), sizeof(NeuronType)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pType, pType, sizeof(NeuronType)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_init), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_init, p_v_init, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_rest), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_rest, p_v_rest, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_reset), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_reset, p_v_init, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_cm), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_cm, p_cm, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_m), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_m, p_tau_m, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_refrac), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_refrac, p_tau_refrac, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_syn_E), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_syn_E, p_tau_syn_E, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_tau_syn_I), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_tau_syn_I, p_tau_syn_E, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_v_thresh), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_v_thresh, p_v_thresh, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_offset), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_offset, p_i_offset, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_syn), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_syn, p_i_syn, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_vm), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_vm, p_vm, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p__dt), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p__dt, p__dt, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_C1), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_C1, p_C1, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_C2), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_C2, p_C2, sizeof(real)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_i_tmp), sizeof(real)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_i_tmp, p_i_tmp, sizeof(real)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->p_refrac_step), sizeof(int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->p_refrac_step, p_refrac_step, sizeof(int)*num, cudaMemcpyHostToDevice));

	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pSynapsesNum), sizeof(unsigned int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pSynapsesNum, pSynapsesNum, sizeof(unsigned int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pSynapsesLoc), sizeof(unsigned int)*num));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pSynapsesLoc, pSynapsesLoc, sizeof(unsigned int)*num, cudaMemcpyHostToDevice));
	checkCudaErrors(cudaMalloc((void**)&(pGpuNeurons->pSynapsesIdx), sizeof(unsigned int)*synapsesNum));
	checkCudaErrors(cudaMemcpy(pGpuNeurons->pSynapsesIdx, pSynapsesIdx, sizeof(unsigned int)*synapsesNum, cudaMemcpyHostToDevice));

	return 0;
}

int freeGNeurons(GLIFNeurons * pGpuNeurons)
{
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
