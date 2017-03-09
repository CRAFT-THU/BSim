/* This header file is writen by qp09
 * usually just for fun
 * Mon January 18 2016
 */
#ifndef GNETWORK_H
#define GNETWORK_H

#include "../base/type.h"
#include "Connection.h"

struct GNetwork {
	//Pointers to neurons
	void **pNeurons;
	//Pointers to synapses
	void **pSynapses;

	//Neuron to Synapse Connection
	N2SConnection *pN2SConnection;

	//Index for each type
	int *neuronNums;
	int *synapseNums;

	//Numbers of types
	int nTypeNum;
	int sTypeNum;

	//Type 
	Type * nTypes;
	Type * sTypes;

	int MAX_DELAY;
};

//This func donot deal with N2SConnection
GNetwork * initGNetwork(int ntype_num, int stype_num);

int copyNetwork(GNetwork *dNet, GNetwork *sNet, int rank, int rankSize);
int mpiSendNetwork(GNetwork *network, int rank, int rankSize);
int mpiRecvNetwork(GNetwork *network, int rank, int rankSize);

int printNetwork(GNetwork *net, int rank = 0);

#endif /* GNETWORK_H */

