/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>

#include "GAlphaSynapses.h"

void *createAlphaSynapses()
{
	return malloc(sizeof(GAlphaSynapses));
}

size_t getAlphaSize()
{
	return sizeof(GAlphaSynapses);
}

int allocAlphaSynapses(void *pSynapses, int S)
{
	GAlphaSynapses * p = (GAlphaSynapses*)pSynapses;
	p->num = S;
	p->pID = (ID*)malloc(S*sizeof(ID));
	p->pType = (Type*)malloc(S*sizeof(Type));
	p->p_weight = (real*)malloc(S*sizeof(real));
	p->p_delay = (real*)malloc(S*sizeof(real));
	p->p_C1 = (real*)malloc(S*sizeof(real));
	p->p_C2 = (real*)malloc(S*sizeof(real));
	p->p__C1 = (real*)malloc(S*sizeof(real));
	p->p__C2 = (real*)malloc(S*sizeof(real));
	p->p_tau_syn = (real*)malloc(S*sizeof(real));
	p->p_I_syn = (real*)malloc(S*sizeof(real));
	p->p_I_tmp = (real*)malloc(S*sizeof(real));
	p->p__dt = (real*)malloc(S*sizeof(real));
	//p->pSrc = (int*)malloc(S*sizeof(int));
	//p->pDst = (int*)malloc(S*sizeof(int));

	return 0;
}

int allocExpConnects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3)
{
	GAlphaSynapses *p = (GAlphaSynapses*)pCpu;
	p->pDst = pSynapsesDst;

	return 0;
}
