
#include <assert.h>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "MultiNetwork.h"

//int MultiNetwork::addConnectionInfo(ID nID, int nid, int offset, int *delay_start, int *delay_num, int *s_idxs, int nodeIdx)
//{
//	int count = 0;
//	map<ID, vector<ID>>::iterator n2siter = network->n2sNetwork.find(nID);
//	if (n2siter == network->n2sNetwork.end()) {
//		for (int delay_t=0; delay_t < network->maxDelaySteps; delay_t++) {
//			delay_start[delay_t + network->maxDelaySteps*nid] = count + offset; 
//			delay_num[delay_t + network->maxDelaySteps*nid] = 0;
//		}
//
//		return count;
//	}
//
//	int synapsesNum_t = n2siter->second.size();
//	for (int delay_t=0; delay_t < network->maxDelaySteps; delay_t++) {
//		delay_start[delay_t + network->maxDelaySteps*nid] = offset + count;
//		for (int i=0; i<synapsesNum_t; i++) {
//			if (network->id2synapse[n2siter->second.at(i)]->getDelay() == delay_t+1) {
//				map<ID, int>::iterator sid2idxiter = network->sid2idx.find(n2siter->second.at(i));
//				assert(sid2idxiter != network->sid2idx.end());
//				if (sid2node[sid2idxiter->first] == nodeIdx) {
//					int sid = sid2idxiter->second;
//					s_idxs[offset + count] = sid;
//					count++;
//				}
//			}
//		}
//		delay_num[delay_t + network->maxDelaySteps*nid] = offset + count - delay_start[delay_t + network->maxDelaySteps*nid];
//	}
//
//	return count;
//}

void MultiNetwork::countTypeNum(bool auto_splited) 
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	for (piter = _network->pPopulations.begin(); piter != _network->pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		Type type = p->getType();
		int node = p->getNode();
		if (auto_splited) {
			node = _nID2node[p->getNeuron(0)->getID()];
		}

		if (_global_ntype_num[node].find(type) == _global_ntype_num[node].end()) {
			_global_ntype_num[node][type] = p->getNum();
		} else {
			_global_ntype_num[node][type] += p->getNum();
		}
	}

	for (niter = _network->pNeurons.begin(); niter != _network->pNeurons.end();  niter++) {
		NeuronBase * p = *niter;
		Type type = p->getType();
		int node = p->getNode();
		if (auto_splited) {
			node = _nID2node[(*niter)->getID()];
		}

		if (_global_ntype_num[node].find(type) == _global_ntype_num[node].end()) {
			_global_ntype_num[node][type] = 1;
		} else {
			_global_ntype_num[node][type] += 1;
		}
	}

	for (siter = _network->pSynapses.begin(); siter != _network->pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		Type type = p->getType();
		int node = p->getNode();
		if (auto_splited) {
			node = _sID2node[(*siter)->getID()];
		}

		if (_global_stype_num[node].find(type) == _global_stype_num[node].end()) {
			_global_stype_num[node][type] = 1;
		} else {
			_global_stype_num[node][type] += 1;
		}
	}
}


