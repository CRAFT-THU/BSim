
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
//			if (network->id2synapse[n2siter->second.at(i)]->getDelaySteps(info.dt) == delay_t+1) {
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
	for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		Type type = p->getType();
		int node = p->getNode();

		if (_global_ntype_num[node].find(type) == _global_ntype_num[node].end()) {
			_global_ntype_num[node][type] = p->getNum();
		} else {
			_global_ntype_num[node][type] += p->getNum();
		}
	}

	for (auto siter = _network->pSynapses.begin(); siter != _network->pSynapses.end();  siter++) {
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

GNetwork* MultiNetwork::arrangeData(int node_idx, SimInfo &info) {
	int ntype_num = _global_ntype_num[node_idx].size();
	int stype_num = _global_stype_num[node_idx].size();

	GNetwork * net = allocNetwork(ntype_num, stype_num);

	int maxDelaySteps = static_cast<int>(round(_network->maxDelay/info.dt));
	int minDelaySteps = static_cast<int>(round(_network->minDelay/info.dt));
	int delayLength = maxDelaySteps - minDelaySteps + 1;

	int index = 0;
	for (map<Type, int>::const_iterator tmp_iter = _global_ntype_num[node_idx].begin(); tmp_iter != _global_ntype_num[node_idx].end(); tmp_iter++) {
		Type type = tmp_iter->first;
		net->pNTypes[index] = tmp_iter->first;

		net->ppNeurons[index] = allocType[type](tmp_iter->second);
		assert(net->ppNeurons[index] != NULL);

		int idx = 0;
		for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end();  piter++) {
			PopulationBase * p = *piter;
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

		//for (siter = _network->pSynapses.begin(); siter != _network->pSynapses.end();  siter++) {
		//	SynapseBase * p = *siter;
		//	int node = p->getNode();

		//	if (node == node_idx && p->getType() == type) {
		//		int copied = p->hardCopy(pS, idx, net->pSynapseNums[index] );
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
			const vector<SynapseBase*> &s_vec = (*niter)->getSynapses();
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

Connection* MultiNetwork::arrangeConnect(int n_num, int s_num, int node_idx, SimInfo &info)
{
	int maxDelaySteps = static_cast<int>(round(_network->maxDelay/info.dt));
	int minDelaySteps = static_cast<int>(round(_network->minDelay/info.dt));
	Connection *connection = allocConnection(n_num, s_num, maxDelaySteps, minDelaySteps);
	assert(connection != NULL);

	int delayLength = maxDelaySteps - minDelaySteps + 1;
	int synapseIdx = 0;
	for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		if (p->getNode() != node_idx) 
			continue;

		for (int i=0; i<p->getNum(); i++) {
			ID nid = p->getNeuron(i)->getID();
			const vector<SynapseBase*> &s_vec = p->getNeuron(i)->getSynapses();
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
		const vector<SynapseBase*> &s_vec = (*niter)->getSynapses();
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

DistriNetwork* MultiNetwork::buildNetworks(SimInfo &info, bool auto_splited)
{
	DistriNetwork * net = initDistriNet(_node_num);

	if (auto_splited) {
		splitNetwork();
	}

	countTypeNum();

	for (int node_idx =0; node_idx <_node_num; node_idx++) {
		net[node_idx]._network = arrangeData(node_idx, info);

		int n_num = net[node_idx]._network->pNeuronNums[net[node_idx]._network->nTypeNum] + _crossnode_neuron2idx[node_idx].size();
		int s_num = net[node_idx]._network->pSynapseNums[net[node_idx]._network->sTypeNum];
		net[node_idx]._network->pConnection = arrangeConnect(n_num, s_num, node_idx, info);

	}

	for (int node_idx =0; node_idx <_node_num; node_idx++) {
		int n_num = net[node_idx]._network->pNeuronNums[net[node_idx]._network->nTypeNum] + _crossnode_neuron2idx[node_idx].size();
		net[node_idx]._crossnode_map = arrangeCrossNodeMap(n_num, node_idx, _node_num);
	}

	return net;
}
