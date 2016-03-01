/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>

#include "GAlphaSynapses.h"

int GAlphaSynapses::allocSynapses(int S)
{
	num = S;
	pID = (ID*)malloc(S*sizeof(ID));
	pType = (SpikeType*)malloc(S*sizeof(SpikeType));
	p_weight = (real*)malloc(S*sizeof(real));
	p_delay = (real*)malloc(S*sizeof(real));
	p_C1 = (real*)malloc(S*sizeof(real));
	p_C2 = (real*)malloc(S*sizeof(real));
	p__C1 = (real*)malloc(S*sizeof(real));
	p__C2 = (real*)malloc(S*sizeof(real));
	p_tau_syn = (real*)malloc(S*sizeof(real));
	p_I_syn = (real*)malloc(S*sizeof(real));
	p_I_tmp = (real*)malloc(S*sizeof(real));
	p__dt = (real*)malloc(S*sizeof(real));
	pSrc = (int*)malloc(S*sizeof(int));
	pDst = (int*)malloc(S*sizeof(int));

	return 0;
}
