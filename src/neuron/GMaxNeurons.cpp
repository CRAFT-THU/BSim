/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 25 2016
 */


#include <stdlib.h>
#include "mpi.h"

#include "../utils/macros.h"
#include "../utils/TagPool.h"
#include "GMaxNeurons.h"

NEURON_GPU_FUNC_BASIC(Max)

int allocMax(void *pCpu, int N) {
	GMaxNeurons *p = (GMaxNeurons *)pCpu;
	p->p_N = (int*)malloc(N*sizeof(int));
	p->p_count = (int*)malloc(N*sizeof(int));
	p->max_N = 0;
	
	return 0;
}

void mpiSendMax(void *data, int rank, int offset, int size) {
}

void mpiRecvMax(void **data, int rank, int rankSize) {
}
