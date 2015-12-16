/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>

#include "GLIFNeurons.h"

int GLIFNeurons::allocNeurons(unsigned int N)
{
	num = N;
	pID = (ID*)malloc(N*sizeof(ID));
	pType = (NeuronType*)malloc(N*sizeof(NeuronType));
	p_v_init = (real*)malloc(N*sizeof(real));
	p_v_rest = (real*)malloc(N*sizeof(real));
	p_v_reset = (real*)malloc(N*sizeof(real));
	p_cm = (real*)malloc(N*sizeof(real));
	p_tau_m = (real*)malloc(N*sizeof(real));
	p_tau_refrac = (real*)malloc(N*sizeof(real));
	p_tau_syn_E = (real*)malloc(N*sizeof(real));
	p_tau_syn_I = (real*)malloc(N*sizeof(real));
	p_v_thresh = (real*)malloc(N*sizeof(real));
	p_i_offset = (real*)malloc(N*sizeof(real));
	p_i_syn = (real*)malloc(N*sizeof(real));
	p_vm = (real*)malloc(N*sizeof(real));
	p__dt = (real*)malloc(N*sizeof(real));
	p_C1 = (real*)malloc(N*sizeof(real));
	p_C2 = (real*)malloc(N*sizeof(real));
	p_i_tmp = (real*)malloc(N*sizeof(real));
	p_refrac_step = (int*)malloc(N*sizeof(int));
	pSynapsesNum = (unsigned int*)malloc(N*sizeof(unsigned int));
	pSynapsesLoc = (unsigned int*)malloc(N*sizeof(unsigned int));
	
	return 0;
}

int GLIFNeurons::allocConnects(unsigned int S)
{
	synapsesNum = S;
	pSynapsesIdx = (unsigned int*)malloc(S*sizeof(unsigned int));
	return 0;
}
