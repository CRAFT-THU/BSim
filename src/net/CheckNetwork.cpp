
#include "Network.h"

bool Network::checkIDtoIdx()
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		for (int i=0; i< p->getNum(); i++) {
			map<ID, int>::const_iterator iter = nid2idx.find(p->getNeuron(i)->getID());
			if ((int)p->getNeuron(i)->getIdx() != iter->second) {
				printf("Not match: neuron %s and index %d should be %d\n", p->getNeuron(i)->getID().getInfo().c_str(), iter->second, (int)p->getNeuron(i)->getIdx());
				return false;
			}
			map<int, ID>::const_iterator reti  = idx2nid.find(p->getNeuron(i)->getIdx());
			if (reti->second != (p->getNeuron(i)->getID())) {
				printf("Not match: index %d and neuron %s should be %s\n", (int)p->getNeuron(i)->getIdx(), reti->second.getInfo().c_str(), p->getNeuron(i)->getID().getInfo().c_str());
				return false;
			}
		}
	}

	//for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
	//	NeuronBase * p = *niter;
	//	if (p->getIdx() != (size_t)nid2idx[p->getID()]) {
	//		printf("Not match: single neuron %s and index %d\n", p->getID().getInfo().c_str(), (int)p->getIdx());
	//	}
	//	if (idx2nid[(int)p->getIdx()] != (p->getID())) {
	//		printf("Not match: index %d and single neuron %s\n", (int)p->getIdx(), p->getID().getInfo().c_str());
	//	}
	//}

	for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		if (p->getIdx() != (size_t)sid2idx[p->getID()]) {
			printf("Not match: synapse %s and index %d\n", p->getID().getInfo().c_str(), (int)p->getIdx());
			return false;
		}
		if (idx2sid[(int)p->getIdx()] != (p->getID())) {
			printf("Not match: index %d and synapse %s\n", (int)p->getIdx(), p->getID().getInfo().c_str());
			return false;
		}
	}

	return true;
}
