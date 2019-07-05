/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <chrono>
#include <math.h>

#include "../utils/TypeFunc.h"
#include "Network.h"

using namespace std::chrono;

Network::Network(int nodeNum)
{
	_maxDelay = 0.0;
	_minDelay = 1.0e7;
	// maxDelaySteps = 0;
	// minDelaySteps = 1e7;
	_maxFireRate = 0.0;
	_populationNum = 0;
	_totalNeuronNum = 0;
	_totalSynapseNum = 0;

	_nodeNum = nodeNum;
	//n2sNetwork.clear();
	//
	_crossnodeNeuronsSend.resize(nodeNum);
	_crossnodeNeuronsRecv.resize(nodeNum);
	_crossnodeNeuron2idx.resize(nodeNum);

	_globalNTypeNum.resize(nodeNum);
	_globalSTypeNum.resize(nodeNum);
}

Network::~Network()
{
	if (!_pPopulations.empty()) {
		for (auto iter = _pPopulations.begin(); iter != _pPopulations.end(); iter++) {
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

	if (!_pSynapses.empty()) {
		for (auto iter = _pSynapses.begin(); iter != _pSynapses.end(); iter++) {
			Synapse * t = *iter;
			delete t;
		}
	}

	_pPopulations.clear();
	_pSynapses.clear();
	// _pOutputs.clear();
	// n2sNetwork.clear();
	// n2sTargetNetwork.clear();
	// s2nNetwork.clear();
	// s2nForwardNetwork.clear();
	// id2neuron.clear();
	// id2synapse.clear();
	// nid2idx.clear();
	// idx2nid.clear();
	// sid2idx.clear();
	// idx2sid.clear();
	_neuronNums.clear();
	_connectNums.clear();
	_synapseNums.clear();
	_nTypes.clear();
	_sTypes.clear();

	_crossnodeNeuronsSend.clear();
	_crossnodeNeuronsRecv.clear();
	_crossnodeNeuron2idx.clear();

	_globalNTypeNum.clear();
	_globalSTypeNum.clear();
}

int Network::setNodeNum(int nodeNum)
{
	_nodeNum = nodeNum;

	_crossnodeNeuronsSend.clear();
	_crossnodeNeuronsRecv.clear();
	_crossnodeNeuron2idx.clear();

	_globalNTypeNum.clear();
	_globalSTypeNum.clear();

	_crossnodeNeuronsSend.resize(nodeNum);
	_crossnodeNeuronsRecv.resize(nodeNum);
	_crossnodeNeuron2idx.resize(nodeNum);

	_globalNTypeNum.resize(nodeNum);
	_globalSTypeNum.resize(nodeNum);

	return _nodeNum;
}

int Network::connect(Population *pSrc, Population *pDst, real weight, real delay, SpikeType type) {
	int srcSize = pSrc->getNum();
	int dstNum = pDst->getNum();
	int size = srcSize * dstNum; 

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
		connect(pSrc->locate(iSrc), pDst->locate(iDst), weight, delay, type, 0.0, false);
		count++;
	}

	return count;
}

int Network::connect(Population *pSrc, Population *pDst, real *weight, real *delay, SpikeType *type, int size) {
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
	assert(size == pSrc->getNum());
	assert(size == pDst->getNum()); 

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
	assert(pSrc->getNum() == height * width); 
	assert(pDst->getNum() == height * width); 

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
	assert(pDst->getNum() == pSrc->getNum() / p_height / p_width); 

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
	vector<Type>::iterator iter = find(_nTypes.begin(), _nTypes.end(), type);
	if (iter == _nTypes.end()) {
		_nTypes.push_back(type);
		_neuronNums.push_back(num);
		_connectNums.push_back(0);
	} else {
		int idx = std::distance(_nTypes.begin(), iter);
		_neuronNums[idx] += num;
	}
	_totalNeuronNum += num;

	return num;
}

int Network::addConnectionNum(Type type, int num)
{
	// To be updated
	vector<Type>::iterator iter = find(_nTypes.begin(), _nTypes.end(), type);
	if (iter == _nTypes.end()) {
		//_nTypes.push_back(type);
		//_neuronNums.push_back(num);
		//_connectNums.push_back(0);
		printf("This should not happed, when a connect is added, a pre-neuron must exist!\n");
	} else {
		int idx = std::distance(_nTypes.begin(), iter);
		_connectNums[idx] += num;
	}

	return num;
}

int Network::addSynapseNum(Type type, int num)
{
	vector<Type>::iterator iter = find(_sTypes.begin(), _sTypes.end(), type);
	if (iter == _sTypes.end()) {
		_sTypes.push_back(type);
		_synapseNums.push_back(num);
	} else {
		int idx = std::distance(_sTypes.begin(), iter);
		_synapseNums[idx] += num;
	}
	_totalSynapseNum += num;
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

	_pSynapses.push_back(p);
	addConnectionNum(pn1->getType(), 1);
	addSynapseNum(p->getType(), 1);

	//n2sTargetNetwork[pn2->getID()].push_back(p->getID());
	//n2sNetwork[pn1->getID()].push_back(p->getID());
	//s2nNetwork[p->getID()] = pn2->getID(); 
	//s2nForwardNetwork[p->getID()] = pn1->getID();


	if (delay > _maxDelay) {
		_maxDelay = delay;
	}
	if (delay < _minDelay) {
		_minDelay = delay;
	}
	
	return p;
}

Population * Network::findPopulation(int populationID)
{
	if (populationID >= (int)_pPopulations.size()) {
		return NULL;
	}

	return _pPopulations[populationID];
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
	//for (iter = _pPopulations.begin(); iter != _pPopulations.end(); iter++) {
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

int Network::reset(const SimInfo &info)
{
	_crossnodeNeuronsSend.clear();
	_crossnodeNeuronsRecv.clear();
	_crossnodeNeuron2idx.clear();

	_globalNTypeNum.clear();
	_globalSTypeNum.clear();
	// maxDelaySteps = static_cast<int>(round(maxDelay/info.dt));
	// minDelaySteps = static_cast<int>(round(minDelay/info.dt));

	// vector<SynapseBase*>::iterator iterS;
	// vector<NeuronBase*>::iterator iterN;
	// vector<PopulationBase*>::iterator iterP;
	// for (iterS=_pSynapses.begin(); iterS!=_pSynapses.end(); iterS++) {
	// 	SynapseBase *p = *iterS;
	// 	p->reset(info);
	// }
	// //for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
	// //	NeuronBase * p = *iterN;
	// //	p->reset(info);
	// //}
	// for (iterP=_pPopulations.begin(); iterP!=_pPopulations.end(); iterP++) {
	// 	PopulationBase * p = *iterP;
	// 	p->reset(info);
	// }
	


	return 0;
}
// 
// int Network::update(const SimInfo &info)
// {
// 	vector<SynapseBase*>::iterator iterS;
// 	vector<NeuronBase*>::iterator iterN;
// 	vector<PopulationBase*>::iterator iterP;
// 
// 	for (iterP=_pPopulations.begin(); iterP!=_pPopulations.end(); iterP++) {
// 		PopulationBase * p = *iterP;
// 		p->update(info);
// 	}
// 
// 	//for (iterN=pNeurons.begin(); iterN!=pNeurons.end(); iterN++) {
// 	//	NeuronBase * p = *iterN;
// 	//	p->update(info);
// 	//}
// 
// 	for (iterS=_pSynapses.begin(); iterS!=_pSynapses.end(); iterS++) {
// 		SynapseBase *p = *iterS;
// 		p->update(info);
// 	}
// 
// 	return 0;
// }
// 
// void Network::monitor(const SimInfo &info)
// {
// 	vector<SynapseBase*>::iterator iterS;
// 	vector<NeuronBase*>::iterator iterN;
// 	vector<PopulationBase*>::iterator iterP;
// 
// 	for (iterP=_pPopulations.begin(); iterP!=_pPopulations.end(); iterP++) {
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
// 	for (iterS=_pSynapses.begin(); iterS!=_pSynapses.end(); iterS++) {
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

CrossNodeData* Network::arrangeCrossNodeData(int node_num)
{
	CrossNodeData * cross_data = (CrossNodeData*)malloc(sizeof(CrossNodeData) * node_num * node_num);
	assert(cross_data != NULL);

	for (int i=0; i<_nodeNum; i++) {
		for (int j=0; j<_nodeNum; j++) {
			// i->j 
			int i2j = j * _nodeNum + i;
			cross_data[i2j]._firedNNum = 0;

			int count = 0;
			for (auto iter = _crossnodeNeuronsSend[i].begin(); iter != _crossnodeNeuronsSend[i].end(); iter++) {
				if (_crossnodeNeuronsRecv[j].find(*iter) != _crossnodeNeuronsRecv[j].end()) {
					count++;
				}
			}
			cross_data[i2j]._maxNNum = count;
			cross_data[i2j]._firedNIdxs = (int*)malloc(sizeof(int)*count);
			assert(cross_data[i2j]._firedNIdxs != NULL || count == 0);
		}
	}


	for (int i=0; i<_nodeNum; i++) {
		int idx = i*_nodeNum + i;
		for (int j=0; j<_nodeNum; j++) {
			if (j != i) {
				cross_data[idx]._maxNNum += cross_data[i*_nodeNum+j]._maxNNum;
			}
		}

		cross_data[idx]._firedNIdxs = (int*)malloc(sizeof(int)*cross_data[idx]._maxNNum);
		assert(cross_data[idx]._firedNIdxs != NULL || cross_data[idx]._maxNNum == 0);
	}

	return cross_data;
}

void Network::countTypeNum() 
{
	for (auto pIter = _pPopulations.begin(); pIter != _pPopulations.end();  pIter++) {
		Population * p = *pIter;
		Type type = p->getType();
		int node = p->getNode();

		if (_globalNTypeNum[node].find(type) == _globalNTypeNum[node].end()) {
			_globalNTypeNum[node][type] = p->getNum();
		} else {
			_globalNTypeNum[node][type] += p->getNum();
		}
	}

	for (auto siter = _pSynapses.begin(); siter != _pSynapses.end();  siter++) {
		Synapse * p = *siter;
		Type type = p->getType();
		int node = p->getNode();

		if (_globalSTypeNum[node].find(type) == _globalSTypeNum[node].end()) {
			_globalSTypeNum[node][type] = 1;
		} else {
			_globalSTypeNum[node][type] += 1;
		}
	}
}

GNetwork* Network::arrangeData(int nodeIdx, const SimInfo &info) {
	int nTypeNum = _globalNTypeNum[nodeIdx].size();
	int sTypeNum = _globalSTypeNum[nodeIdx].size();

	GNetwork * net = allocNetwork(nTypeNum, sTypeNum);

	int maxDelaySteps = static_cast<int>(round(_maxDelay/info.dt));
	int minDelaySteps = static_cast<int>(round(_minDelay/info.dt));
	int delayLength = maxDelaySteps - minDelaySteps + 1;

	int index = 0;
	for (auto tIter = _globalNTypeNum[nodeIdx].begin(); tIter != _globalNTypeNum[nodeIdx].end(); tIter++) {
		Type type = tIter->first;
		net->pNTypes[index] = tIter->first;
		net->ppNeurons[index] = allocType[type](tIter->second);
		assert(net->ppNeurons[index] != NULL);

		int idx = 0;
		for (auto pIter = _pPopulations.begin(); pIter != _pPopulations.end();  pIter++) {
			Population * p = *pIter;
			int node = p->getNode();

			if (node == nodeIdx && p->getType() == type) {
				size_t copied = p->hardCopy(net->ppNeurons[index], idx, net->pNeuronNums[index], info);
				idx += copied;
			}
		}

		assert(idx == tIter->second);
		net->pNeuronNums[index+1] = idx + net->pNeuronNums[index];
		index++;
	}
	assert(index == nTypeNum);

	index = 0;
	for (auto tIter = _globalSTypeNum[nodeIdx].begin(); tIter != _globalSTypeNum[nodeIdx].end(); tIter++) {
		Type type = tIter->first;
		net->pSTypes[index] = type;
		net->ppSynapses[index] = allocType[type](tIter->second);
		assert(net->ppSynapses[index] != NULL);

		int idx = 0;
		for (auto pIter = _pPopulations.begin(); pIter != _pPopulations.end(); pIter++) {
			Population *pop = *pIter;
			if (pop->getNode() != nodeIdx)
				continue;

			for (int nidx=0; nidx<pop->getNum(); nidx++) {
				const vector<Synapse *> &s_vec = pop->locate(nidx)->getSynapses();
				for (int delay_t=0; delay_t<delayLength; delay_t++) {
					for (auto siter = s_vec.begin(); siter != s_vec.end(); siter++) {
						if ((*siter)->getDelaySteps(info.dt) == delay_t + minDelaySteps) {
							if ((*siter)->getType() == type && (*siter)->getNode() == nodeIdx) {
								assert(idx < tIter->second);
								int copied = (*siter)->hardCopy(net->ppSynapses[index], idx, net->pSynapseNums[index], info);
								idx += copied;
							}
						}
					}
				}
			}
		}

		for (auto niter = _crossnodeNeuronsRecv[nodeIdx].begin(); niter != _crossnodeNeuronsRecv[nodeIdx].end(); niter++) {
			const vector<Synapse *> &s_vec = (*niter)->getSynapses();
			for (int delay_t=0; delay_t<delayLength; delay_t++) {
				for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
					if (((*iter)->getNode() == nodeIdx) && ((*iter)->getDelaySteps(info.dt) == delay_t + minDelaySteps) && ((*iter)->getType() == type)) {
						assert(idx < tIter->second);
						int copied = (*iter)->hardCopy(net->ppSynapses[index], idx, net->pSynapseNums[index], info);
						idx += copied;
					}
				}
			}
		}

		assert(idx == tIter->second); 
		net->pSynapseNums[index+1] = idx + net->pSynapseNums[index];
		index++;
	}
	assert(index == sTypeNum);

	int nodeNNum = net->pNeuronNums[net->nTypeNum];

	for (auto iter = _crossnodeNeuronsRecv[nodeIdx].begin(); iter !=  _crossnodeNeuronsRecv[nodeIdx].end(); iter++) {
		int size = _crossnodeNeuron2idx[nodeIdx].size();
		_crossnodeNeuron2idx[nodeIdx][*iter] = nodeNNum + size;
	}

	return net;
}

Connection* Network::arrangeConnect(int nNum, int sNum, int node_idx, const SimInfo &info)
{
	int maxDelaySteps = static_cast<int>(round(_maxDelay/info.dt));
	int minDelaySteps = static_cast<int>(round(_minDelay/info.dt));
	Connection *connection = allocConnection(nNum, sNum, maxDelaySteps, minDelaySteps);
	assert(connection != NULL);

	int delayLength = maxDelaySteps - minDelaySteps + 1;
	int synapseIdx = 0;
	for (auto pIter = _pPopulations.begin(); pIter != _pPopulations.end(); pIter++) {
		Population * p = *pIter;
		if (p->getNode() != node_idx) 
			continue;

		for (int i=0; i<p->getNum(); i++) {
			ID nid = p->locate(i)->getID();
			const vector<Synapse *> &s_vec = p->locate(i)->getSynapses();
			for (int delay_t=0; delay_t<delayLength; delay_t++) {
				connection->pDelayStart[delay_t + delayLength*nid] = synapseIdx;

				for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
					if (((*iter)->getNode() == node_idx) && ((*iter)->getDelaySteps(info.dt) == delay_t + minDelaySteps)) {
						// int sid = (*iter)->getID();
						assert(synapseIdx < sNum);
						synapseIdx++;
					}
				}
				connection->pDelayNum[delay_t + delayLength*nid] = synapseIdx - connection->pDelayStart[delay_t + delayLength*nid];
			}
		}
	}

	for (auto niter = _crossnodeNeuronsRecv[node_idx].begin(); niter != _crossnodeNeuronsRecv[node_idx].end(); niter++) {
		int nid = _crossnodeNeuron2idx[node_idx][*niter];
		const vector<Synapse *> &s_vec = (*niter)->getSynapses();
		for (int delay_t=0; delay_t<delayLength; delay_t++) {
			connection->pDelayStart[delay_t + delayLength*nid] = synapseIdx;

			for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
				if (((*iter)->getNode() == node_idx) && ((*iter)->getDelaySteps(info.dt) == delay_t + minDelaySteps)) {
					// int sid = (*iter)->getID();
					assert(synapseIdx < sNum);
					synapseIdx++;
				}
			}
			connection->pDelayNum[delay_t + delayLength*nid] = synapseIdx - connection->pDelayStart[delay_t + delayLength*nid];
		}
	}

	return connection;
}

