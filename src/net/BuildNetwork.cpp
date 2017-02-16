
#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "Network.h"

GNetwork* Network::buildNetwork()
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	int neuronTypeNum = nTypes.size();
	int synapseTypeNum = sTypes.size();

	void **pAllNeurons = (void**)malloc(sizeof(void*)*neuronTypeNum);
	void **pAllSynapses = (void**)malloc(sizeof(void*)*synapseTypeNum);
	N2SConnection *pAllConnections = (N2SConnection*)malloc(sizeof(N2SConnection));

	int *pNOffsets = (int*)malloc(sizeof(int)*(neuronTypeNum));
	int *pSOffsets = (int*)malloc(sizeof(int)*(synapseTypeNum));
	int *pNeuronsNum = (int*)malloc(sizeof(int)*(neuronTypeNum + 1));
	int *pSynapsesNum = (int*)malloc(sizeof(int)*(synapseTypeNum + 1));
	pNeuronsNum[0] = 0;
	pSynapsesNum[0] = 0;

	Type *pNTypes = (Type*)malloc(sizeof(Type)*neuronTypeNum);
	Type *pSTypes = (Type*)malloc(sizeof(Type)*synapseTypeNum);
	int *pGNeuronNums = (int*)malloc(sizeof(int)*(neuronTypeNum));
	int *pGSynapseNums = (int*)malloc(sizeof(int)*(synapseTypeNum));
	pGNeuronNums[0] = 0;
	pGSynapseNums[0] = 0;

	for (int i=0; i<neuronTypeNum; i++) {
		pNTypes[i] = nTypes[i];

		void *pN = createType[nTypes[i]]();
		allocType[nTypes[i]](pN, neuronNums[i]);

		int idx = 0;
		for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
			PopulationBase * p = *piter;
			if (p->getType() == nTypes[i]) {
				size_t copied = p->hardCopy(pN, idx, pNeuronsNum[i], nid2idx, idx2nid);
				idx += copied;
			}
		}
		for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
			NeuronBase * p = *niter;
			if (p->getType() == nTypes[i]) {
				size_t copied = p->hardCopy(pN, idx, pNeuronsNum[i], nid2idx, idx2nid);
				idx += copied;
			}
		}

		pNOffsets[i] = 0;
		pNeuronsNum[i+1] = idx + pNeuronsNum[i];
		pGNeuronNums[i+1] = pNeuronsNum[i+1];
		pAllNeurons[i] = pN;

	}

	for (int i=0; i<synapseTypeNum; i++) {
		pSTypes[i] = sTypes[i];

		void *pS = createType[sTypes[i]]();
		allocType[sTypes[i]](pS, synapseNums[i]);

		int idx = 0;
		for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
			SynapseBase * p = *siter;
			if (p->getType() == sTypes[i]) {
				int copied = p->hardCopy(pS, idx, pSynapsesNum[i], sid2idx, idx2sid);
				idx += copied;
			}
		}

		pSOffsets[i] = 0;
		pSynapsesNum[i+1] = idx + pSynapsesNum[i];
		pGSynapseNums[i+1] = pSynapsesNum[i+1];
		pAllSynapses[i] = pS;
	}

	int *delayNum = (int*)malloc(sizeof(int)*(this->maxDelaySteps)*totalNeuronNum);
	int *delayStart = (int*)malloc(sizeof(int)*(this->maxDelaySteps)*totalNeuronNum);
	int *pSynapsesIdx = (int*)malloc(sizeof(int)*totalSynapseNum);

	int synapseIdx = 0;
	for (int nid=0; nid<totalNeuronNum; nid++) {
		map<int, ID>::iterator iter = idx2nid.find(nid);
		if (iter == idx2nid.end()) {
			printf("Can't find neuron index %d\n", nid);
		}
		map<ID, vector<ID>>::iterator n2siter = n2sNetwork.find(iter->second);
		if (n2siter == n2sNetwork.end()) {
			//printf("Cant't find neuron id %d_%d\n", iter->second.groupId, iter->second.id);
			
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

	for (int i=0; i<synapseTypeNum; i++) {
		int *pSynapsesDst = (int*)malloc(sizeof(int)*synapseNums[i]);
		map<ID, ID>::iterator s2nIter;
		for (s2nIter = s2nNetwork.begin(); s2nIter != s2nNetwork.end(); s2nIter++) {
			map<ID, int>::iterator iter = sid2idx.find(s2nIter->first);
			if (iter == sid2idx.end()) {
				printf("Can't find ID %s\n", s2nIter->first.getInfo().c_str());
			}
			if (i != getType(pSynapsesNum, synapseTypeNum, iter->second)) {
				continue;
			}
			int idx = getOffset(pSynapsesNum, synapseTypeNum, iter->second);
			iter = nid2idx.find(s2nIter->second);
			if (iter == nid2idx.end()) {
				printf("Can't find ID %s\n", s2nIter->second.getInfo().c_str());
			}
			pSynapsesDst[idx] = iter->second;
		}

		addConnection[sTypes[i]](pAllSynapses[i], pSynapsesDst);
	}

	GNetwork * ret = (GNetwork*)malloc(sizeof(GNetwork));
	if (ret == NULL) {
		printf("Malloc GNetwork failed/n");
		return NULL;
	}

	ret->pNeurons = pAllNeurons;
	ret->pSynapses = pAllSynapses;
	ret->pN2SConnection = pAllConnections;
	ret->nOffsets = pNOffsets;
	ret->sOffsets = pSOffsets;
	ret->gNeuronNums = pGNeuronNums;
	ret->gSynapseNums = pGSynapseNums;

	ret->nTypeNum = neuronTypeNum;
	ret->sTypeNum = synapseTypeNum;
	ret->nTypes = pNTypes;
	ret->sTypes = pSTypes;
	ret->neuronNums = pNeuronsNum;
	ret->synapseNums = pSynapsesNum;

	ret->MAX_DELAY = maxDelaySteps;

	return ret;
}
