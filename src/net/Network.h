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
#include "DistriNetwork.h"

using std::pair;
using std::find;
using std::array;
using std::vector;
using std::map;
using std::set;

class Network {
public:
	Network(int nodeNum = 1);
	~Network();

	int setNodeNum(int nodeNum); 
	
	// template<class N>
	// Population<N>* createNeuron(N n1);
	template<class N>
	Population* createPopulation(int id, int num, N templ, bool empty = false);

	template<class N>
	Population* createPopulation(int num, N templ, bool empty = false);

	template<class S>
	int connect(Population *pSrc, Population *pDst, S templ);

	template<class S>
	int connect(Population *pSrc, Population *pDst, S *pTempl, int size);

	template<class S>
	Synapse* connect(Neuron *pSrc, Neuron *pDst, S templ, bool store = true);

	int connect(Population *pSrc, Population *pDst, real weight, real delay, SpikeType type);
	int connect(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size);
	int connectOne2One(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size);
	int connectConv(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int height, int width, int k_height, int k_width);
	int connectPooling(Population *pSrc, Population *pDst, real weight, int height, int width, int p_height, int p_width);
	
	int connect(int populationIDSrc, int neuronIDSrc, int populationIDDst, int neuronIDDst, real weight, real delay, real tau = 0);
	Synapse* connect(Neuron *pSrc, Neuron *pDst, real weight, real delay, SpikeType type = Excitatory, real tau = 0, bool store = true);

	GNetwork* buildNetwork(const SimInfo &info);

	int addNeuronNum(Type type, int num);
	int addConnectionNum(Type type, int num);
	int addSynapseNum(Type type, int num);

	// int addMonitor(int populationIDSrc, int neuronIDSrc);
	// int addOutput(int populationIDSrc, int neuronIDSrc);
	// int addProbe(int populationIDSrc, int neuronIDSrc, double weight = 1);
	Population* findPopulation(int populationID);
	Neuron* findNeuron(int populationIDSrc, int neuronIDSrc);

	int reset(const SimInfo &info);
	// int update(const SimInfo &info);
	// void monitor(const SimInfo &info);
	
	void logMap();

	DistriNetwork * buildNetworks(const SimInfo &info, bool auto_splited = true);
	CrossNodeData* arrangeCrossNodeData(int node_num);
	CrossNodeDataGPU* arrangeCrossNodeDataGPU(int node_num);

private:
	void mapIDtoIdx(GNetwork *net);
	// bool checkIDtoIdx();
	
	void splitNetwork();
	void countTypeNum();
	GNetwork* arrangeData(int node, const SimInfo &info);
	Connection* arrangeConnect(int n_num, int s_num, int node_idx, const SimInfo &info);
	CrossNodeMap* arrangeCrossNodeMap(int n_num, int node_idx, int node_num);

public:
	// vector<Neuron*> pOutputs;
	vector<Population*> _pPopulations;
	vector<Synapse*> _pSynapses;
	// map<ID, vector<ID> > n2sNetwork;
	// map<ID, vector<ID> > n2sTargetNetwork;
	// map<ID, ID> s2nNetwork;
	// map<ID, ID> s2nForwardNetwork;
	// map<ID, Neuron*> id2neuron;
	// map<ID, Synapse*> id2synapse;
	// map<ID, int> nid2idx;
	// map<ID, int> sid2idx;
	// map<int, ID> idx2nid;
	// map<int, ID> idx2sid;

	// int maxDelaySteps;
	// int minDelaySteps;
	
	/** Cross Node Data **/
	// map<ID, int> _nID2node;
	// map<ID, int> _sID2node;
	// Neurons that on this node and would issue spikes to others.
	// Acessed by neurons = _crossnodeNeuronsSend[node]
	vector<set<Neuron *> > _crossnodeNeuronsSend;
	// Neurons that on other nodes and would issue spike to this node.
	// Accessed by neurons = _crossnodeNeuronsRecv[node]
	vector<set<Neuron *> > _crossnodeNeuronsRecv;
	// Get idx of shadow neuron on destination node, the idxs of shadow neurons are larger than that of real neurons.
	// Accessed by idx = _crossnodeNeuron2idx[node][neuron]
	vector<map<Neuron *, int> > _crossnodeNeuron2idx;

