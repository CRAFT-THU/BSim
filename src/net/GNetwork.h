/* This header file is writen by qp09
 * usually just for fun
 * Mon January 18 2016
 */
#ifndef GNETWORK_H
#define GNETWORK_H

#include "../utils/type.h"

#include "Connection.h"

struct GNetwork {
	//Numbers of types
	int nTypeNum;
	int sTypeNum;

	// Delay info moved into connection
	// int maxDelay;
	// int minDelay;

	//Type 
	Type * pNTypes;
	Type * pSTypes;

	//Index for each type
	int *pNeuronNums;
	int *pSynapseNums;

	//Pointers to neurons
	void **ppNeurons;
	//Pointers to synapses
	void **ppSynapses;

	//Neuron to Synapse Connection
	Connection *pConnection;

};


// init and free
// This func just set pConnection to NULL
GNetwork * allocNetwork(int nTypeNum, int sTypeNum);
// TODO freeGNetwork
void freeGNetwork(GNetwork * network);

// Save and Load
int saveGNetwork(GNetwork *, char *filename);
GNetwork *loadGNetwork(char *filename);

// Transfer GNetwork between CPU and GPU
// Only copy inside data arrays to GPU, the info data is left on CPU
GNetwork* copyNetworkToGPU(GNetwork *);
int fetchNetworkFromGPU(GNetwork *, GNetwork*);
int freeNetworkGPU(GNetwork *);

// MPI
int copyNetwork(GNetwork *dNet, GNetwork *sNet, int rank, int rankSize);
int mpiSendNetwork(GNetwork *network, int rank, int rankSize);
int mpiRecvNetwork(GNetwork *network, int rank, int rankSize);


// Other utils
int checkNetworkGPU(GNetwork *g, GNetwork *c);
int printNetwork(GNetwork *net, int rank = 0);

#endif /* GNETWORK_H */

