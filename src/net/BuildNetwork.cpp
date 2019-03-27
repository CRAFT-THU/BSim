
#include <assert.h>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "Network.h"
#include "../neuron/array/ArrayNeuron.h"
#include "../neuron/array/GArrayNeurons.h"

void arrangeFireArray(vector<int> &fire_array, vector<int> &start_loc, PopulationBase *popu)
{
	size_t num = popu->getNum();
	for (size_t i=0; i<num; i++) {
		ArrayNeuron *p = dynamic_cast<ArrayNeuron*>(popu->getNeuron(i));
		vector<int> &vec = p->getFireTimes();
		start_loc.push_back(fire_array.size());
		fire_array.insert(fire_array.end(), vec.begin(), vec.end());
	}
}

void arrangeArrayNeuron(vector<int> &fire_array, vector<int> &start_loc, GArrayNeurons *p, int num)
{
	assert(num == (int)start_loc.size());
	for (int i=0; i<num; i++) {
		p->p_start[i] = start_loc[i];
		p->p_end[i] += p->p_start[i];
		if (i > 0) {
			assert(p->p_end[i-1] == p->p_start[i]);
		}
	}
	assert(p->p_end[num-1] == (int)fire_array.size());
	p->p_fire_time = static_cast<int*>(malloc(sizeof(int) * fire_array.size()));
	std::copy(fire_array.begin(), fire_array.end(), p->p_fire_time);
}