GNetwork* MultiNetwork::arrangeData(int node_idx, bool auto_splited) {
	int ntype_num = _global_ntype_num[node_idx].size();
	int stype_num = _global_stype_num[node_idx].size();

	GNetwork * net = initGNetwork(ntype_num, stype_num);

	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	int index = 0;
	for (map<Type, int>::const_iterator tmp_iter = _global_ntype_num[node_idx].begin(); tmp_iter != _global_ntype_num[node_idx].end(); tmp_iter++) {
		Type type = tmp_iter->first;
		net->nTypes[index] = tmp_iter->first;

		void *pN = createType[type]();
		assert(pN != NULL);
		allocType[type](pN, tmp_iter->second);

		int idx = 0;
		for (piter = _network->pPopulations.begin(); piter != _network->pPopulations.end();  piter++) {
			PopulationBase * p = *piter;
			int node = p->getNode();
			if (auto_splited) {
				node = _nID2node[p->getNeuron(0)->getID()];
			}

			if (node == node_idx && p->getType() == type) {
				size_t copied = p->hardCopy(pN, idx, net->neuronNums[index], _network->nid2idx, _global_idx2nID[node_idx]);
				idx += copied;
			}
		}
		for (niter = _network->pNeurons.begin(); niter != _network->pNeurons.end();  niter++) {
			NeuronBase * p = *niter;
			int node = p->getNode();
			if (auto_splited) {
				node = _nID2node[(*niter)->getID()];
			}
			if (node == node_idx && p->getType() == type) {
				size_t copied = p->hardCopy(pN, idx, net->neuronNums[index], _network->nid2idx, _global_idx2nID[node_idx]);
				idx += copied;
			}
		}

		assert(idx == tmp_iter->second);
		net->neuronNums[index+1] = idx + net->neuronNums[index];
		net->pNeurons[index] = pN;
		index++;
	}
	assert(index == ntype_num);

	index = 0;
	for (map<Type, int>::const_iterator tmp_iter = _global_stype_num[node_idx].begin(); tmp_iter != _global_stype_num[node_idx].end(); tmp_iter++) {
		Type type = tmp_iter->first;
		net->sTypes[index] = type;

		void *pS = createType[type]();
		assert(pS != NULL);
		allocType[type](pS, tmp_iter->second);

		int idx = 0;
		for (siter = _network->pSynapses.begin(); siter != _network->pSynapses.end();  siter++) {
			SynapseBase * p = *siter;
			int node = p->getNode();
			if (auto_splited) {
				node = _sID2node[(*siter)->getID()];
			}

			if (node == node_idx && p->getType() == type) {
				int copied = p->hardCopy(pS, idx, net->synapseNums[index], _network->sid2idx, _global_idx2sID[node_idx]);
				idx += copied;
			}
		}

		assert(idx == tmp_iter->second); 
		net->synapseNums[index+1] = idx + net->synapseNums[index];
		net->pSynapses[index] = pS;
		index++;
	}
	assert(index == stype_num);

	int node_s_num = net->synapseNums[_global_stype_num[node_idx].size()];
	for (int i=0; i<stype_num; i++) {
		int *dst = (int*)malloc(sizeof(int)*(net->synapseNums[i+1] - net->synapseNums[i]));
		assert(dst != NULL);

		for (int sidx = 0; sidx< node_s_num; sidx++) {
			assert(_global_idx2sID[node_idx].find(sidx) == _global_idx2sID[node_idx].end());
			ID sID = _global_idx2sID[node_idx][sidx];
			assert(_network->s2nNetwork.find(sID) != _network->s2nNetwork.end());
			ID nID = _network->s2nNetwork[sID];	
			map<ID, int>::iterator iter = _network->nid2idx.find(nID);
			assert(iter != _network->nid2idx.end());
			dst[sidx - net->synapseNums[i]] = iter->second;
		}

		addTypeConnection[net->sTypes[i]](net->pSynapses[i], dst);
	}

	int node_n_num = net->neuronNums[net->nTypeNum];

	for (map<ID, set<int> >::iterator tmp_iter = _crossnode_nID2nodes.begin(); tmp_iter != _crossnode_nID2nodes.end(); tmp_iter++) {	
		if (tmp_iter->second.find(node_idx) != tmp_iter->second.end()) {
			int size = _crossnode_nID2idx[node_idx].size();
			_crossnode_nID2idx[node_idx][tmp_iter->first] = node_n_num + size;
			_global_idx2nID[node_idx][node_n_num + size] = tmp_iter->first;

		}
	}
	assert(_global_idx2nID[node_idx].size() ==  node_n_num + crossnode_nID2idx.size());

	net->MAX_DELAY = _network->maxDelaySteps;

	return net;
}

N2SConnection* MultiNetwork::arrangeConnect(int n_num, int s_num, int node_idx)
{
	N2SConnection *connection = (N2SConnection*)malloc(sizeof(N2SConnection));
	assert(pAllConnections != NULL);

	int *delay_num = (int*)malloc(sizeof(int)*(_network->maxDelaySteps)*(n_num));
	assert(delay_num != NULL);
	int *delay_start = (int*)malloc(sizeof(int)*(_network->maxDelaySteps)*(n_num));
	assert(delay_start != NULL);
	int *s_idxs = (int*)malloc(sizeof(int)*s_num);
	assert(s_idxs != NULL);

	int synapseIdx = 0;
	for (int nid=0; nid<n_num; nid++) {
		map<int, ID>::iterator iter = _global_idx2nID[node_idx].find(nid);
		assert(iter != _global_idx2nID[node_idx].end());

		map<ID, vector<ID>>::iterator n2siter = _network->n2sNetwork.find(iter->second);
		if (n2siter == _network->n2sNetwork.end()) {
			for (int delay_t=0; delay_t < _network->maxDelaySteps; delay_t++) {
				delay_start[delay_t + _network->maxDelaySteps*nid] = synapseIdx;
				delay_num[delay_t + _network->maxDelaySteps*nid] = 0;
			}
			continue;
		}

		int s_num_t = n2siter->second.size();
		for (int delay_t=0; delay_t < _network->maxDelaySteps; delay_t++) {
			delay_start[delay_t + _network->maxDelaySteps*nid] = synapseIdx;
			for (int i=0; i<s_num_t; i++) {
				ID sID = n2siter->second.at(i);
				if (_network->id2synapse[sID]->getDelay() == delay_t+1) {
					map<ID, int>::iterator sid2idxiter = _network->sid2idx.find(sID);
					assert(_sID2node[sID] == node_idx && sid2idxiter == _network->sid2idx.end());

					if(sid2idxiter != _network->sid2idx.end() && _sID2node[sID] == node_idx) {
						int sid = sid2idxiter->second;
						assert(synapseIdx < s_num);
						s_idxs[synapseIdx] = sid;
						synapseIdx++;
					}
				}
			}
			delay_num[delay_t + _network->maxDelaySteps*nid] = synapseIdx - delay_start[delay_t + _network->maxDelaySteps*nid];
		}
	}

	connection->pSynapsesIdx = s_idxs;
	connection->delayStart = delay_start;
	connection->delayNum = delay_num;

	return connection;
}

