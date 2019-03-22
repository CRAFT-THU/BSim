
#include <stdlib.h>
#include "mpi.h"
#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GTJNeurons.h"

NEURON_GPU_FUNC_BASIC(TJ)

int allocTJ(void *pCpu, int N)
{
	GTJNeurons *p = (GTJNeurons*)pCpu;
	p->p_vm = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_i_tmp = (real*)malloc(N*sizeof(real));
	p->p_cm = (real*)malloc(N*sizeof(real));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	p->p_refrac_time = (int*)malloc(N*sizeof(int));
	return 0;
}

int freeTJ(void *pCpu)
{
	GTJNeurons *pCpuNeurons = (GTJNeurons*)pCpu;
	free(pCpuNeurons->p_vm);
	free(pCpuNeurons->p_v_thresh);
	free(pCpuNeurons->p_v_reset);
	free(pCpuNeurons->p_i_tmp);
	free(pCpuNeurons->p_cm);
	free(pCpuNeurons->p_refrac_step);
	free(pCpuNeurons->p_refrac_time);
	return 0;
}

void mpiSendTJ(void *data, int rank, int offset, int size)
{
}

void mpiRecvTJ(void **data, int rank, int rankSize)
{
}