GNetwork* Network::buildNetwork()
{
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	int neuronTypeNum = nTypes.size();
	int synapseTypeNum = sTypes.size();

	void **pAllNeurons = (void**)malloc(sizeof(void*)*neuronTypeNum);
	assert(pAllNeurons != NULL);
	void **pAllSynapses = (void**)malloc(sizeof(void*)*synapseTypeNum);
	assert(pAllSynapses != NULL);

	int *pNeuronsNum = (int*)malloc(sizeof(int)*(neuronTypeNum + 1));
	assert(pNeuronsNum != NULL);
	int *pSynapsesNum = (int*)malloc(sizeof(int)*(synapseTypeNum + 1));
	assert(pSynapsesNum != NULL);
	pNeuronsNum[0] = 0;
	pSynapsesNum[0] = 0;

	Type *pNTypes = (Type*)malloc(sizeof(Type)*neuronTypeNum);
	assert(pNTypes != NULL);
	Type *pSTypes = (Type*)malloc(sizeof(Type)*synapseTypeNum);
	assert(pSTypes != NULL);

	vector<int> array_neuron_start;
	vector<int> array_neuron_fire_times;

	for (int i=0; i<neuronTypeNum; i++) {
		pNTypes[i] = nTypes[i];

		void *pN = createType[nTypes[i]]();
		assert(pN != NULL);
		allocType[nTypes[i]](pN, neuronNums[i]);

		int idx = 0;
		for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
			PopulationBase * p = *piter;
			if (p->getType() == nTypes[i]) {
				size_t copied = p->hardCopy(pN, idx, pNeuronsNum[i]);
				idx += copied;
				if (p->getType() == Array) {
					arrangeFireArray(array_neuron_fire_times, array_neuron_start, p);
				}

			}
		}

		assert(idx == neuronNums[i]);

		if (nTypes[i] == Array) {
			arrangeArrayNeuron(array_neuron_fire_times, array_neuron_start, static_cast<GArrayNeurons*>(pN), idx);
		}

		pNeuronsNum[i+1] = idx + pNeuronsNum[i];
		pAllNeurons[i] = pN;
	}
	assert(pNeuronsNum[neuronTypeNum] == totalNeuronNum);

	for (int i=0; i<synapseTypeNum; i++) {
		pSTypes[i] = sTypes[i];

		void *pS = createType[sTypes[i]]();
		assert(pS != NULL);
		allocType[sTypes[i]](pS, synapseNums[i]);

		int idx = 0;
		for (auto piter = pPopulations.begin(); piter != pPopulations.end(); piter++) {
			PopulationBase * p = *piter;
			for (int nidx=0; nidx<p->getNum(); nidx++) {
				const vector<SynapseBase*> &s_vec = p->getNeuron(nidx)->getSynapses();
				for (int delay_t=0; delay_t < maxDelaySteps; delay_t++) {
					for (auto siter = s_vec.begin(); siter != s_vec.end(); siter++) {
						if ((*siter)->getDelay() == delay_t + 1) {
							if ((*siter)->getType() == sTypes[i]) {
								//int sid = (*iter)->getID();
								//assert(synapseIdx < totalSynapseNum);
								//pSynapsesIdx[synapseIdx] = sid;
								//synapseIdx++;
								int copied = (*siter)->hardCopy(pS, idx, pSynapsesNum[i]);
								idx += copied;
							}
						}
					}
				}
			}
		}
		//for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
		//	SynapseBase * p = *siter;
		//	if (p->getType() == sTypes[i]) {
		//		int copied = p->hardCopy(pS, idx, pSynapsesNum[i]);
		//		idx += copied;
		//	}
		//}

		assert(idx == synapseNums[i]);
		pSynapsesNum[i+1] = idx + pSynapsesNum[i];
		pAllSynapses[i] = pS;
	}
	assert(pSynapsesNum[synapseTypeNum] == totalSynapseNum);

	logMap();

	N2SConnection *pAllConnections = (N2SConnection*)malloc(sizeof(N2SConnection));
	assert(pAllConnections != NULL);

	pAllConnections->n_num = totalNeuronNum;
	pAllConnections->s_num = totalSynapseNum;
	pAllConnections->MAX_DELAY = this->maxDelaySteps;

	int *delayNum = (int*)malloc(sizeof(int)*(this->maxDelaySteps)*totalNeuronNum);
	assert(delayNum != NULL);
	int *delayStart = (int*)malloc(sizeof(int)*(this->maxDelaySteps)*totalNeuronNum);
	assert(delayStart != NULL);
	int *pSynapsesIdx = (int*)malloc(sizeof(int)*totalSynapseNum);
	assert(pSynapsesIdx != NULL);



	int synapseIdx = 0;
	for (auto piter = pPopulations.begin(); piter != pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		for (int i=0; i<p->getNum(); i++) {
			ID nid = p->getNeuron(i)->getID();
			const vector<SynapseBase*> &s_vec = p->getNeuron(i)->getSynapses();
			for (int delay_t=0; delay_t < maxDelaySteps; delay_t++) {
				delayStart[delay_t + maxDelaySteps*nid] = synapseIdx;

				for (auto iter = s_vec.begin(); iter != s_vec.end(); iter++) {
					if ((*iter)->getDelay() == delay_t + 1) {
						int sid = (*iter)->getID();
						assert(synapseIdx < totalSynapseNum);
						pSynapsesIdx[synapseIdx] = sid;
						synapseIdx++;
					}
				}

				delayNum[delay_t + maxDelaySteps*nid] = synapseIdx - delayStart[delay_t + maxDelaySteps*nid];
			}
		}
	}

	for (int i=0; i<totalSynapseNum; i++) {
		assert(pSynapsesIdx[i] == i);
	}

	free(pSynapsesIdx);
	//pAllConnections->pSynapsesIdx = pSynapsesIdx;
	pAllConnections->delayStart = delayStart;
	pAllConnections->delayNum = delayNum;


	GNetwork * ret = (GNetwork*)malloc(sizeof(GNetwork));
	assert(ret != NULL);

	ret->pNeurons = pAllNeurons;
	ret->pSynapses = pAllSynapses;
	ret->pN2SConnection = pAllConnections;

	ret->nTypeNum = neuronTypeNum;
	ret->sTypeNum = synapseTypeNum;
	ret->nTypes = pNTypes;
	ret->sTypes = pSTypes;
	ret->neuronNums = pNeuronsNum;
	ret->synapseNums = pSynapsesNum;

	ret->MAX_DELAY = maxDelaySteps;

	return ret;
}
