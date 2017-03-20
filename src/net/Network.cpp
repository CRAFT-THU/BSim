/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <chrono>

//#include "utils.h"
#include "Network.h"

using namespace std::chrono;

Network::Network()
{
	maxDelay = 0.0;
	maxDelaySteps = 0;
	maxFireRate = 0.0;
	populationNum = 0;
	totalNeuronNum = 0;
	totalSynapseNum = 0;

	n2sNetwork.clear();
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

	//if (!pNeurons.empty()) {
	//	vector<NeuronBase*>::iterator iter;
	//	for (iter = pNeurons.begin(); iter != pNeurons.end(); iter++) {
	//		NeuronBase * t = *iter;
	//		delete t;
	//	}
	//}

	if (!pSynapses.empty()) {
		vector<SynapseBase*>::iterator iter;
		for (iter = pSynapses.begin(); iter != pSynapses.end(); iter++) {
			SynapseBase * t = *iter;
			delete t;
		}
	}

	pPopulations.clear();
	//pNeurons.clear();
	pSynapses.clear();
	pOutputs.clear();
	n2sNetwork.clear();
	n2sTargetNetwork.clear();
	s2nNetwork.clear();
	s2nForwardNetwork.clear();
	id2neuron.clear();
	id2synapse.clear();
	nid2idx.clear();
	idx2nid.clear();
	sid2idx.clear();
	idx2sid.clear();
	neuronNums.clear();
	connectNums.clear();
	synapseNums.clear();
	nTypes.clear();
	sTypes.clear();
}

int Network::addNeuronNum(Type type, int num)
{
	vector<Type>::iterator iter = find(nTypes.begin(), nTypes.end(), type);
	if (iter == nTypes.end()) {
		nTypes.push_back(type);
		neuronNums.push_back(num);
		connectNums.push_back(0);
	} else {
		int idx = std::distance(nTypes.begin(), iter);
		neuronNums[idx] += num;
	}
	totalNeuronNum += num;

	return num;
}

int Network::addConnectionNum(Type type, int num)
{
	// To be updated
	vector<Type>::iterator iter = find(nTypes.begin(), nTypes.end(), type);
	if (iter == nTypes.end()) {
		//nTypes.push_back(type);
		//neuronNums.push_back(num);
		//connectNums.push_back(0);
		printf("This should not happed, when a connect is added, a pre-neuron must exist!");
	} else {
		int idx = std::distance(nTypes.begin(), iter);
		connectNums[idx] += num;
	}

	return num;
}

int Network::addSynapseNum(Type type, int num)
{
	vector<Type>::iterator iter = find(sTypes.begin(), sTypes.end(), type);
	if (iter == sTypes.end()) {
		sTypes.push_back(type);
		synapseNums.push_back(num);
	} else {
		int idx = std::distance(sTypes.begin(), iter);
		synapseNums[idx] += num;
	}
	totalSynapseNum += num;
	return num;
}

SynapseBase* Network::connect(NeuronBase *pn1, NeuronBase *pn2, real weight, real delay, SpikeType type, real tau, bool store)
{
	//if (store) {
	//	if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
	//		pNeurons.push_back(pn1);
	//		addNeuronNum(pn1->getType(), 1);
	//	}
	//	if (find(pNeurons.begin(), pNeurons.end(), pn2) == pNeurons.end()) {
	//		pNeurons.push_back(pn2);
	//		addNeuronNum(pn2->getType(), 1);
	//	}
	//}

	if (id2neuron.find(pn1->getID()) == id2neuron.end()) {
		id2neuron[pn1->getID()] = pn1;
	}
	if (id2neuron.find(pn2->getID()) == id2neuron.end()) {
		id2neuron[pn2->getID()] = pn2;
	}

	SynapseBase * p = pn2->createSynapse(weight, delay, type, tau, pn2);
	pn1->addSynapse(p);

	if (id2synapse.find(p->getID()) == id2synapse.end()) {
		id2synapse[p->getID()] = p;
	}

	pSynapses.push_back(p);
	addConnectionNum(pn1->getType(), 1);
	addSynapseNum(p->getType(), 1);

	n2sTargetNetwork[pn2->getID()].push_back(p->getID());
	n2sNetwork[pn1->getID()].push_back(p->getID());
	s2nNetwork[p->getID()] = pn2->getID(); 
	s2nForwardNetwork[p->getID()] = pn1->getID();


	if (delay > maxDelay) {
		maxDelay = delay;
	}
	
	return p;
}

PopulationBase* Network::findPopulation(int populationID)
{
	PopulationBase *pP= NULL;
	vector<PopulationBase*>::iterator iter;
	for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
		PopulationBase * t = *iter;
		if (t->getID().getId() == populationID) {
			pP = *iter;
		}
		if (pP != NULL) {
			break;
		}
	}

	//if (pP == NULL) {
	//	printf("Cann't find population: %d\n", populationID);
	//	return NULL;
	//}

	return pP;
}

NeuronBase* Network::findNeuron(int populationIDSrc, int neuronIDSrc)
{
	//PopulationBase *pP= NULL;
	//vector<PopulationBase*>::iterator iter;
	//for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
	//	PopulationBase * t = *iter;
	//	if (t->getID().id == populationIDSrc) {
	//		pP = *iter;
	//	}
	//	if (pP != NULL) {
	//		break;
	//	}
	//}
	
	PopulationBase *pP = findPopulation(populationIDSrc);
	if (pP == NULL) {
		//printf("Cann't find population: %d\n", populationIDSrc);
		return NULL;
	}

	NeuronBase *pN = NULL;
	pN = pP->findNeuron(ID(populationIDSrc, neuronIDSrc));
	if (pN == NULL) {
		//printf("Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return NULL;
	}

	return pN;
}

