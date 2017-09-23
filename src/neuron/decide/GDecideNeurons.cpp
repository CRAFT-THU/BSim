/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 25 2016
 */


#include <stdlib.h>
#include "mpi.h"

#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GMemNeurons.h"

NEURON_GPU_FUNC_BASIC(Mem)

int allocMem(void *pCpu, int N) 
{
	GMemNeurons *p = (GMemNeurons *)pCpu;
	p->p_tmp_rate = (real*)malloc(N*sizeof(real));
	p->p_fire_rate = (real*)malloc(N*sizeof(real));
	p->p_fire_count = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int freeMem(void *pCpu) 
{
	GMemNeurons *p = (GMemNeurons *)pCpu;
	free(p->p_tmp_rate); 
	free(p->p_fire_rate); 
	free(p->p_fire_count);

	return 0;
}


void mpiSendMem(void *data, int rank, int offset, int size) {
}

void mpiRecvMem(void **data, int rank, int rankSize) {
}
