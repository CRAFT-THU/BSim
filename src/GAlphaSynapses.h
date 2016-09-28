/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GALPHASYNAPSES_H
#define GALPHASYNAPSES_H

#include "ID.h"

struct GAlphaSynapses {
	ID *pID;
	Type *pType;
	real *p_weight;
	real *p_delay;
	real *p_C1;
	real *p_C2;
	real *p__C1;
	real *p__C2;
	real *p_tau_syn;
	real *p_I_syn;
	real *p_I_tmp;
	real *p__dt;
	//int *pSrc;
	int *pDst;
	int num;
};

void *createAlphaSynapses();
size_t getAlphaSize();
int allocAlphaSynapses(void *pSynapses, int S);
int allocAlphaConnects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3);

void sendAlphaSynapses(void *data, int rank, int offset, int size);
void recvAlphaSynapses(void **data, int rank, int size);

int cudaAllocAlphaSynapses(void *pCpu, void *pGpu);
int cudaFreeAlphaSynapses(void *pGpu);

#endif /* GALPHASYNAPSES_H */