int Network::addOutput(int populationIDSrc, int neuronIDSrc)
{
	NeuronBase *pN = findNeuron(populationIDSrc, neuronIDSrc);
	if (pN == NULL) {
		printf("OUTPUT Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return -1;
	} else {
		pOutputs.push_back(pN);
	}

	return 0;
}

//int Network::addProbe(int populationIDSrc, int neuronIDSrc, double weight)
//{
//	NeuronBase *pN = findNeuron(populationIDSrc, neuronIDSrc);
//	if (pN == NULL) {
//		printf("PROBE NEURON Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
//		return -1;
//	}
//
//	NeuronBase *probe = findNeuron(-2, populationIDSrc);
//	if (probe == NULL) {
//		PopulationBase *pP = findPopulation(-2);
//		if (pP == NULL) {
//			pP = createPopulation(ID(0, -2), 1, ProbeNeuron(ID(0, 0), 0.01, 0.01));
//		}
//		Population<ProbeNeuron> *pNewP = (Population<ProbeNeuron>*)pP;
//		pNewP->addNeuron(ProbeNeuron(ID(-2, populationIDSrc), 0.01, 0.01));
//		probe = findNeuron(-2, populationIDSrc);
//		if (probe == NULL) {
//			printf("PROBE Cann't find neuron: %d:%d\n", -2, populationIDSrc);
//			return -1;
//		}
//		probe->monitorOn();
//	}
//	((ProbeNeuron*)probe)->addProbe(pN, weight);
//
//	return 0;
//}

int Network::addMonitor(int populationIDSrc, int neuronIDSrc)
{
	NeuronBase *pN = findNeuron(populationIDSrc, neuronIDSrc);
	if (pN == NULL) {
		printf("MONITOR Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return -1;
	} else {
		pN->monitorOn();
	}

	return 0;
}

int Network::connect(int populationIDSrc, int neuronIDSrc, int populationIDDst, int neuronIDDst, real weight, real delay, real tau)
{
	//PopulationBase *ppSrc = NULL, *ppDst = NULL;
	//vector<PopulationBase*>::iterator iter;
	//for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
	//	PopulationBase * t = *iter;
	//	if (t->getID().id == populationIDSrc) {
	//		ppSrc = *iter;
	//	}
	//	if (t->getID().id == populationIDDst) {
	//		ppDst = *iter;
	//	}

	//	if ((ppSrc != NULL) && (ppDst != NULL)) {
	//		break;
	//	}
	//}

	//if (ppSrc == NULL) {
	//	printf("Cann't find population: %d\n", populationIDSrc);
	//	return -1;
	//}

	//if (ppDst == NULL) {
	//	printf("Cann't find population: %d\n", populationIDDst);
	//	return -2;
	//}

	NeuronBase *pnSrc = NULL, *pnDst = NULL;
	pnSrc = findNeuron(populationIDSrc, neuronIDSrc);
	pnDst = findNeuron(populationIDDst, neuronIDDst);

	if (pnSrc == NULL) {
		printf("CONNECTION SRC Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return -1;
	}

	if (pnDst == NULL) {
		printf("CONNECTION DST Cann't find neuron: %d:%d\n", populationIDDst, neuronIDDst);
		return -2;
	}
	
	SpikeType type = Excitatory;
	if (delay < 0) {
		type = Inhibitory;
	}
	connect(pnSrc, pnDst, weight, delay, type, tau, false);

	return 0;
}

int Network::reset(SimInfo &info)
{
	maxDelaySteps = static_cast<int>(maxDelay/info.dt);
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;
	for (iterS=pSynapses.begin(); iterS!=pSynapses.end(); iterS++) {
		SynapseBase *p = *iterS;
		p->reset(info);
	}
	//for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
	//	NeuronBase * p = *iterN;
	//	p->reset(info);
	//}
	for (iterP=pPopulations.begin(); iterP!=pPopulations.end(); iterP++) {
		PopulationBase * p = *iterP;
		p->reset(info);
	}

	return 0;
}

int Network::update(SimInfo &info)
{
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;

	for (iterP=pPopulations.begin(); iterP!=pPopulations.end(); iterP++) {
		PopulationBase * p = *iterP;
		p->update(info);
	}

	//for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
	//	NeuronBase * p = *iterN;
	//	p->update(info);
	//}

	for (iterS=pSynapses.begin(); iterS!=pSynapses.end(); iterS++) {
		SynapseBase *p = *iterS;
		p->update(info);
	}

	return 0;
}

void Network::monitor(SimInfo &info)
{
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;

	for (iterP=pPopulations.begin(); iterP!=pPopulations.end(); iterP++) {
		PopulationBase * p = *iterP;
		//p->monitorOn();
		p->monitor(info);
	}

	//for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
	//	NeuronBase * p = *iterN;
	//	p->monitor(info);
	//}

	for (iterS=pSynapses.begin(); iterS!=pSynapses.end(); iterS++) {
		SynapseBase *p = *iterS;
		//p->monitorOn();
		p->monitor(info);
	}
}

void Network::logMap() {
	FILE *f = fopen("NID.map", "w+");
	for (map<int, ID>::const_iterator iter = idx2nid.begin(); iter != idx2nid.end(); iter++) {
		fprintf(f, "%d:%s\n", iter->first, iter->second.getInfo().c_str());
	}
}
