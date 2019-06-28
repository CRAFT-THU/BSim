/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <chrono>
#include <math.h>

//#include "utils.h"
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
	_crossnode_neurons_send.resize(nodeNum);
	_crossnode_neurons_recv.resize(nodeNum);
	_crossnode_neuron2idx.resize(nodeNum);

	_global_ntype_num.resize(nodeNum);
	_global_stype_num.resize(nodeNum);
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

	_crossnode_neurons_send.clear();
	_crossnode_neurons_recv.clear();
	_crossnode_neuron2idx.clear();

	_global_ntype_num.clear();
	_global_stype_num.clear();
}

int Network::connect(Population *pSrc, Population *pDst, real weight, real delay, SpikeType type) {
	int srcSize = pSrc->getNum();
	int dstSize = pDst->getNum();
	int size = srcSize * dstSize; 

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

int Network::reset(SimInfo &info)
{
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
// int Network::update(SimInfo &info)
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
// void Network::monitor(SimInfo &info)
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
			cross_data[i2j]._fired_n_num = 0;

			int count = 0;
			for (auto iter = _crossnode_neurons_send[i].begin(); iter != _crossnode_neurons_send[i].end(); iter++) {
				if (_crossnode_neurons_recv[j].find(*iter) != _crossnode_neurons_recv[j].end()) {
					count++;
				}
			}
			cross_data[i2j]._max_n_num = count;
			cross_data[i2j]._fired_n_idxs = (int*)malloc(sizeof(int)*count);
			assert(cross_data[i2j]._fired_n_idxs != NULL || count == 0);
		}
	}


	for (int i=0; i<_nodeNum; i++) {
		int idx = i*_nodeNum + i;
		for (int j=0; j<_nodeNum; j++) {
			if (j != i) {
				cross_data[idx]._max_n_num += cross_data[i*_nodeNum+j]._max_n_num;
			}
		}

		cross_data[idx]._fired_n_idxs = (int*)malloc(sizeof(int)*cross_data[idx]._max_n_num);
		assert(cross_data[idx]._fired_n_idxs != NULL || cross_data[idx]._max_n_num == 0);
	}

	return cross_data;
}

void Network::countTypeNum() 
{
	for (auto piter = _pPopulations.begin(); piter != _pPopulations.end();  piter++) {
		Population * p = *piter;
		Type type = p->getType();
		int node = p->getNode();

		if (_global_ntype_num[node].find(type) == _global_ntype_num[node].end()) {
			_global_ntype_num[node][type] = p->getNum();
		} else {
			_global_ntype_num[node][type] += p->getNum();
		}
	}

	for (auto siter = _pSynapses.begin(); siter != _pSynapses.end();  siter++) {
		Synapse * p = *siter;
		Type type = p->getType();
		int node = p->getNode();

		if (_global_stype_num[node].find(type) == _global_stype_num[node].end()) {
			_global_stype_num[node][type] = 1;
		} else {
			_global_stype_num[node][type] += 1;
		}
	}
}