// Should finish data arrange of all nodes first.
CrossNodeMap* Network::arrangeCrossNodeMap(int n_num, int node_idx, int node_num)
{
	CrossNodeMap* crossMap = (CrossNodeMap*)malloc(sizeof(CrossNodeMap));
	assert(crossMap != NULL);

	crossMap->_idx2index = (int*)malloc(sizeof(int)*n_num);
	assert(crossMap->_idx2index != NULL);
	std::fill(crossMap->_idx2index, crossMap->_idx2index + n_num, -1);

	if (_crossnodeNeuronsSend[node_idx].size() > 0) {
		crossMap->_crossnodeIndex2idx = (int*)malloc(sizeof(int) * node_num * _crossnodeNeuronsSend[node_idx].size());
	} else {
		crossMap->_crossnodeIndex2idx = NULL;
	}
	crossMap->_crossSize = node_num * _crossnodeNeuronsSend[node_idx].size();



	int index = 0;
	for (auto iter = _crossnodeNeuronsSend[node_idx].begin(); iter != _crossnodeNeuronsSend[node_idx].end(); iter++) {
		int nidx = (*iter)->getID();
		crossMap->_idx2index[nidx] = index;
		for (int t=0; t<node_num; t++) {
			if (_crossnodeNeuronsRecv[t].find(*iter) != _crossnodeNeuronsRecv[t].end()) {
				assert(crossMap->_crossnodeIndex2idx != NULL);
				crossMap->_crossnodeIndex2idx[index*node_num + t] = _crossnodeNeuron2idx[t][*iter];
			} else {
				assert(crossMap->_crossnodeIndex2idx != NULL);
				crossMap->_crossnodeIndex2idx[index*node_num + t] = -1;
			}
		}
		index++;
	}

	return crossMap;
}

