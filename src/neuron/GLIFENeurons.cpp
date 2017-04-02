
#include <stdlib.h>
#include "mpi.h"
#include "../utils/macros.h"
#include "../utils/TagPool.h"
#include "GLIFENeurons.h"

NEURON_GPU_FUNC_BASIC(LIFE)

int allocLIFE(void *pCpu, int N)
{
	GLIFENeurons *p = (GLIFENeurons*)pCpu;
	p->p_CI = (real*)malloc(N*sizeof(real));
	p->p_vm = (real*)malloc(N*sizeof(real));
	p->p_CE = (real*)malloc(N*sizeof(real));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	p->p_refrac_time = (int*)malloc(N*sizeof(int));
	p->p_i_tmp = (real*)malloc(N*sizeof(real));
	p->p_i_I = (real*)malloc(N*sizeof(real));
	p->p_i_E = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_C2 = (real*)malloc(N*sizeof(real));
	p->p_C1 = (real*)malloc(N*sizeof(real));
	return 0;
}

int freeLIFE(void *pCpu)
{
	GLIFENeurons *pCpuNeurons = (GLIFENeurons*)pCpu;
	freepCpuNeurons->p_CI);
	freepCpuNeurons->p_vm);
	freepCpuNeurons->p_CE);
	freepCpuNeurons->p_refrac_step);
	freepCpuNeurons->p_refrac_time);
	freepCpuNeurons->p_i_tmp);
	freepCpuNeurons->p_i_I);
	freepCpuNeurons->p_i_E);
	freepCpuNeurons->p_v_thresh);
	freepCpuNeurons->p_v_reset);
	freepCpuNeurons->p_C2);
	freepCpuNeurons->p_C1);
	return 0;
}

void mpiSendLIFE(void *data, int rank, int offset, int size)
{
}

void mpiRecvLIFE(void **data, int rank, int rankSize)
{
}

