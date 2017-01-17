/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 25 2016
 */


#include <stdlib.h>
#include "mpi.h"

#include "../utils/macros.h"
#include "../utils/IDPool.h"
#include "GConstantNeurons.h"

NEURON_GPU_FUNC_BASIC(Constant)

int allocConstantNeurons(void *pCpu, int N) {
	GConstantNeurons *p = (GConstantNeurons *)pCpu;
	p->p_fire_rate = (real*)malloc(N*sizeof(real));
	p->p_fire_count = (int*)malloc(N*sizeof(int));
	
	return 0;
}

void sendConstantNeurons(void *data, int rank, int offset, int size) {
}

void recvConstantNeurons(void **data, int rank, int rankSize) {
}
