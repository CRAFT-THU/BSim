/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GEXPSYNAPSES_H
#define GEXPSYNAPSES_H

#include "ID.h"

struct GExpSynapses {
	ID *pID;
	Type *pType;
	real *p_weight;
	real *p_delay;
	real *p_C1;
	real *p__C1;
	real *p_tau_syn;
	real *p_I_syn;
	real *p__dt;
	//int *pSrc;
	int *pDst;
	int num;
};

void *createExpSynapses();
size_t getExpSize();
int allocExpSynapses(void *pSynapses, int S);
int allocExpConnects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3);

void sendExpSynapses(void *data, int rank, int offset, int size);
void recvExpSynapses(void **data, int rank, int size);

int cudaAllocExpSynapses(void *pCpu, void *pGpu);
int cudaFreeExpSynapses(void *pGpu);

#endif /* GEXPSYNAPSES_H */

