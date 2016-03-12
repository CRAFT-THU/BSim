/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>

#include "GExpSynapses.h"

void *createExpSynapses()
{
	return malloc(sizeof(GExpSynapses));
}

size_t getExpSize()
{
	return sizeof(GExpSynapses);
}

int allocExpSynapses(void *pSynapses, int S)
{
	GExpSynapses *p = (GExpSynapses*)pSynapses;
	p->num = S;
	p->pID = (ID*)malloc(S*sizeof(ID));
	p->pType = (Type*)malloc(S*sizeof(Type));
	p->p_weight = (real*)malloc(S*sizeof(real));
	p->p_delay = (real*)malloc(S*sizeof(real));
	p->p_C1 = (real*)malloc(S*sizeof(real));
	p->p__C1 = (real*)malloc(S*sizeof(real));
	p->p_tau_syn = (real*)malloc(S*sizeof(real));
	p->p_I_syn = (real*)malloc(S*sizeof(real));
	p->p__dt = (real*)malloc(S*sizeof(real));
	//p->pSrc = (int *)malloc(S*sizeof(int));
	//p->pDst = (int *)malloc(S*sizeof(int));

	return 0;
}

int allocExpConnects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3)
{
	GExpSynapses *p = (GExpSynapses*)pCpu;
	p->pDst = pSynapsesDst;

	return 0;
}
