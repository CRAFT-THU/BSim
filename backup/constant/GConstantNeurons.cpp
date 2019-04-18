/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 25 2016
 */


#include <stdlib.h>
#include "mpi.h"

#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GConstantNeurons.h"

NEURON_GPU_FUNC_BASIC(Constant)

int allocConstant(void *pCpu, int N) 
{
	GConstantNeurons *p = (GConstantNeurons *)pCpu;
	p->p_fire_rate = (real*)malloc(N*sizeof(real));
	p->p_start_cycle = (int*)malloc(N*sizeof(int));
	p->p_end_cycle = (int*)malloc(N*sizeof(int));
	p->p_fire_count = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int freeConstant(void *pCpu) 
{
	GConstantNeurons *p = (GConstantNeurons *)pCpu;
	free(p->p_fire_rate); 
	free(p->p_start_cycle); 
	free(p->p_end_cycle); 
	free(p->p_fire_count);

	return 0;
}


int mpiSendConstant(void *data, int rank, int offset, int size) {
	return 0;
}

int mpiRecvConstant(void **data, int rank, int rankSize) {
	return 0;
}
