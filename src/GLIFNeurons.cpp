/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>

#include "GLIFNeurons.h"

void* createLIFNeurons()
{
	return malloc(sizeof(GLIFNeurons));
}

size_t getLIFSize()
{
	return sizeof(GLIFNeurons);
}

int allocLIFNeurons(void *pCpu, int N)
{
	GLIFNeurons *p = (GLIFNeurons *)pCpu;
	p->num = N;
	p->pID = (ID*)malloc(N*sizeof(ID));
	p->pType = (Type*)malloc(N*sizeof(Type));
	p->p_v_init = (real*)malloc(N*sizeof(real));
	p->p_v_rest = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_cm = (real*)malloc(N*sizeof(real));
	p->p_tau_m = (real*)malloc(N*sizeof(real));
	p->p_tau_refrac = (real*)malloc(N*sizeof(real));
	p->p_tau_syn_E = (real*)malloc(N*sizeof(real));
	p->p_tau_syn_I = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	p->p_i_offset = (real*)malloc(N*sizeof(real));
	p->p_i_syn = (real*)malloc(N*sizeof(real));
	p->p_vm = (real*)malloc(N*sizeof(real));
	p->p__dt = (real*)malloc(N*sizeof(real));
	p->p_C1 = (real*)malloc(N*sizeof(real));
	p->p_C2 = (real*)malloc(N*sizeof(real));
	p->p_i_tmp = (real*)malloc(N*sizeof(real));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	//p->pSynapsesNum = (int*)malloc(N*sizeof(int));
	//p->pSynapsesLoc = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int allocLIFConnects(void *pCpu, int *pSynapsesNum, int *pSynapsesLoc, int *pSynapsesIdx, int S)
{
	GLIFNeurons *p = (GLIFNeurons *)pCpu;
	p->synapsesNum = S;
	p->pSynapsesIdx = pSynapsesIdx;
	p->pSynapsesNum = pSynapsesNum;
	p->pSynapsesLoc = pSynapsesLoc;

	return 0;
}
