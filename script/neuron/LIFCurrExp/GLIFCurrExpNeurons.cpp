
#include <stdlib.h>
#include "mpi.h"
#include "../utils/macros.h"
#include "../utils/TagPool.h"
#include "GLIFCurrExpNeurons.h"

NEURON_GPU_FUNC_BASIC(LIFCurrExp)

int allocLIFCurrExp(void *pCpu, int N)
{
	GLIFCurrExpNeurons *p = (GLIFCurrExpNeurons*)pCpu;
	p->p_i_offset = (real*)malloc(N*sizeof(real));
	p->p_i_injection = (real*)malloc(N*sizeof(real));
	p->p_cm = (real*)malloc(N*sizeof(real));
	p->p_tau_m = (real*)malloc(N*sizeof(real));
	p->p_tau_e = (real*)malloc(N*sizeof(real));
	p->p_tau_i = (real*)malloc(N*sizeof(real));
	p->p_vm = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_i_E = (real*)malloc(N*sizeof(real));
	p->p_i_I = (real*)malloc(N*sizeof(real));
	p->p_refrac_time = (int*)malloc(N*sizeof(int));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	return 0;
}

int freeLIFCurrExp(void *pCpu)
{
	GLIFCurrExpNeurons *pCpuNeurons = (GLIFCurrExpNeurons*)pCpu;
	free(pCpuNeurons->p_i_offset);
	free(pCpuNeurons->p_i_injection);
	free(pCpuNeurons->p_cm);
	free(pCpuNeurons->p_tau_m);
	free(pCpuNeurons->p_tau_e);
	free(pCpuNeurons->p_tau_i);
	free(pCpuNeurons->p_vm);
	free(pCpuNeurons->p_v_thresh);
	free(pCpuNeurons->p_v_reset);
	free(pCpuNeurons->p_v_reset);
	free(pCpuNeurons->p_i_E);
	free(pCpuNeurons->p_i_I);
	free(pCpuNeurons->p_refrac_time);
	free(pCpuNeurons->p_refrac_step);
	return 0;
}

void mpiSendLIFCurrExp(void *data, int rank, int offset, int size)
{
}

void mpiRecvLIFCurrExp(void **data, int rank, int rankSize)
{
}

