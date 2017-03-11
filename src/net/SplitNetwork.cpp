#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "MultiNetwork.h"

void MultiNetwork::splitNetwork()
{
	//vector<PopulationBase*>::iterator piter;
	//vector<NeuronBase*>::iterator niter;
	//vector<SynapseBase*>::iterator siter;
	//map<ID, vector<ID> >::iterator n2siter;

	int node_idx = 0;
	int synapse_count = 0;
	int synapse_per_node = _network->totalSynapseNum/_node_num;

	for (auto piter = _network->pPopulations.begin(); piter != _network->pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		for (int i=0; i<p->getNum(); i++) {
			ID nID = p->getNeuron(i)->getID();
			_nID2node[nID] = node_idx;
			auto n2siter = _network->n2sTargetNetwork.find(nID);
			if (n2siter != _network->n2sTargetNetwork.end()) {
				synapse_count += n2siter->second.size();
				for (vector<ID>::iterator viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
					_sID2node[*viter] = node_idx;
				}
			}

		}
		if (synapse_count >= node_idx * synapse_per_node && node_idx < _node_num) {
			node_idx++;	
		}
	}

	for (auto niter = _network->pNeurons.begin(); niter != _network->pNeurons.end();  niter++) {
		ID nID = (*niter)->getID();
		_nID2node[nID] = node_idx;
		auto n2siter = _network->n2sTargetNetwork.find(nID);
		if (n2siter != _network->n2sTargetNetwork.end()) {
			synapse_count += n2siter->second.size();
			for (vector<ID>::iterator viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
				_sID2node[*viter] = node_idx;
			}
		}
		if (synapse_count >= node_idx * synapse_per_node && node_idx < _node_num) {
			node_idx++;	
		}
	}

	for (auto n2siter = _network->n2sNetwork.begin(); n2siter != _network->n2sNetwork.end(); n2siter++) {
		vector<ID> &p = n2siter->second;
		int synapse_num = p.size();
		int nnode = _nID2node[n2siter->first];
		if (synapse_num > 0) {
			bool cross_node = false;
			for (vector<ID>::iterator iter = p.begin(); iter != p.end(); iter++) {
				int snode = _sID2node[*iter];
				if (snode != nnode) {
					cross_node = true;
					_crossnode_IDs_receive[snode].insert(n2siter->first);
				}
			}
			if (cross_node) {
				_crossnode_IDs_send[nnode].insert(n2siter->first);
			}
		}

	}

	//Double Check
	map<ID, set<int> > _crossnode_nID2nodes;
	for (auto n2siter = _network->n2sNetwork.begin(); n2siter != _network->n2sNetwork.end(); n2siter++) {
		vector<ID> &p = n2siter->second;
		int synapse_num = p.size();
		int baseNode = _nID2node[n2siter->first];
		if (synapse_num > 0) {
			for (vector<ID>::iterator iter = p.begin(); iter != p.end(); iter++) {
				int snode = _sID2node[*iter];
				if (snode != baseNode) {
					_crossnode_nID2nodes[n2siter->first].insert(snode);
				}
			}
		}

	}

	for (auto iter = _crossnode_nID2nodes.begin(); iter != _crossnode_nID2nodes.end(); iter++) {
		int nnode = _nID2node[iter->first];
		assert(_crossnode_IDs_send[nnode].find(iter->first) != _crossnode_IDs_send[nnode].end());
		for (auto iter2 = iter->second.begin(); iter2 != iter->second.end(); iter2++) {
			int snode = _sID2node[*iter2];
			assert(_crossnode_IDs_receive[snode].find(iter->first) != _crossnode_IDs_receive[snode].end());
		}
	}

	return;
}
