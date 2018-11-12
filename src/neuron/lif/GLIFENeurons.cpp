
#include <stdlib.h>
#include "mpi.h"
#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GLIFENeurons.h"

NEURON_GPU_FUNC_BASIC(LIFE)

int allocLIFE(void *pCpu, int N)
{
	GLIFENeurons *p = (GLIFENeurons*)pCpu;
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

int freeLIFE(void *pCpu)
{
	GLIFENeurons *pCpuNeurons = (GLIFENeurons*)pCpu;
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

void mpiSendLIFE(void *data, int rank, int offset, int size)
{
}

void mpiRecvLIFE(void **data, int rank, int rankSize)
{
}