void Network::splitNetwork()
{
	map<Neuron *, vector<Synapse *>> n2sInput;
	for (auto sIter = _pSynapses.begin(); sIter != _pSynapses.end(); sIter++) {
		Synapse * p = *sIter;
		n2sInput[p->getDst()].push_back(p);
	}

	// Check n2s is right
	for (auto pIter = _pPopulations.begin(); pIter != _pPopulations.end(); pIter++) {
		Population * p = *pIter;
		for (int i=0; i<p->getNum(); i++) {
			Neuron * n = p->locate(i);
			auto n2sIter = n2sInput.find(n);
			if (n2sIter != n2sInput.end()) {
				for (auto vIter = n2sIter->second.begin(); vIter != n2sIter->second.end(); vIter++) {
					assert((*vIter)->getDst() == n);
				}
			}
		}
	}
	

	int nodeIdx = 0;
	int synapseCount = 0;
	int synapsePerNode = _totalSynapseNum/_nodeNum;
	for (auto pIter = _pPopulations.begin(); pIter != _pPopulations.end(); pIter++) {
		Population * p = *pIter;
		p->setNode(nodeIdx);
		for (int i=0; i<p->getNum(); i++) {
			p->locate(i)->setNode(nodeIdx);
			auto n2sIter = n2sInput.find(p->locate(i));
			if (n2sIter != n2sInput.end()) {
				synapseCount += n2sIter->second.size();
				for (auto vIter = n2sIter->second.begin(); vIter != n2sIter->second.end(); vIter++) {
					(*vIter)->setNode(nodeIdx);
				}
			}

		}
		if (synapseCount >= (nodeIdx+1) * synapsePerNode && nodeIdx < _nodeNum - 1) {
			nodeIdx++;	
		}
	}

	n2sInput.clear();

	for (auto pIter= _pPopulations.begin(); pIter != _pPopulations.end(); pIter++) {
		Population * p = *pIter;
		for (int i=0; i<p->getNum(); i++) {
			bool crossNoded = false;
			Neuron *n = p->locate(i);
			int n_node = p->locate(i)->getNode();
			const vector<Synapse *> &tmp = p->locate(i)->getSynapses();
			for (auto iter = tmp.begin(); iter != tmp.end(); iter++) {
				int s_node = (*iter)->getNode();
				if (s_node != n_node) {
					crossNoded = true;
					_crossnodeNeuronsRecv[s_node].insert(n);
				}
			}

			if (crossNoded) {
				_crossnodeNeuronsSend[n_node].insert(n);
			}
		}

	}

	return;
}