GNetwork* Network::arrangeData(int node_idx, SimInfo &info) {
	int ntype_num = _global_ntype_num[node_idx].size();
	int stype_num = _global_stype_num[node_idx].size();

	GNetwork * net = allocNetwork(ntype_num, stype_num);

	int maxDelaySteps = static_cast<int>(round(_maxDelay/info.dt));
	int minDelaySteps = static_cast<int>(round(_minDelay/info.dt));
	int delayLength = maxDelaySteps - minDelaySteps + 1;

	int index = 0;
	for (map<Type, int>::const_iterator tmp_iter = _global_ntype_num[node_idx].begin(); tmp_iter != _global_ntype_num[node_idx].end(); tmp_iter++) {
		Type type = tmp_iter->first;
		net->pNTypes[index] = tmp_iter->first;

		net->ppNeurons[index] = allocType[type](tmp_iter->second);
		assert(net->ppNeurons[index] != NULL);

		int idx = 0;
		for (auto piter = _pPopulations.begin(); piter != _pPopulations.end();  piter++) {
			Population * p = *piter;
			int node = p->getNode();

			if (node == node_idx && p->getType() == type) {
				size_t copied = p->hardCopy(net->ppNeurons[index], idx, net->pNeuronNums[index], info);
				idx += copied;
			}
		}

		assert(idx == tmp_iter->second);
		net->pNeuronNums[index+1] = idx + net->pNeuronNums[index];
		index++;
	}
	assert(index == ntype_num);

	index = 0;
	for (map<Type, int>::const_iterator tmp_iter = _global_stype_num[node_idx].begin(); tmp_iter != _global_stype_num[node_idx].end(); tmp_iter++) {
		Type type = tmp_iter->first;
		net->pSTypes[index] = type;

		net->ppSynapses[index] = allocType[type](tmp_iter->second);
		assert(net->ppSynapses[index] != NULL);

		//for (siter = _network->_pSynapses.begin(); siter != _network->_pSynapses.end();  siter++) {
		//	SynapseBase * p = *siter;
		//	int node = p->getNode();

		//	if (node == node_idx && p->getType() == type) {
		//		int copied = p->hardCopy(pS, idx, net->pSynapseNums[index] );
		//		idx += copied;
		//	}
		//}

		int idx = 0;
		for (auto piter = _pPopulations.begin(); piter != _pPopulations.end(); piter++) {
			Population *pop = *piter;
			if (pop->getNode() != node_idx)
				continue;

			for (int nidx=0; nidx<pop->getNum(); nidx++) {
				const vector<Synapse *> &s_vec = pop->locate(nidx)->getSynapses();
				for (int delay_t=0; delay_t<delayLength; delay_t++) {
					for (auto siter = s_vec.begin(); siter != s_vec.end(); siter++) {
						if ((*siter)->getDelaySteps(info.dt) == delay_t + minDelaySteps) {
							if ((*siter)->getType() == type && (*siter)->getNode() == node_idx) {
								assert(idx < tmp_iter->second);
								int copied = (*siter)->hardCopy(net->ppSynapses[index], idx, net->pSynapseNums[index], info);
								idx += copied;
							}
						}
					}
				}
			}
		}

		for (auto niter = _crossnode_neurons_recv[node_idx].begin(); niter != _crossnode_neurons_recv[node_idx].end(); niter++) {
			const vector<Synapse *> &s_vec = (*niter)->getSynapses();
			for (int delay_t=0; delay_t<delayLength; delay_t++) {
				for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
					if (((*iter)->getNode() == node_idx) && ((*iter)->getDelaySteps(info.dt) == delay_t + minDelaySteps) && ((*iter)->getType() == type)) {
						assert(idx < tmp_iter->second);
						int copied = (*iter)->hardCopy(net->ppSynapses[index], idx, net->pSynapseNums[index], info);
						idx += copied;
					}
				}
			}
		}


		assert(idx == tmp_iter->second); 
		net->pSynapseNums[index+1] = idx + net->pSynapseNums[index];
		index++;
	}
	assert(index == stype_num);

	int node_n_num = net->pNeuronNums[net->nTypeNum];

	for (auto iter = _crossnode_neurons_recv[node_idx].begin(); iter !=  _crossnode_neurons_recv[node_idx].end(); iter++) {
		int size = _crossnode_neuron2idx[node_idx].size();
		_crossnode_neuron2idx[node_idx][*iter] = node_n_num + size;
	}

	// net->maxDelay = _network->maxDelaySteps;
	// net->minDelay = _network->minDelaySteps;

	return net;
}

