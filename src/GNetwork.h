/* This header file is writen by qp09
 * usually just for fun
 * Mon January 18 2016
 */
#ifndef GNETWORK_H
#define GNETWORK_H

#include "constant.h"

struct GNetwork {
	void *pNeurons;
	void *pSynapses;
	//void *pPopulations;
	//size_t neuronSize;
	//size_t synapseSize;
	//size_t populationSize;
	int neuronNum;
	int synapseNum;
	real MAX_DELAY;
};

#endif /* GNETWORK_H */

