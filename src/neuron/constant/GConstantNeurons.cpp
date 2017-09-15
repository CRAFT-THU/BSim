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
	p->p_fire_count = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int freeConstant(void *pCpu) 
{
	GConstantNeurons *p = (GConstantNeurons *)pCpu;
	free(p->p_fire_rate); 
	free(p->p_fire_count);

	return 0;
}


void mpiSendConstant(void *data, int rank, int offset, int size) {
}

void mpiRecvConstant(void **data, int rank, int rankSize) {
}
