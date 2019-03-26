
#include <stdlib.h>
#include "mpi.h"
#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GLIFNeurons.h"

NEURON_GPU_FUNC_BASIC(LIF)

int allocLIF(void *pCpu, int N)
{
	GLIFNeurons *p = (GLIFNeurons*)pCpu;
	p->p_vm = (real*)malloc(N*sizeof(real));
	p->p_CI = (real*)malloc(N*sizeof(real));
	p->p_CE = (real*)malloc(N*sizeof(real));
	p->p_C_I = (real*)malloc(N*sizeof(real));
	p->p_C_E = (real*)malloc(N*sizeof(real));
	p->p_v_tmp = (real*)malloc(N*sizeof(real));
	p->p_i_I = (real*)malloc(N*sizeof(real));
	p->p_i_E = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_Cm = (real*)malloc(N*sizeof(real));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	p->p_refrac_time = (int*)malloc(N*sizeof(int));
	//p->p_start_E = (int*)malloc(N*sizeof(int));
	//p->p_start_I = (int*)malloc(N*sizeof(int));
	//p->p_end = (int*)malloc(N*sizeof(int));
	return 0;
}

int freeLIF(void *pCpu)
{
	GLIFNeurons *pCpuNeurons = (GLIFNeurons*)pCpu;
	free(pCpuNeurons->p_vm);
	free(pCpuNeurons->p_CI);
	free(pCpuNeurons->p_CE);
	free(pCpuNeurons->p_C_I);
	free(pCpuNeurons->p_C_E);
	free(pCpuNeurons->p_v_tmp);
	free(pCpuNeurons->p_i_I);
	free(pCpuNeurons->p_i_E);
	free(pCpuNeurons->p_v_thresh);
	free(pCpuNeurons->p_v_reset);
	free(pCpuNeurons->p_Cm);
	free(pCpuNeurons->p_refrac_step);
	free(pCpuNeurons->p_refrac_time);
	//free(pCpuNeurons->p_start_E);
	//free(pCpuNeurons->p_start_I);
	//free(pCpuNeurons->p_end);
	return 0;
}

int mpiSendLIF(void *data, int rank, int offset, int size)
{
	return 0;
}

int mpiRecvLIF(void **data, int rank, int rankSize)
{
	return 0;
}

