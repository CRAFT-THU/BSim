#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "MultiNetwork.h"

void MultiNetwork::splitNetwork(int nodeNum)
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;
	map<ID, vector<ID> >::iterator n2siter;

	int nodeIdx = 0;
	int synapseCount = 0;
	int synapsePerNode = network->totalSynapseNum/nodeNum;

	for (piter = network->pPopulations.begin(); piter != network->pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		for (int i=0; i<p->getNum(); i++) {
			ID nid = p->getNeuron(i)->getID();
			nid2node[nid] = nodeIdx;
			n2siter = network->n2sNetwork.find(nid);
			if (n2siter != network->n2sNetwork.end()) {
				synapseCount += n2siter->second.size();
				for (vector<ID>::iterator viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
					sid2node[*viter] = nodeIdx;
				}
			}

		}
		//synapseCount += p->setNode(nodeNum);
		if (synapseCount >= nodeIdx * synapsePerNode && nodeIdx < nodeNum) {
			nodeIdx++;	
		}
	}

	for (niter = network->pNeurons.begin(); niter != network->pNeurons.end();  niter++) {
		ID nid = (*niter)->getID();
		nid2node[nid] = nodeIdx;
		n2siter = network->n2sNetwork.find(nid);
		if (n2siter != network->n2sNetwork.end()) {
			synapseCount += n2siter->second.size();
			for (vector<ID>::iterator viter = n2siter->second.begin(); viter != n2siter->second.end(); viter++) {
				sid2node[*viter] = nodeIdx;
			}
		}
		if (synapseCount >= nodeIdx * synapsePerNode && nodeIdx < nodeNum) {
			nodeIdx++;	
		}
	}

	//set<int> tmp_s;
	for (n2siter = network->n2sNetwork.begin(); n2siter != network->n2sNetwork.end(); n2siter++) {
		vector<ID> &p = n2siter->second;
		int synapseNum = p.size();
		int baseNode = nid2node[n2siter->first];
		if (synapseNum > 0) {
			//tmp_s.clear();
			for (vector<ID>::iterator iter = p.begin(); iter != p.end(); iter++) {
				int snode = sid2node[*iter];
				if (snode != baseNode) {
					//tmp_s.insert(iter->getNode());
					crossNodeInfo[n2siter->first].insert(snode);
				}
			}
			//if (tmp_s.size() > 0) {
			//	crossNodeInfo[n2siter->first] = tmp_s;
			//}
		}

	}

	return;
}
