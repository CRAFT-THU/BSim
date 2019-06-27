/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <chrono>
#include <math.h>

//#include "utils.h"
#include "Network.h"

using namespace std::chrono;

Network::Network()
{
	maxDelay = 0.0;
	minDelay = 1.0e7;
	// maxDelaySteps = 0;
	// minDelaySteps = 1e7;
	maxFireRate = 0.0;
	populationNum = 0;
	totalNeuronNum = 0;
	totalSynapseNum = 0;

	//n2sNetwork.clear();
}

Network::~Network()
{
	if (!pPopulations.empty()) {
		vector<Population*>::iterator iter;
		for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
			Population* t = *iter;
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
		vector<Synapse*>::iterator iter;
		for (iter = pSynapses.begin(); iter != pSynapses.end(); iter++) {
			Synapse * t = *iter;
			delete t;
		}
	}

	pPopulations.clear();
	pSynapses.clear();
	pOutputs.clear();
	//n2sNetwork.clear();
	//n2sTargetNetwork.clear();
	//s2nNetwork.clear();
	//s2nForwardNetwork.clear();
	//id2neuron.clear();
	//id2synapse.clear();
	//nid2idx.clear();
	//idx2nid.clear();
	//sid2idx.clear();
	//idx2sid.clear();
	neuronNums.clear();
	connectNums.clear();
	synapseNums.clear();
	nTypes.clear();
	sTypes.clear();
}

int Network::connect(Population *pSrc, Population *pDst, real weight, real delay, SpikeType type) {
	int srcSize = pSrc->getNum();
	int dstSize = pDst->getNum();
	int size = srcSize * dstSize; 

	if (find(pPopulations.begin(), pPopulations.end(), pSrc) == pPopulations.end()) {
		pPopulations.push_back(pSrc);
		populationNum++;
		//neuronNum += pSrc->getNum();
		addNeuronNum(pSrc->getType(), pSrc->getNum());
	}
	if (find(pPopulations.begin(), pPopulations.end(), pDst) == pPopulations.end()) {
		pPopulations.push_back(pDst);
		populationNum++;
		//neuronNum += pDst->getNum();
		addNeuronNum(pDst->getType(), pDst->getNum());
	}

	int count = 0;
	for (int i=0; i<size; i++) {
		int iSrc = i/dstSize;
		int iDst = i%dstSize;
		connect(pSrc->locate(iSrc), pDst->locate(iDst), weight, delay, type, 0.0, false);
		count++;
	}

	return count;
}

int Network::connect(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size) {
	int srcSize = pSrc->getNum();
	int dstSize = pDst->getNum();
	assert(size == (srcSize * dstSize)); 

	if (find(pPopulations.begin(), pPopulations.end(), pSrc) == pPopulations.end()) {
		pPopulations.push_back(pSrc);
		populationNum++;
		//neuronNum += pSrc->getNum();
		addNeuronNum(pSrc->getType(), pSrc->getNum());
	}
	if (find(pPopulations.begin(), pPopulations.end(), pDst) == pPopulations.end()) {
		pPopulations.push_back(pDst);
		populationNum++;
		//neuronNum += pDst->getNum();
		addNeuronNum(pDst->getType(), pDst->getNum());
	}

	int count = 0;
	for (int i=0; i<size; i++) {
		int iSrc = i/dstSize;
		int iDst = i%dstSize;
		        //connect(Neuron *pn1, Neuron *pn2, real weight, real delay, SpikeType type, real tau, bool store)
		if (type == NULL) {
			connect(pSrc->locate(iSrc), pDst->locate(iDst), weight[i], delay[i], Excitatory, 0.0, false);
		} else {
			connect(pSrc->locate(iSrc), pDst->locate(iDst), weight[i], delay[i], type[i], 0.0, false);
		}
		count++;
	}

	return count;
}

