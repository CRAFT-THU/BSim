
#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "Network.h"

GNetwork* Network::buildNetworks(int nodeNum, bool autoSplited)
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	vector<map<Type, int> >	globalNTypeInfo(nodeNum);
	vector<map<Type, int> > globalSTypeInfo(nodeNum);

	//Get Type info
	for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
		PopulationBase * p = static_cast<PopulationBase*>(*piter);
		Type type = p->getType();
		int node = p->getID().getNode();
		if (autoSplited) {
			node = nid2node[p->getNeuron(0)->getID()];
		}

		if (globalNTypeInfo[node].find(type) == globalNTypeInfo[node].end()) {
			globalNTypeInfo[node][type] = 0;
		} else {
			globalNTypeInfo[node][type] += p->getNum();
		}
	}
	for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
		NeuronBase * p = *niter;
		Type type = p->getType();
		int node = p->getID().getNode();
		if (autoSplited) {
			node = nid2node[(*niter)->getID()];
		}

		if (globalNTypeInfo[node].find(type) == globalNTypeInfo[node].end()) {
			globalNTypeInfo[node][type] = 0;
		} else {
			globalNTypeInfo[node][type] += 1;
		}
	}
	for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		Type type = p->getType();
		int node = p->getID().getNode();
		if (autoSplited) {
			node = sid2node[(*siter)->getID()];
		}

		if (globalSTypeInfo[node].find(type) == globalSTypeInfo[node].end()) {
			globalSTypeInfo[node][type] = 0;
		} else {
			globalSTypeInfo[node][type] += 1;
		}
	}


	GNetwork *ret = (GNetwork*)malloc(sizeof(GNetwork)*nodeNum);
	if (ret == NULL) {
		printf("Malloc GNetwork failed/n");
		return NULL;
	}

	for (int nodeIdx = 0; nodeIdx < nodeNum; nodeIdx++) {
		int nodeNeuronTypeNum = globalNTypeInfo[nodeIdx].size();
		int nodeSynapseTypeNum = globalSTypeInfo[nodeIdx].size();
		void **pAllNeurons = (void**)malloc(sizeof(void*)*nodeNeuronTypeNum);
		void **pAllSynapses = (void**)malloc(sizeof(void*)*nodeSynapseTypeNum);
		N2SConnection *pAllConnections = (N2SConnection*)malloc(sizeof(N2SConnection));

		int *pNeuronsNum = (int*)malloc(sizeof(int)*(nodeNeuronTypeNum + 1));
		int *pSynapsesNum = (int*)malloc(sizeof(int)*(nodeSynapseTypeNum + 1));
		pNeuronsNum[0] = 0;
		pSynapsesNum[0] = 0;

		Type *pNTypes = (Type*)malloc(sizeof(Type)*nodeNeuronTypeNum);
		Type *pSTypes = (Type*)malloc(sizeof(Type)*nodeSynapseTypeNum);

		map<int, ID> nodeIdx2nid;
		map<int, ID> nodeIdx2sid;

		int index = 0;
		for (map<Type, int>::const_iterator tmp_iter = globalNTypeInfo[nodeIdx].begin(); tmp_iter != globalNTypeInfo[nodeIdx].end(); tmp_iter++) {
			Type type = tmp_iter->first;
			pNTypes[index] = tmp_iter->first;

			void *pN = createType[type]();
			allocType[type](pN, tmp_iter->second);

			int idx = 0;
			for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
				PopulationBase * p = *piter;
				int node = p->getID().getNode();
				if (autoSplited) {
					node = nid2node[p->getNeuron(0)->getID()];
				}

				if (node == nodeIdx && p->getType() == type) {
					size_t copied = p->hardCopy(pN, idx, pNeuronsNum[index], nid2idx, nodeIdx2nid);
					idx += copied;
				}
			}
			for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
				NeuronBase * p = *niter;
				int node = p->getID().getNode();
				if (autoSplited) {
					node = nid2node[(*niter)->getID()];
				}
				if (node == nodeIdx && p->getType() == type) {
					size_t copied = p->hardCopy(pN, idx, pNeuronsNum[index], nid2idx, nodeIdx2nid);
					idx += copied;
				}
			}

			pNeuronsNum[index+1] = idx + pNeuronsNum[index];
			pAllNeurons[index] = pN;
			index++;
		}
		
		globalIdx2nid.push_back(nodeIdx2nid);

		index = 0;
		for (map<Type, int>::const_iterator tmp_iter = globalSTypeInfo[nodeIdx].begin(); tmp_iter != globalSTypeInfo[nodeIdx].end(); tmp_iter++) {
			Type type = tmp_iter->first;
			pSTypes[index] = type;

			void *pS = createType[type]();
			allocType[type](pS, synapseNums[index]);

			int idx = 0;
			for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
				SynapseBase * p = *siter;
				int node = p->getID().getNode();
				if (autoSplited) {
					node = sid2node[(*siter)->getID()];
				}

				if (node == nodeIdx && p->getType() == type) {
					int copied = p->hardCopy(pS, idx, pSynapsesNum[index], sid2idx, nodeIdx2sid);
					idx += copied;
				}
			}

			pSynapsesNum[index+1] = idx + pSynapsesNum[index];
			pAllSynapses[index] = pS;
			index++;
		}
		globalIdx2sid.push_back(nodeIdx2sid);

		int nodeNeuronNum = pNeuronsNum[nodeNeuronTypeNum];
		int nodeSynapseNum = pNeuronsNum[nodeSynapseTypeNum];

		int *delayNum = (int*)malloc(sizeof(int)*(this->maxDelaySteps)*nodeNeuronNum);
		int *delayStart = (int*)malloc(sizeof(int)*(this->maxDelaySteps)*nodeNeuronNum);
		int *pSynapsesIdx = (int*)malloc(sizeof(int)*nodeSynapseNum);

		int synapseIdx = 0;
		for (int nid=0; nid<nodeNeuronNum; nid++) {
			map<int, ID>::iterator iter = nodeIdx2nid.find(nid);
			if (iter == nodeIdx2nid.end()) {
				printf("Can't find neuron index %d\n", nid);
			}
			map<ID, vector<ID>>::iterator n2siter = n2sNetwork.find(iter->second);
			if (n2siter == n2sNetwork.end()) {
				pSynapsesIdx[synapseIdx] = -1;

				for (int delay_t=0; delay_t < maxDelaySteps; delay_t++) {

					delayStart[delay_t + maxDelaySteps*nid] = synapseIdx;
					delayNum[delay_t + maxDelaySteps*nid] = 0;
				}
				continue;
			}

			int synapsesNum_t = n2siter->second.size();
			for (int delay_t=0; delay_t < maxDelaySteps; delay_t++) {
				delayStart[delay_t + maxDelaySteps*nid] = synapseIdx;
				for (int i=0; i<synapsesNum_t; i++) {
					if (id2synapse[n2siter->second.at(i)]->getDelay() == delay_t+1) {
						map<ID, int>::iterator sid2idxiter = sid2idx.find(n2siter->second.at(i));
						if (sid2idxiter == sid2idx.end()) {
							printf("Can't find synapse ID %s\n", n2siter->second.at(i).getInfo().c_str());
						}

						int sid = sid2idxiter->second;
						pSynapsesIdx[synapseIdx] = sid;
						synapseIdx++;
					}
				}
				delayNum[delay_t + maxDelaySteps*nid] = synapseIdx - delayStart[delay_t + maxDelaySteps*nid];
			}

		}

		pAllConnections->pSynapsesIdx = pSynapsesIdx;
		pAllConnections->delayStart = delayStart;
		pAllConnections->delayNum = delayNum;

		for (int i=0; i<nodeSynapseTypeNum; i++) {
			int *pSynapsesDst = (int*)malloc(sizeof(int)*synapseNums[i]);
			map<ID, ID>::iterator s2nIter;
			for (s2nIter = s2nNetwork.begin(); s2nIter != s2nNetwork.end(); s2nIter++) {
				map<ID, int>::iterator iter = sid2idx.find(s2nIter->first);
				if (iter == sid2idx.end()) {
					printf("Can't find ID %s\n", s2nIter->first.getInfo().c_str());
				}
				if (i != getType(pSynapsesNum, nodeSynapseTypeNum, iter->second)) {
					continue;
				}
				int idx = getOffset(pSynapsesNum, nodeSynapseTypeNum, iter->second);
				iter = nid2idx.find(s2nIter->second);
				if (iter == nid2idx.end()) {
					printf("Can't find ID %s\n", s2nIter->first.getInfo().c_str());
				}
				pSynapsesDst[idx] = iter->second;
			}

			addTypeConnection[pSTypes[i]](pAllSynapses[i], pSynapsesDst);
		}


		ret[nodeIdx].pNeurons = pAllNeurons;
		ret[nodeIdx].pSynapses = pAllSynapses;
		ret[nodeIdx].pN2SConnection = pAllConnections;

		ret[nodeIdx].nTypeNum = nodeNeuronTypeNum;
		ret[nodeIdx].sTypeNum = nodeSynapseTypeNum;
		ret[nodeIdx].nTypes = pNTypes;
		ret[nodeIdx].sTypes = pSTypes;
		ret[nodeIdx].neuronNums = pNeuronsNum;
		ret[nodeIdx].synapseNums = pSynapsesNum;

		ret[nodeIdx].MAX_DELAY = maxDelaySteps;
	}

	return ret;
}
