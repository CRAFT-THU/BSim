/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 25 2016
 */


#include <stdlib.h>
#include "mpi.h"

#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GArrayNeurons.h"

NEURON_GPU_FUNC_BASIC(Array)

int allocArray(void *pCpu, int N) {
	GArrayNeurons *p = (GArrayNeurons *)pCpu;
	p->p_start = (int*)malloc(N*sizeof(int));
	p->p_end = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int freeArray(void *pCpu) {
	GArrayNeurons *p = (GArrayNeurons *)pCpu;
	free(p->p_start);
	free(p->p_end);
	free(p->p_fire_time);
	
	return 0;
}

int mpiSendArray(void *data, int rank, int offset, int size) {
	return 0;
}

int mpiRecvArray(void **data, int rank, int rankSize) {
	return 0;
}
