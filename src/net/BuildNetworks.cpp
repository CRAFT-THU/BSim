
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

void MultiNetwork::countTypeNum() 
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	for (piter = _network->pPopulations.begin(); piter != _network->pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		Type type = p->getType();
		int node = p->getNode();

		if (_global_ntype_num[node].find(type) == _global_ntype_num[node].end()) {
			_global_ntype_num[node][type] = p->getNum();
		} else {
			_global_ntype_num[node][type] += p->getNum();
		}
	}

	for (siter = _network->pSynapses.begin(); siter != _network->pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		Type type = p->getType();
		int node = p->getNode();

		if (_global_stype_num[node].find(type) == _global_stype_num[node].end()) {
			_global_stype_num[node][type] = 1;
		} else {
			_global_stype_num[node][type] += 1;
		}
	}
}

GNetwork* MultiNetwork::arrangeData(int node_idx) {
	int ntype_num = _global_ntype_num[node_idx].size();
	int stype_num = _global_stype_num[node_idx].size();

	GNetwork * net = initGNetwork(ntype_num, stype_num);

	int index = 0;
	for (map<Type, int>::const_iterator tmp_iter = _global_ntype_num[node_idx].begin(); tmp_iter != _global_ntype_num[node_idx].end(); tmp_iter++) {
		Type type = tmp_iter->first;
		net->nTypes[index] = tmp_iter->first;

		void *pN = createType[type]();
		assert(pN != NULL);
		allocType[type](pN, tmp_iter->second);

		int idx = 0;
		for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end();  piter++) {
			PopulationBase * p = *piter;
			int node = p->getNode();

			if (node == node_idx && p->getType() == type) {
				size_t copied = p->hardCopy(pN, idx, net->neuronNums[index]);
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

		//for (siter = _network->pSynapses.begin(); siter != _network->pSynapses.end();  siter++) {
		//	SynapseBase * p = *siter;
		//	int node = p->getNode();

		//	if (node == node_idx && p->getType() == type) {
		//		int copied = p->hardCopy(pS, idx, net->synapseNums[index] );
		//		idx += copied;
		//	}
		//}
		
		int idx = 0;
		for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end(); piter++) {
			PopulationBase *pop = *piter;
			if (pop->getNode() != node_idx)
				continue;

			for (int nidx=0; nidx<pop->getNum(); nidx++) {
				const vector<SynapseBase*> &s_vec = pop->getNeuron(nidx)->getSynapses();
				for (int delay_t=0; delay_t < _network->maxDelaySteps; delay_t++) {
					for (auto siter = s_vec.begin(); siter != s_vec.end(); siter++) {
						if ((*siter)->getDelay() == delay_t + 1) {
							if ((*siter)->getType() == type && (*siter)->getNode() == node_idx) {
								assert(idx < tmp_iter->second);
								int copied = (*siter)->hardCopy(pS, idx, net->synapseNums[index]);
								idx += copied;
							}
						}
					}
				}
			}
		}

		for (auto niter = _crossnode_neurons_recv[node_idx].begin(); niter != _crossnode_neurons_recv[node_idx].end(); niter++) {
			const vector<SynapseBase*> &s_vec = (*niter)->getSynapses();
			for (int delay_t=0; delay_t < _network->maxDelaySteps; delay_t++) {
				for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
					if (((*iter)->getNode() == node_idx) && ((*iter)->getDelay() == delay_t + 1) && ((*iter)->getType() == type)) {
						assert(idx < tmp_iter->second);
						int copied = (*iter)->hardCopy(pS, idx, net->synapseNums[index]);
						idx += copied;
					}
				}
			}
		}


		assert(idx == tmp_iter->second); 
		net->synapseNums[index+1] = idx + net->synapseNums[index];
		net->pSynapses[index] = pS;
		index++;
	}
	assert(index == stype_num);

	int node_n_num = net->neuronNums[net->nTypeNum];

	for (auto iter = _crossnode_neurons_recv[node_idx].begin(); iter !=  _crossnode_neurons_recv[node_idx].end(); iter++) {
			int size = _crossnode_neuron2idx[node_idx].size();
			_crossnode_neuron2idx[node_idx][*iter] = node_n_num + size;
	}

	net->MAX_DELAY = _network->maxDelaySteps;

	return net;
}

