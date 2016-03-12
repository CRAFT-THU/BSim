/* This header file is writen by qp09
 * usually just for fun
 * Mon January 18 2016
 */
#ifndef GNETWORK_H
#define GNETWORK_H

#include "constant.h"

struct GNetwork {
	void **pNeurons;
	void **pSynapses;

	int nTypeNum;
	int sTypeNum;
	Type * nTypes;
	Type * sTypes;
	int *neuronNums;
	int *synapseNums;
	real MAX_DELAY;
};

#endif /* GNETWORK_H */

