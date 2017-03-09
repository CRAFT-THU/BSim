#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "MultiNetwork.h"

void MultiNetwork::splitNetwork()
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;
	map<ID, vector<ID> >::iterator n2siter;

	int nodeIdx = 0;
	int synapseCount = 0;
	int synapsePerNode = _network->totalSynapseNum/_node_num;

	for (piter = _network->pPopulations.begin(); piter != _network->pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		for (int i=0; i<p->getNum(); i++) {
			ID nID = p->getNeuron(i)->getID();
			_nID2node[nID] = nodeIdx;
			n2siter = _network->n2sTargetNetwork.find(nID);
			if (n2siter != _network->n2sTargetNetwork.end()) {
				synapseCount += n2siter->second.size();
				for (vector<ID>::iterator viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
					_sID2node[*viter] = nodeIdx;
				}
			}

		}
		if (synapseCount >= nodeIdx * synapsePerNode && nodeIdx < _node_num) {
			nodeIdx++;	
		}
	}

	for (niter = _network->pNeurons.begin(); niter != _network->pNeurons.end();  niter++) {
		ID nID = (*niter)->getID();
		_nID2node[nID] = nodeIdx;
		n2siter = _network->n2sNetwork.find(nID);
		if (n2siter != _network->n2sTargetNetwork.end()) {
			synapseCount += n2siter->second.size();
			for (vector<ID>::iterator viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
				_sID2node[*viter] = nodeIdx;
			}
		}
		if (synapseCount >= nodeIdx * synapsePerNode && nodeIdx < _node_num) {
			nodeIdx++;	
		}
	}

	for (n2siter = _network->n2sNetwork.begin(); n2siter != _network->n2sNetwork.end(); n2siter++) {
		vector<ID> &p = n2siter->second;
		int synapseNum = p.size();
		int baseNode = _nID2node[n2siter->first];
		if (synapseNum > 0) {
			for (vector<ID>::iterator iter = p.begin(); iter != p.end(); iter++) {
				int snode = _sID2node[*iter];
				if (snode != baseNode) {
					_crossnode_nID2nodes[n2siter->first].insert(snode);
				}
			}
		}

	}

	return;
}
