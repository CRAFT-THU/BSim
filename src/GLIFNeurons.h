/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GLIFNEURON_H
#define GLIFNEURON_H

#include "ID.h"

struct GLIFNeurons {
	ID *pID;
	Type* pType;
	real *p_v_init;
	real *p_v_rest;
	real *p_v_reset;
	real *p_cm;
	real *p_tau_m;
	real *p_tau_refrac;
	real *p_tau_syn_E;
	real *p_tau_syn_I;
	real *p_v_thresh;
	real *p_i_offset;
	real *p_i_syn;
	real *p_vm;
	real *p__dt;
	real *p_C1;
	real *p_C2;
	real *p_i_tmp;
	int *p_refrac_step;
	int *pSynapsesNum;
	int *pSynapsesLoc;
	int *pSynapsesIdx;
	int num;
	int synapsesNum;
};

void* createLIFNeurons();
size_t getLIFSize();
int allocLIFNeurons(void *pCpu, int N);
int allocLIFConnects(void *pCpu, int *pSynapsesNum, int *pSynapsesLoc, int *pSynapsesIdx, int S);
void sendLIFNeurons(void *data, int rank, int offset, int size);
void recvLIFNeurons(void **data, int rank, int size);

int cudaAllocLIFNeurons(void *pCpu, void *pGpu);
int cudaFreeLIFNeurons(void *pGpu);

#endif /* GLIFNEURON_H */