Connection* Network::arrangeConnect(int n_num, int s_num, int node_idx, SimInfo &info)
{
	int maxDelaySteps = static_cast<int>(round(_maxDelay/info.dt));
	int minDelaySteps = static_cast<int>(round(_minDelay/info.dt));
	Connection *connection = allocConnection(n_num, s_num, maxDelaySteps, minDelaySteps);
	assert(connection != NULL);

	int delayLength = maxDelaySteps - minDelaySteps + 1;
	int synapseIdx = 0;
	for (auto piter = _pPopulations.begin(); piter != _pPopulations.end(); piter++) {
		Population * p = *piter;
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
						assert(synapseIdx < s_num);
						synapseIdx++;
					}
				}
				connection->pDelayNum[delay_t + delayLength*nid] = synapseIdx - connection->pDelayStart[delay_t + delayLength*nid];
			}
		}
	}

	for (auto niter = _crossnode_neurons_recv[node_idx].begin(); niter != _crossnode_neurons_recv[node_idx].end(); niter++) {
		int nid = _crossnode_neuron2idx[node_idx][*niter];
		const vector<Synapse *> &s_vec = (*niter)->getSynapses();
		for (int delay_t=0; delay_t<delayLength; delay_t++) {
			connection->pDelayStart[delay_t + delayLength*nid] = synapseIdx;

			for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
				if (((*iter)->getNode() == node_idx) && ((*iter)->getDelaySteps(info.dt) == delay_t + minDelaySteps)) {
					// int sid = (*iter)->getID();
					assert(synapseIdx < s_num);
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
	CrossNodeMap* cross_map = (CrossNodeMap*)malloc(sizeof(CrossNodeMap));
	assert(cross_map != NULL);

	cross_map->_idx2index = (int*)malloc(sizeof(int)*n_num);
	assert(cross_map->_idx2index != NULL);
	std::fill(cross_map->_idx2index, cross_map->_idx2index + n_num, -1);

	cross_map->_crossnode_index2idx = (int*)malloc(sizeof(int) * node_num * _crossnode_neurons_send[node_idx].size());
	cross_map->_cross_size = node_num * _crossnode_neurons_send[node_idx].size();

	assert(cross_map->_crossnode_index2idx != NULL);


	int index = 0;
	for (auto iter = _crossnode_neurons_send[node_idx].begin(); iter != _crossnode_neurons_send[node_idx].end(); iter++) {
		int nidx = (*iter)->getID();
		cross_map->_idx2index[nidx] = index;
		for (int t=0; t<node_num; t++) {
			if (_crossnode_neurons_recv[t].find(*iter) != _crossnode_neurons_recv[t].end()) {
				cross_map->_crossnode_index2idx[index*node_num + t] = _crossnode_neuron2idx[t][*iter];
			} else {
				cross_map->_crossnode_index2idx[index*node_num + t] = -1;
			}
		}
		index++;
	}

	return cross_map;
}

void Network::splitNetwork()
{
	int node_idx = 0;
	int synapse_count = 0;
	int synapse_per_node = _totalSynapseNum/_nodeNum;

	map<Neuron *, vector<Synapse *>> n2s_input;
	for (auto siter = _pSynapses.begin(); siter != _pSynapses.end(); siter++) {
		Synapse * p = *siter;
		n2s_input[p->getDst()].push_back(p);
	}

	for (auto piter = _pPopulations.begin(); piter != _pPopulations.end(); piter++) {
		Population * p = *piter;
		p->setNode(node_idx);
		for (int i=0; i<p->getNum(); i++) {
			p->locate(i)->setNode(node_idx);
			auto n2siter = n2s_input.find(p->locate(i));
			if (n2siter != n2s_input.end()) {
				synapse_count += n2siter->second.size();
				for (auto viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
					(*viter)->setNode(node_idx);
				}
			}

		}
		if (synapse_count >= (node_idx+1) * synapse_per_node && node_idx < _nodeNum - 1) {
			node_idx++;	
		}
	}

	n2s_input.clear();

	for (auto piter= _pPopulations.begin(); piter != _pPopulations.end(); piter++) {
		Population * p = *piter;
		for (int i=0; i<p->getNum(); i++) {
			bool cross_node = false;
			Neuron *n = p->locate(i);
			int n_node = p->locate(i)->getNode();
			const vector<Synapse *> &tmp = p->locate(i)->getSynapses();
			for (auto iter = tmp.begin(); iter != tmp.end(); iter++) {
				int s_node = (*iter)->getNode();
				if (s_node != n_node) {
					cross_node = true;
					_crossnode_neurons_recv[s_node].insert(n);
				}
			}

			if (cross_node) {
				_crossnode_neurons_send[n_node].insert(n);
			}
		}

	}

	return;
}

DistriNetwork* Network::buildNetworks(SimInfo &info, bool auto_splited)
{
	DistriNetwork * net = initDistriNet(_nodeNum);

	if (auto_splited) {
		splitNetwork();
	}

	countTypeNum();

	for (int node_idx =0; node_idx <_nodeNum; node_idx++) {
		net[node_idx]._network = arrangeData(node_idx, info);

		int n_num = net[node_idx]._network->pNeuronNums[net[node_idx]._network->nTypeNum] + _crossnode_neuron2idx[node_idx].size();
		int s_num = net[node_idx]._network->pSynapseNums[net[node_idx]._network->sTypeNum];
		net[node_idx]._network->pConnection = arrangeConnect(n_num, s_num, node_idx, info);

	}

	for (int node_idx =0; node_idx <_nodeNum; node_idx++) {
		int n_num = net[node_idx]._network->pNeuronNums[net[node_idx]._network->nTypeNum] + _crossnode_neuron2idx[node_idx].size();
		net[node_idx]._crossnode_map = arrangeCrossNodeMap(n_num, node_idx, _nodeNum);
	}

	return net;
}
