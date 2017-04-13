/* This header file is writen by qp09
 * usually just for fun
 * Mon January 18 2016
 */
#ifndef GNETWORK_H
#define GNETWORK_H

#include "../base/constant.h"
#include "Connection.h"

struct GNetwork {
	//Local Info
	//Pointers to neurons
	void **pNeurons;
	//Pointers to synapses
	void **pSynapses;
	//Pointer to neuron to synapse connections
	N2SConnection *pN2SConnections;
	//Local machine offsets
	int *nOffsets;
	int *sOffsets;
	//Local index for each type
	int *neuronNums;
	int *synapseNums;

	//Global Info
	//Numbers of types
	int nTypeNum;
	int sTypeNum;
	//Type 
	Type * nTypes;
	Type * sTypes;
	//Global index for each type
	int *gNeuronNums;
	int *gSynapseNums;

	real MAX_DELAY;
};

int copyNetwork(GNetwork *dNet, GNetwork *sNet, int rank, int rankSize);
int mpiSendNetwork(GNetwork *network, int rank, int rankSize);
int mpiRecvNetwork(GNetwork *network, int rank, int rankSize);

int printNetwork(GNetwork *net, int rank = 0);

#endif /* GNETWORK_H */

