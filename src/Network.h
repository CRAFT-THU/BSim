/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NETWORK_H
#define NETWORK_H

#include <vector>
#include <map>
#include <algorithm>
#include "Neuron.h"
#include "Synapse.h"
#include "Population.h"

using std::map;
using std::pair;
using std::find;
using std::vector;

struct PlainNetwork {
	void *pNeurons;
	void *pSynapses;
	void *pPopulations;
	void *network;
	size_t neuronSize;
	size_t synapseSize;
	size_t populationSize;
};

class Network {
public:
	Network();
	~Network();

	template<class Neuron>
	Neuron* create(Neuron n1);
	template<class Neuron>
	int connect(Population<Neuron> *pSrc, Population<Neuron> *pDst, real *weight, real *delay, SpikeType *type, int size);
	
	int connect(NeuronBase *pSrc, NeuronBase *pDst, real weight, real delay, SpikeType type, bool store = true);
	PlainNetwork* buildNetwrok();

//protected:
	vector<PopulationBase*> pPopulations;
	vector<NeuronBase*> pNeurons;
	vector<SynapseBase*> pSynapses;
	map<NeuronBase*, vector<SynapseBase*> > network;
};

template<class Neuron>
Neuron* Network::create(Neuron n1)
{
	NeuronBase *pn1 = new Neuron(n1);
	if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
		pNeurons.push_back(pn1);
	}

	return (Neuron*) pn1;
}

template<class Neuron>
int Network::connect(Population<Neuron> *pSrc, Population<Neuron> *pDst, real *weight, real *delay, SpikeType *type, int size) {
	int srcSize = pSrc->getSize();
	int dstSize = pDst->getSize();
	if (size != srcSize * dstSize) {
		return -1;
	}	
	if (find(pPopulations.begin(), pPopulations.end(), pSrc) == pPopulations.end()) {
		pPopulations.push_back(pSrc);
	}
	if (find(pPopulations.begin(), pPopulations.end(), pDst) == pPopulations.end()) {
		pNeurons.push_back(pDst);
	}

	int count = 0;
	for (int i=0; i<size; i++) {
		int iSrc = i/dstSize;
		int iDst = i%dstSize;
		connect(pSrc->getNeuron(iSrc), pDst->getNeuron(iDst), weight[i], delay[i], type[i], false);
		count++;
	}

	return count;
}

#endif /* NETWORK_H */