int Network::connectOne2One(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size) {
	int srcSize = pSrc->getNum();
	int dstSize = pDst->getNum();
	assert(size == srcSize);
	assert(size == dstSize); 

	if (find(pPopulations.begin(), pPopulations.end(), pSrc) == pPopulations.end()) {
		pPopulations.push_back(pSrc);
		populationNum++;
		//neuronNum += pSrc->getNum();
		addNeuronNum(pSrc->getType(), pSrc->getNum());
	}
	if (find(pPopulations.begin(), pPopulations.end(), pDst) == pPopulations.end()) {
		pPopulations.push_back(pDst);
		populationNum++;
		//neuronNum += pDst->getNum();
		addNeuronNum(pDst->getType(), pDst->getNum());
	}

	int count = 0;
	for (int i=0; i<size; i++) {
		if (type == NULL) {
			connect(pSrc->locate(i), pDst->locate(i), weight[i], delay[i], Excitatory, 0.0, false);
		} else {
			connect(pSrc->locate(i), pDst->locate(i), weight[i], delay[i], type[i], 0.0, false);
		}
		count++;
	}

	return count;
}

int Network::connectConv(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int height, int width, int k_height, int k_width) {
	int srcSize = pSrc->getNum();
	int dstSize = pDst->getNum();
	assert(srcSize == height * width); 
	assert(dstSize == height * width); 

	int count = 0;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			for (int i = 0; i< k_height; i++) {
				for (int j = 0; j < k_width; j++) {
					int idx_h = h + i - (k_height - 1)/2;
					int idx_w = w + j - (k_width - 1)/2;

					if (idx_h >= 0 && idx_h < height && idx_w >= 0 && idx_w < width) {
						count++;
						if (type == NULL) {
							connect(pSrc->locate(idx_h * width + idx_w), pDst->locate(h * width + w), weight[i*k_width + j], delay[i*k_width + j], Excitatory, 0.0, false);

						} else {
							connect(pSrc->locate(idx_h * width + idx_w), pDst->locate(h * width + w), weight[i*k_width + j], delay[i*k_width + j], type[i*k_width + j], 0.0, false);
						}
					}
				}
			}
		}	
	}

	return count;
}

int Network::connectPooling(Population *pSrc, Population *pDst, real delay, int height, int width, int p_height, int p_width)
{
	int srcSize = pSrc->getNum();
	int dstSize = pDst->getNum();
	assert(dstSize == srcSize / p_height / p_width); 

	//int d_height = height/p_height;
	int d_width = width/p_width;

	int count = 0;
	for (int h = 0; h < height; h++) {
		for (int w = 0; w < width; w++) {
			int d_h = h/p_height;
			int d_w = w/p_width;
			int d_h_ = h % p_height;
			int d_w_ = w % p_width;
			int idx = d_h_ * p_width + d_w_;

			count++;
			connect(pSrc->locate(h * width + w), pDst->locate(d_h*d_width + d_w), (real)(1 << idx), delay, Excitatory, 0.0, false);
		}	
	}

	return count;
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
		printf("This should not happed, when a connect is added, a pre-neuron must exist!\n");
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

Synapse* Network::connect(Neuron *pn1, Neuron *pn2, real weight, real delay, SpikeType type, real tau, bool store)
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

	Synapse * p = pn2->createSynapse(weight, delay, type, tau, pn2);
	//p->setSrc(pn1);
	pn1->addSynapse(p);

	pSynapses.push_back(p);
	addConnectionNum(pn1->getType(), 1);
	addSynapseNum(p->getType(), 1);

	//n2sTargetNetwork[pn2->getID()].push_back(p->getID());
	//n2sNetwork[pn1->getID()].push_back(p->getID());
	//s2nNetwork[p->getID()] = pn2->getID(); 
	//s2nForwardNetwork[p->getID()] = pn1->getID();


	if (delay > maxDelay) {
		maxDelay = delay;
	}
	if (delay < minDelay) {
		minDelay = delay;
	}
	
	return p;
}

Population * Network::findPopulation(int populationID)
{
	vector<Population*>::iterator iter;
	if (populationID >= (int)pPopulations.size()) {
		return NULL;
	}

	return pPopulations[populationID];
}

