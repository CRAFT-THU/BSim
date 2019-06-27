/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NETWORK_H
#define NETWORK_H

#include <assert.h>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

#include "../utils/SimInfo.h"
#include "../interface/Neuron.h"
#include "../interface/Synapse.h"
#include "../utils/ModelArray.h"
#include "GNetwork.h"

using std::pair;
using std::find;
using std::array;
using std::vector;
using std::map;
using std::set;

class Network {
public:
	Network();
	~Network();

	//template<class N>
	//Population<N>* createNeuron(N n1);
	template<class N>
	Population* createPopulation(int id, int num, N templ, bool empty = false);

	template<class N>
	Population* createPopulation(int num, N templ, bool empty = false);

	int connect(Population *pSrc, Population *pDst, real weight, real delay, SpikeType type);
	int connect(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size);
	int connectOne2One(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size);
	int connectConv(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int height, int width, int k_height, int k_width);
	int connectPooling(Population *pSrc, Population *pDst, real weight, int height, int width, int p_height, int p_width);
	
	int connect(int populationIDSrc, int neuronIDSrc, int populationIDDst, int neuronIDDst, real weight, real delay, real tau = 0);
	Synapse* connect(Neuron *pSrc, Neuron *pDst, real weight, real delay, SpikeType type = Excitatory, real tau = 0, bool store = true);

	GNetwork* buildNetwork(SimInfo &info);

	int addNeuronNum(Type type, int num);
	int addConnectionNum(Type type, int num);
	int addSynapseNum(Type type, int num);

	// int addMonitor(int populationIDSrc, int neuronIDSrc);
	// int addOutput(int populationIDSrc, int neuronIDSrc);
	// int addProbe(int populationIDSrc, int neuronIDSrc, double weight = 1);
	Population* findPopulation(int populationID);
	Neuron* findNeuron(int populationIDSrc, int neuronIDSrc);

	int reset(SimInfo &info);
	// int update(SimInfo &info);
	// void monitor(SimInfo &info);

	void logMap();
private:
	void mapIDtoIdx(GNetwork *net);
	//bool checkIDtoIdx();

public:
	vector<Neuron*> pOutputs;
	vector<Population*> pPopulations;
	vector<Synapse*> pSynapses;
	//map<ID, vector<ID> > n2sNetwork;
	//map<ID, vector<ID> > n2sTargetNetwork;
	//map<ID, ID> s2nNetwork;
	//map<ID, ID> s2nForwardNetwork;
	//map<ID, Neuron*> id2neuron;
	//map<ID, Synapse*> id2synapse;
	//map<ID, int> nid2idx;
	//map<ID, int> sid2idx;
	//map<int, ID> idx2nid;
	//map<int, ID> idx2sid;

	// int maxDelaySteps;
	// int minDelaySteps;
	real maxDelay;
	real minDelay;
	int totalNeuronNum;
	int totalSynapseNum;
private:
	real maxFireRate;
	vector<Type> nTypes;
	vector<Type> sTypes;
	int populationNum;
	vector<int> neuronNums;
	vector<int> connectNums;
	vector<int> synapseNums;
};

//template<class Neuron>
//Population<N>* Network::createNeuron(N n1)
//{
//	//N *pn1 = new N(n1);
//	//if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
//	//	pNeurons.push_back(pn1);
//	//	addNeuronNum(pn1->getType(), 1);
//	//}
//	//
//	
//	Population<N> * pn1 = createPopulation(n1.getID().getId(), 1, n1);
//	
//	return pn1;
//}

template<class N>
Population * Network::createPopulation(int id, int num, N templ, bool empty)
{
	return createPopulation(num, templ, empty);
}

template<class N>
Population * Network::createPopulation(int num, N templ, bool empty)
{
	//ID id = totalNeuronNum;
	Population * pp1 = NULL;
	if (empty) {
		pp1 = new Population(num, templ.getType());
	} else {
		pp1 = new Population(num, templ);
	}

	if (find(pPopulations.begin(), pPopulations.end(), pp1) == pPopulations.end()) {
		pPopulations.push_back(pp1);
		populationNum++;
		addNeuronNum(pp1->getType(), pp1->getNum());
	}

	return pp1;
}

#endif /* NETWORK_H */

