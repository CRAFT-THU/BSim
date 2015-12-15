/* This program is writen by qp09.
 * usually just for fun.
 * Mon December 14 2015
 */

#include <stdlib.h>

#include "./utils/cuda/helper_cuda.h"
#include "GLIFNeuron.h"

int GLIFNeurons::allocNeurons(unsigned int N)
{
	num = N;
	pID = (ID*)malloc(N*sizeof(ID));
	pType = (NeuronType*)malloc(N*sizeof(NeuronType));
	p_v_init = (real*)malloc(N*sizeof(real));
	p_v_rest = (real*)malloc(N*sizeof(real));
	p_v_reset = (real*)malloc(N*sizeof(real));
	p_cm = (real*)malloc(N*sizeof(real));
	p_tau_m = (real*)malloc(N*sizeof(real));
	p_tau_refrac = (real*)malloc(N*sizeof(real));
	p_tau_syn_E = (real*)malloc(N*sizeof(real));
	p_tau_syn_I = (real*)malloc(N*sizeof(real));
	p_v_thresh = (real*)malloc(N*sizeof(real));
	p_i_offset = (real*)malloc(N*sizeof(real));
	p_i_syn = (real*)malloc(N*sizeof(real));
	p_vm = (real*)malloc(N*sizeof(real));
	p__dt = (real*)malloc(N*sizeof(real));
	p_C1 = (real*)malloc(N*sizeof(real));
	p_C2 = (real*)malloc(N*sizeof(real));
	p_i_tmp = (real*)malloc(N*sizeof(real));
	p_refrac_step = (int*)malloc(N*sizeof(int));
	pSynapsesNum = (unsigned int*)malloc(N*sizeof(unsigned int));
	pSynapsesLoc = (unsigned int*)malloc(N*sizeof(unsigned int));
	
	return 0;
}

int GLIFNeurons::allocConnects(unsigned int S)
{
	synapsesNum = S;
	pSynapsesIdx = (unsigned int*)malloc(S*sizeof(unsigned int));
	return 0;
}

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

