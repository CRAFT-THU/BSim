/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <algorithm>
#include "Network.h"

using std::find;

Network::Network()
{
}


Network::~Network()
{
	if (!pPopulations.empty()) {
		vector<PopulationBase*>::iterator iter;
		for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
			PopulationBase * t = *iter;
			delete t;
		}
	}

	if (!pNeurons.empty()) {
		vector<NeuronBase*>::iterator iter;
		for (iter = pNeurons.begin(); iter != pNeurons.end(); iter++) {
			NeuronBase * t = *iter;
			delete t;
		}
	}

	if (!pSynapses.empty()) {
		vector<SynapseBase*>::iterator iter;
		for (iter = pSynapses.begin(); iter != pSynapses.end(); iter++) {
			SynapseBase * t = *iter;
			delete t;
		}
	}

	pPopulations.clear();
	pNeurons.clear();
	pSynapses.clear();
}

int Network::connect(NeuronBase *pn1)
{
	if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
		pNeurons.push_back(pn1);
	}

	return 1;
}

int Network::connect(NeuronBase *pn1, NeuronBase *pn2, real weight, real delay, SynType type, bool store)
{
	if (store) {
		if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
			pNeurons.push_back(pn1);
		}
		if (find(pNeurons.begin(), pNeurons.end(), pn2) == pNeurons.end()) {
			pNeurons.push_back(pn2);
		}
	}

	SynapseBase * p = pn1->addSyn(weight, delay, type, pn2);

	pSynapses.push_back(p);
	
	return 1;
}

template<class Neuron>
int Network::connect(Population<Neuron*> *pSrc, Population<Neuron*> *pDst, real *weight, real *delay, SynType *type, int size) {
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

PlainNetwork* Network::buildNetwrok()
{
	PlainNetwork * ret = (PlainNetwork*)malloc(sizeof(PlainNetwork));
	if (ret == NULL) {
		printf("Malloc PlainNetwork failed/n");
		return NULL;
	}
	size_t populationSize = 0;
	size_t neuronSize = 0;
	size_t synapseSize = 0;
	vector<PopulationBase*>::iterator piter;
	for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		populationSize += p->getSize();
	}
	vector<NeuronBase*>::iterator niter;
	for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
		NeuronBase * p = *niter;
		neuronSize += p->getSize();
	}
	vector<SynapseBase*>::iterator siter;
	for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		synapseSize += p->getSize();
	}

	ret->populationSize = populationSize;
	ret->neuronSize = neuronSize;
	ret->synapseSize = synapseSize;

	ret->pPopulations = malloc(populationSize);
	ret->pNeurons = malloc(neuronSize);
	ret->pSynapses = malloc(synapseSize);

	if ((ret->pPopulations == NULL) || (ret->pNeurons == NULL) || (ret->pSynapses == NULL)) {
		printf("Malloc PlainNetwork failed/n");
		free(ret->pPopulations);
		free(ret->pNeurons);
		free(ret->pSynapses);
		free(ret);
		return NULL;
	}

	size_t idx = 0;
	for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		size_t copied = p->hardCopy(((unsigned char*)ret->pPopulations) + idx);
		idx += copied;
	}
	idx = 0;
	for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
		NeuronBase * p = *niter;
		size_t copied = p->hardCopy(((unsigned char*)ret->pNeurons) + idx);
		idx += copied;
	}
	idx = 0;
	for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		size_t copied = p->hardCopy(((unsigned char*)ret->pSynapses) + idx);
		idx += copied;
	}

	return ret;
}