DistriNetwork* Network::buildNetworks(const SimInfo &info, bool auto_splited)
{
	assert(_nodeNum >= 1);
	DistriNetwork * net = initDistriNet(_nodeNum, info.dt);

	if (auto_splited && _nodeNum > 1) {
		splitNetwork();
	}

	countTypeNum();

	for (int nodeIdx =0; nodeIdx <_nodeNum; nodeIdx++) {
		net[nodeIdx]._network = arrangeData(nodeIdx, info);

		int nNum = net[nodeIdx]._network->pNeuronNums[net[nodeIdx]._network->nTypeNum] + _crossnodeNeuron2idx[nodeIdx].size();
		int sNum = net[nodeIdx]._network->pSynapseNums[net[nodeIdx]._network->sTypeNum];
		net[nodeIdx]._network->pConnection = arrangeConnect(nNum, sNum, nodeIdx, info);

	}

	for (int nodeIdx =0; nodeIdx <_nodeNum; nodeIdx++) {
		int nNum = net[nodeIdx]._network->pNeuronNums[net[nodeIdx]._network->nTypeNum] + _crossnodeNeuron2idx[nodeIdx].size();
		net[nodeIdx]._crossnodeMap = arrangeCrossNodeMap(nNum, nodeIdx, _nodeNum);
	}

	return net;
}