DistriNetwork* MultiNetwork::buildNetworks(bool auto_splited)
{
	DistriNetwork * net = initDistriNet(_node_num);

	if (auto_splited) {
		splitNetwork();
	}

	countTypeNum(auto_splited);

	for (int node_idx =0; node_idx <_node_num; node_idx++) {
		net[node_idx]._network = arrangeData(node_idx, auto_splited);


		int n_num = _global_idx2nID[node_idx].size();
		int s_num = net[node_idx]._network->synapseNums[net[node_idx]._network->sTypeNum];
		net[node_idx]._network->pN2SConnection = arrangeConnect(n_num, s_num, node_idx);

	}

	_crossnode_idx2idx.resize(_node_num);
	for (int node_idx =0; node_idx <_node_num; node_idx++) {
		for (map<ID, set<int> >::const_iterator tmp_iter = _crossnode_nID2nodes.begin(); tmp_iter != _crossnode_nID2nodes.end(); tmp_iter++) {
			int node = _nID2node[tmp_iter->first];
			int idx = _network->nid2idx[tmp_iter->first];
			map<int, vector<int> >::const_iterator tmp_iter2 = _crossnode_idx2idx[node].find(idx);
			if (tmp_iter2 == _crossnode_idx2idx[node].end()) {
				_crossnode_idx2idx[node][idx].resize(_node_num, -1);
			}

			for (set<int>::const_iterator tmp_iter3 = tmp_iter->second.begin(); tmp_iter3 != tmp_iter->second.end(); tmp_iter3++) {
				_crossnode_idx2idx[node][idx][*tmp_iter3] = nodeNid2idx[*tmp_iter3][tmp_iter->first];
			}
		}

		crossNodeMap[node_idx].idx2index = (int*)malloc(sizeof(int)*nodeNeuronNum);
		assert(crossNodeMap[node_idx].idx2index != NULL);
		crossNodeMap[node_idx].crossNodeMap = (int*)malloc(sizeof(int)*_crossnode_idx2idx[node_idx].size()*_node_num);
		assert(crossNodeMap[node_idx].crossNodeMap != NULL);
		for (int tmp = 0; tmp < nodeNeuronNum; tmp++) {
			crossNodeMap[node_idx].idx2index[tmp] = -1;
		}

		int count_idx = 0;
		for (map<int, vector<int> >::const_iterator tmp_iter = _crossnode_idx2idx[node_idx].begin(); tmp_iter != _crossnode_idx2idx[node_idx].end(); tmp_iter++) {
			crossNodeMap[node_idx].idx2index[tmp_iter->first] = count_idx;
			for (int tmp =0; tmp<_node_num; tmp++) {
				crossNodeMap[node_idx].crossNodeMap[count_idx*_node_num + tmp] = tmp_iter->second[tmp];
			}
		}

		
		for (int tmp = 0; tmp < _node_num; tmp++) {
			// node_idx to tmp 
			int tmp_idx = tmp * _node_num + node_idx;
			crossNodeData[tmp_idx].maxNeuronNum = 0;
			for (map<int, vector<int> >::const_iterator tmp_iter = _crossnode_idx2idx[node_idx].begin(); tmp_iter != _crossnode_idx2idx[node_idx].end(); tmp_iter++) {
				if (tmp_iter->second[tmp] != -1) {
					crossNodeData[tmp_idx].maxNeuronNum++;
				}
			}
			if (crossNodeData[tmp_idx].maxNeuronNum > 0) {
				crossNodeData[tmp_idx].firedNeuronIdx = (int*)malloc(sizeof(int)*crossNodeData[tmp_idx].maxNeuronNum);
				assert(crossNodeData[tmp_idx].firedNeuronIdx != NULL);
			}
		}
	}

	for (int i=0; i<_node_num; i++) {
		int tmp_idx = i * _node_num + i;
		for (int j=0; j<_node_num; j++) {
			if (j != i) {
				crossNodeData[tmp_idx].maxNeuronNum += crossNodeData[i*_node_num + j].maxNeuronNum;
			}
		}

		if (crossNodeData[tmp_idx].maxNeuronNum > 0) {
			crossNodeData[tmp_idx].firedNeuronIdx = (int*)malloc(sizeof(int)*crossNodeData[tmp_idx].maxNeuronNum);
			assert(crossNodeData[tmp_idx].firedNeuronIdx != NULL);
		}
	}

	return ret;
}
