
#include <stdlib.h>
#include "mpi.h"
#include "../utils/macros.h"
#include "../utils/TagPool.h"
#include "GStaticSynapses.h"

SYNAPSE_GPU_FUNC_BASIC(Static)

int allocStatic(void *pCpu, int N)
{
	GStaticSynapses *p = (GStaticSynapses*)pCpu;
	p->p_weight = (real*)malloc(N*sizeof(real));
	//p->p_delay = (int*)malloc(N*sizeof(int));
	//p->p_src = (int *)malloc(N*sizeof(int));
	p->p_dst = (int *)malloc(N*sizeof(int));
	return 0;
}

int freeStatic(void *pCpu)
{
	GStaticSynapses *pCpuSynapses = (GStaticSynapses*)pCpu;
	free(pCpuSynapses->p_weight);
	//free(pCpuSynapses->p_delay);
	//free(pCpuSynapses->p_src);
	free(pCpuSynapses->p_dst);
	return 0;
}

void mpiSendStatic(void *data, int rank, int offset, int size)
{
}

void mpiRecvStatic(void **data, int rank, int rankSize)
{
}

