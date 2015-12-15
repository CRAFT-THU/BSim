/* This header file is writen by qp09
 * usually just for fun
 * Mon December 14 2015
 */
#ifndef GLIFNEURON_H
#define GLIFNEURON_H

#include "ID.h"

struct GLIFNeurons {
	int num;
	int synapsesNum;
	ID *pID;
	NeuronType* pType;
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
	unsigned int *pSynapsesNum;
	unsigned int *pSynapsesLoc;
	unsigned int *pSynapsesIdx;

	int allocNeurons(unsigned int N);
	int allocConnects(unsigned int N);
	int allocGNeurons(GLIFNeurons * pGpuNeurons);
};

#endif /* GLIFNEURON_H */
