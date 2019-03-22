/* This program is writen by qp09.
 * usually just for fun.
 * Sun December 25 2016
 */


#include <stdlib.h>
#include "mpi.h"

#include "../../utils/macros.h"
#include "../../utils/TagPool.h"
#include "GFFTNeurons.h"

NEURON_GPU_FUNC_BASIC(FFT)

int allocFFT(void *pCpu, int N) 
{
	GFFTNeurons *p = (GFFTNeurons *)pCpu;
	p->p_input = (cufftComplex*)malloc(N/2*sizeof(cufftComplex));
	p->p_res = (cufftComplex*)malloc(N/2*sizeof(cufftComplex));
	p->p_fire_count = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int freeFFT(void *pCpu) 
{
	GFFTNeurons *p = (GFFTNeurons *)pCpu;
	free(p->p_input); 
	free(p->p_res); 
	free(p->p_fire_count);

	return 0;
}


void mpiSendFFT(void *data, int rank, int offset, int size) {
}

void mpiRecvFFT(void **data, int rank, int rankSize) {
}
