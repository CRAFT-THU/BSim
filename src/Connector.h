/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef CONNECTOR_H
#define CONNECTOR_H

#include <vector>
#include <map>
#include "Neuron.h"
#include "Synapse.h"

using std::vector;
using std::map;
using std::pair;

class Connector {
public:
	Connector();
	//Connector(NeuronBase *pn1, NeuronBase *pn2, SynapseBase *pSyn);
	~Connector();

	int connect(NeuronBase *pn1);
	int connect(NeuronBase *pn1, NeuronBase *pn2, real weight, real delay, SynType type);
	vector<NeuronBase*> pNeurons;
	vector<SynapseBase*> pSynapses;
	//map<NeuronBase*, vector<SynapseBase*> > network;
};

#endif /* CONNECTOR_H */