N2SConnection* MultiNetwork::arrangeConnect(int n_num, int s_num, int node_idx)
{
	N2SConnection *connection = (N2SConnection*)malloc(sizeof(N2SConnection));
	assert(connection != NULL);

	connection->n_num = n_num;
	connection->s_num = s_num;

	int *delay_num = (int*)malloc(sizeof(int)*(_network->maxDelaySteps)*(n_num));
	assert(delay_num != NULL);
	int *delay_start = (int*)malloc(sizeof(int)*(_network->maxDelaySteps)*(n_num));
	assert(delay_start != NULL);
	int *s_idxs = (int*)malloc(sizeof(int)*s_num);
	assert(s_idxs != NULL);

	int synapseIdx = 0;
	for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		if (p->getNode() != node_idx) 
			continue;

		for (int i=0; i<p->getNum(); i++) {
			ID nid = p->getNeuron(i)->getID();
			const vector<SynapseBase*> &s_vec = p->getNeuron(i)->getSynapses();
			for (int delay_t=0; delay_t < _network->maxDelaySteps; delay_t++) {
				delay_start[delay_t + _network->maxDelaySteps*nid] = synapseIdx;

				for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
					if (((*iter)->getNode() == node_idx) && ((*iter)->getDelay() == delay_t + 1)) {
						int sid = (*iter)->getID();
						assert(synapseIdx < s_num);
					        s_idxs[synapseIdx] = sid;
						synapseIdx++;
					}
				}
				delay_num[delay_t + _network->maxDelaySteps*nid] = synapseIdx - delay_start[delay_t + _network->maxDelaySteps*nid];
			}
		}
	}

	for (auto niter = _crossnode_neurons_recv[node_idx].begin(); niter != _crossnode_neurons_recv[node_idx].end(); niter++) {
		int nid = _crossnode_neuron2idx[node_idx][*niter];
		const vector<SynapseBase*> &s_vec = (*niter)->getSynapses();
		for (int delay_t=0; delay_t < _network->maxDelaySteps; delay_t++) {
			delay_start[delay_t + _network->maxDelaySteps*nid] = synapseIdx;

			for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
				if (((*iter)->getNode() == node_idx) && ((*iter)->getDelay() == delay_t + 1)) {
					int sid = (*iter)->getID();
					assert(synapseIdx < s_num);
					s_idxs[synapseIdx] = sid;
					synapseIdx++;
				}
			}
			delay_num[delay_t + _network->maxDelaySteps*nid] = synapseIdx - delay_start[delay_t + _network->maxDelaySteps*nid];
		}
	}

	for (int i=0; i<s_num; i++) {
		assert(i==s_idxs[i]);
	}


	//TODO assert s_idx is in order
	//connection->pSynapsesIdx = s_idxs;
	free(s_idxs);
	connection->delayStart = delay_start;
	connection->delayNum = delay_num;

	return connection;
}

// Should finish data arrange of all nodes first.
CrossNodeMap* MultiNetwork::arrangeCrossNodeMap(int n_num, int node_idx, int node_num)
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

DistriNetwork* MultiNetwork::buildNetworks(bool auto_splited)
{
	DistriNetwork * net = initDistriNet(_node_num);

	if (auto_splited) {
		splitNetwork();
	}

	countTypeNum();

	for (int node_idx =0; node_idx <_node_num; node_idx++) {
		net[node_idx]._network = arrangeData(node_idx);

		int n_num = net[node_idx]._network->neuronNums[net[node_idx]._network->nTypeNum] + _crossnode_neuron2idx[node_idx].size();
		int s_num = net[node_idx]._network->synapseNums[net[node_idx]._network->sTypeNum];
		net[node_idx]._network->pN2SConnection = arrangeConnect(n_num, s_num, node_idx);

	}

	for (int node_idx =0; node_idx <_node_num; node_idx++) {
		int n_num = net[node_idx]._network->neuronNums[net[node_idx]._network->nTypeNum] + _crossnode_neuron2idx[node_idx].size();
		net[node_idx]._crossnode_map = arrangeCrossNodeMap(n_num, node_idx, _node_num);
	}

	return net;
}
