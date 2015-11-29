/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <map>
#include "Neuron.h"
#include "Synapse.h"
#include "Population.h"

using std::vector;
using std::map;
using std::pair;

class Network {
public:
	Network();
	//Network(NeuronBase *pn1, NeuronBase *pn2, SynapseBase *pSyn);
	~Network();

	int connect(NeuronBase *pn1);
	//int connect(NeuronBase pn1);
	int connect(NeuronBase *pSrc, NeuronBase *pDst, real weight, real delay, SynType type, bool store = true);
	template<class Neuron>
	int connect(Population<Neuron*> *pSrc, Population<Neuron*> *pDst, real *weight, real *delay, SynType *type, int size);

//protected:
	//map<NeuronBase*, vector<SynapseBase*> > network;
	vector<PopulationBase*> pPopulations;
	vector<NeuronBase*> pNeurons;
	vector<SynapseBase*> pSynapses;
};

#endif /* NETWORK_H */