Neuron * Network::findNeuron(int populationIDSrc, int neuronIDSrc)
{
	Population *pP = findPopulation(populationIDSrc);

	if (pP == NULL) {
		printf("Cann't find population: %d\n", populationIDSrc);
		return NULL;
	}

	Neuron *pN = NULL;
	pN = pP->locate(neuronIDSrc);
	if (pN == NULL) {
		printf("Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return NULL;
	}

	return pN;
}

// int Network::addOutput(int populationIDSrc, int neuronIDSrc)
// {
// 	Neuron *pN = findNeuron(populationIDSrc, neuronIDSrc);
// 	if (pN == NULL) {
// 		printf("OUTPUT Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
// 		return -1;
// 	} else {
// 		pOutputs.push_back(pN);
// 	}
// 
// 	return 0;
// }

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

// int Network::addMonitor(int populationIDSrc, int neuronIDSrc)
// {
// 	Neuron *pN = findNeuron(populationIDSrc, neuronIDSrc);
// 	if (pN == NULL) {
// 		printf("MONITOR Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
// 		return -1;
// 	} else {
// 		pN->monitorOn();
// 	}
// 
// 	return 0;
// }

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

	Neuron *pnSrc = NULL, *pnDst = NULL;
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
	if (weight < 0.0) {
		type = Inhibitory;
	}
	connect(pnSrc, pnDst, weight, delay, type, tau, false);

	return 0;
}

int Network::reset(SimInfo &info)
{
	// maxDelaySteps = static_cast<int>(round(maxDelay/info.dt));
	// minDelaySteps = static_cast<int>(round(minDelay/info.dt));

	// vector<SynapseBase*>::iterator iterS;
	// vector<NeuronBase*>::iterator iterN;
	// vector<PopulationBase*>::iterator iterP;
	// for (iterS=pSynapses.begin(); iterS!=pSynapses.end(); iterS++) {
	// 	SynapseBase *p = *iterS;
	// 	p->reset(info);
	// }
	// //for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
	// //	NeuronBase * p = *iterN;
	// //	p->reset(info);
	// //}
	// for (iterP=pPopulations.begin(); iterP!=pPopulations.end(); iterP++) {
	// 	PopulationBase * p = *iterP;
	// 	p->reset(info);
	// }

	return 0;
}
// 
// int Network::update(SimInfo &info)
// {
// 	vector<SynapseBase*>::iterator iterS;
// 	vector<NeuronBase*>::iterator iterN;
// 	vector<PopulationBase*>::iterator iterP;
// 
// 	for (iterP=pPopulations.begin(); iterP!=pPopulations.end(); iterP++) {
// 		PopulationBase * p = *iterP;
// 		p->update(info);
// 	}
// 
// 	//for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
// 	//	NeuronBase * p = *iterN;
// 	//	p->update(info);
// 	//}
// 
// 	for (iterS=pSynapses.begin(); iterS!=pSynapses.end(); iterS++) {
// 		SynapseBase *p = *iterS;
// 		p->update(info);
// 	}
// 
// 	return 0;
// }
// 
// void Network::monitor(SimInfo &info)
// {
// 	vector<SynapseBase*>::iterator iterS;
// 	vector<NeuronBase*>::iterator iterN;
// 	vector<PopulationBase*>::iterator iterP;
// 
// 	for (iterP=pPopulations.begin(); iterP!=pPopulations.end(); iterP++) {
// 		PopulationBase * p = *iterP;
// 		//p->monitorOn();
// 		p->monitor(info);
// 	}
// 
// 	//for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
// 	//	NeuronBase * p = *iterN;
// 	//	p->monitor(info);
// 	//}
// 
// 	for (iterS=pSynapses.begin(); iterS!=pSynapses.end(); iterS++) {
// 		SynapseBase *p = *iterS;
// 		//p->monitorOn();
// 		p->monitor(info);
// 	}
// }

void Network::logMap() {
	//FILE *f = fopen("NID.map", "w+");
	//for (map<int, ID>::const_iterator iter = idx2nid.begin(); iter != idx2nid.end(); iter++) {
	//	fprintf(f, "%d:%s\n", iter->first, iter->second.getInfo().c_str());
	//}
}