	/** Per Node Data **/
	// vector<map<int, ID> > _global_idx2nID;
	// vector<map<int, ID> > _global_idx2sID;
	// Number of neurons for different types on different nodes accessed by _global_ntype_num[node][type]
	vector<map<Type, int> >	_globalNTypeNum;
	// Number of synapses for different types on different nodes accessed by _global_ntype_num[node][type]
	vector<map<Type, int> > _globalSTypeNum;

	real _maxDelay;
	real _minDelay;
	int _totalNeuronNum;
	int _totalSynapseNum;
	int _nodeNum;
private:
	real _maxFireRate;
	vector<Type> _nTypes;
	vector<Type> _sTypes;
	int _populationNum;
	vector<int> _neuronNums;
	vector<int> _connectNums;
	vector<int> _synapseNums;
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

	if (find(_pPopulations.begin(), _pPopulations.end(), pp1) == _pPopulations.end()) {
		_pPopulations.push_back(pp1);
		_populationNum++;
		addNeuronNum(pp1->getType(), pp1->getNum());
	}

	return pp1;
}

template<class S>
int Network::connect(Population *pSrc, Population *pDst, S templ) {
	int srcNum = pSrc->getNum();
	int dstNum = pDst->getNum();

	if (find(_pPopulations.begin(), _pPopulations.end(), pSrc) == _pPopulations.end()) {
		_pPopulations.push_back(pSrc);
		_populationNum++;
		//neuronNum += pSrc->getNum();
		addNeuronNum(pSrc->getType(), pSrc->getNum());
	}
	if (find(_pPopulations.begin(), _pPopulations.end(), pDst) == _pPopulations.end()) {
		_pPopulations.push_back(pDst);
		_populationNum++;
		//neuronNum += pDst->getNum();
		addNeuronNum(pDst->getType(), pDst->getNum());
	}

	int count = 0;
	for (int iSrc=0; iSrc<srcNum; iSrc++) {
		for (int iDst =0; iDst<dstNum; iDst++) {
			connect(pSrc->locate(iSrc), pDst->locate(iDst), templ, false);
			count++;
		}
	}

	return count;
}

template<class S>
int Network::connect(Population *pSrc, Population *pDst, S *pTempl, int size) {
	int dstNum = pDst->getNum();
	assert(size == (pSrc->getNum() * dstNum)); 

	if (find(_pPopulations.begin(), _pPopulations.end(), pSrc) == _pPopulations.end()) {
		_pPopulations.push_back(pSrc);
		_populationNum++;
		//neuronNum += pSrc->getNum();
		addNeuronNum(pSrc->getType(), pSrc->getNum());
	}
	if (find(_pPopulations.begin(), _pPopulations.end(), pDst) == _pPopulations.end()) {
		_pPopulations.push_back(pDst);
		_populationNum++;
		//neuronNum += pDst->getNum();
		addNeuronNum(pDst->getType(), pDst->getNum());
	}

	int count = 0;
	for (int i=0; i<size; i++) {
		int iSrc = i/dstNum;
		int iDst = i%dstNum;
		connect(pSrc->locate(iSrc), pDst->locate(iDst), pTempl[i], false);
		count++;
	}

	return count;
}

template<class S>
Synapse * Network::connect(Neuron *pSrc, Neuron *pDst, S templ, bool store)
{
	Synapse * p = new S(templ);
	p->setSrc(pSrc);
	p->setDst(pDst);
	pSrc->addSynapse(p);

	_pSynapses.push_back(p);
	addConnectionNum(pSrc->getType(), 1);
	addSynapseNum(p->getType(), 1);


	real delay = p->getRealDelay();
	if (delay > _maxDelay) {
		_maxDelay = delay;
	}
	if (delay < _minDelay) {
		_minDelay = delay;
	}
	
	return p;
}

#endif /* NETWORK_H */

