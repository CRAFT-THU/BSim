/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include "Network.h"
#include "utils.h"

Network::Network()
{
}

Network::~Network()
{
	if (!pPopulations.empty()) {
		vector<PopulationBase*>::iterator iter;
		for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
			PopulationBase * t = *iter;
			delete t;
		}
	}

	if (!pNeurons.empty()) {
		vector<NeuronBase*>::iterator iter;
		for (iter = pNeurons.begin(); iter != pNeurons.end(); iter++) {
			NeuronBase * t = *iter;
			delete t;
		}
	}

	if (!pSynapses.empty()) {
		vector<SynapseBase*>::iterator iter;
		for (iter = pSynapses.begin(); iter != pSynapses.end(); iter++) {
			SynapseBase * t = *iter;
			delete t;
		}
	}

	pPopulations.clear();
	pNeurons.clear();
	pSynapses.clear();
}

SynapseBase* Network::connect(NeuronBase *pn1, NeuronBase *pn2, real weight, real delay, SpikeType type, real tau, bool store)
{
	if (store) {
		if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
			pNeurons.push_back(pn1);
		}
		if (find(pNeurons.begin(), pNeurons.end(), pn2) == pNeurons.end()) {
			pNeurons.push_back(pn2);
		}
	}

	SynapseBase * p = pn1->addSynapse(weight, delay, type, tau, pn2);

	pSynapses.push_back(p);
	synapseNum++;
	s2nNetwork[p->getID()] = pn2->getID(); 
	n2sNetwork[pn1->getID()].push_back(p->getID());

	if (delay > maxDelay) {
		maxDelay = delay;
	}
	
	return p;
}

PopulationBase* Network::findPopulation(int populationID)
{
	PopulationBase *pP= NULL;
	vector<PopulationBase*>::iterator iter;
	for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
		PopulationBase * t = *iter;
		if (t->getID().id == populationID) {
			pP = *iter;
		}
		if (pP != NULL) {
			break;
		}
	}

	//if (pP == NULL) {
	//	printf("Cann't find population: %d\n", populationID);
	//	return NULL;
	//}

	return pP;
}

NeuronBase* Network::findNeuron(int populationIDSrc, int neuronIDSrc)
{
	//PopulationBase *pP= NULL;
	//vector<PopulationBase*>::iterator iter;
	//for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
	//	PopulationBase * t = *iter;
	//	if (t->getID().id == populationIDSrc) {
	//		pP = *iter;
	//	}
	//	if (pP != NULL) {
	//		break;
	//	}
	//}
	
	PopulationBase *pP = findPopulation(populationIDSrc);
	if (pP == NULL) {
		//printf("Cann't find population: %d\n", populationIDSrc);
		return NULL;
	}

	NeuronBase *pN = NULL;
	pN = pP->findNeuron(ID(populationIDSrc, neuronIDSrc));
	if (pN == NULL) {
		//printf("Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return NULL;
	}

	return pN;
}

int Network::addOutput(int populationIDSrc, int neuronIDSrc, double weight)
{
	NeuronBase *pN = findNeuron(populationIDSrc, neuronIDSrc);
	if (pN == NULL) {
		printf("Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return -1;
	} else {
		pOutputs.push_back(pN);
	}

	NeuronBase *probe = findNeuron(-2, populationIDSrc);
	if (probe == NULL) {
		PopulationBase *pP = findPopulation(-2);
		if (pP == NULL) {
			pP = createPopulation(ID(0, -2), 1, Probe_lowpass(ProbeNeuron(ID(0, 0), 0.01, 0.01), ID(0, 0)));
		}
		Population<Probe_lowpass> *pNewP = (Population<Probe_lowpass>*)pP;
		pNewP->addNeuron(Probe_lowpass(ProbeNeuron(ID(0, 0), 0.01, 0.01), ID(-2, populationIDSrc)));
		probe = findNeuron(-2, populationIDSrc);
		if (probe == NULL) {
			printf("Cann't find neuron: %d:%d\n", -2, populationIDSrc);
			return -1;
		}
		probe->monitorOn();
	}
	SynapseBase * t = connect(pN, probe, weight, 0, Excitatory, 0.01, false);
	t->monitorOn();

	return 0;
}

int Network::addMonitor(int populationIDSrc, int neuronIDSrc)
{
	NeuronBase *pN = findNeuron(populationIDSrc, neuronIDSrc);
	if (pN == NULL) {
		printf("Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return -1;
	} else {
		pN->monitorOn();
	}

	return 0;
}

int Network::connect(int populationIDSrc, int neuronIDSrc, int populationIDDst, int neuronIDDst, real weight, real delay, real tau)
{
	//PopulationBase *ppSrc = NULL, *ppDst = NULL;
	//vector<PopulationBase*>::iterator iter;
	//for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
	//	PopulationBase * t = *iter;
	//	if (t->getID().id == populationIDSrc) {
	//		ppSrc = *iter;
	//	}
	//	if (t->getID().id == populationIDDst) {
	//		ppDst = *iter;
	//	}

	//	if ((ppSrc != NULL) && (ppDst != NULL)) {
	//		break;
	//	}
	//}

	//if (ppSrc == NULL) {
	//	printf("Cann't find population: %d\n", populationIDSrc);
	//	return -1;
	//}

	//if (ppDst == NULL) {
	//	printf("Cann't find population: %d\n", populationIDDst);
	//	return -2;
	//}

	NeuronBase *pnSrc = NULL, *pnDst = NULL;
	pnSrc = findNeuron(populationIDSrc, neuronIDSrc);
	pnDst = findNeuron(populationIDDst, neuronIDDst);

	if (pnSrc == NULL) {
		printf("Cann't find neuron: %d:%d\n", populationIDSrc, neuronIDSrc);
		return -1;
	}

	if (pnDst == NULL) {
		printf("Cann't find neuron: %d:%d\n", populationIDDst, neuronIDDst);
		return -2;
	}
	
	SpikeType type = Excitatory;
	if (delay < 0) {
		type = Inhibitory;
	}
	connect(pnSrc, pnDst, weight, delay, type, tau, false);

	return 0;
}


GNetwork* Network::buildNetwrok()
{
	//size_t populationSize = 0;
	//size_t neuronSize = 0;
	//size_t synapseSize = 0;
	vector<PopulationBase*>::iterator piter;
	vector<NeuronBase*>::iterator niter;
	vector<SynapseBase*>::iterator siter;

	//for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
	//	PopulationBase * p = *piter;
	//	populationSize += p->getSize();
	//	neuronNum += p->getNum();
	//}
	//for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
	//	NeuronBase * p = *niter;
	//	neuronSize += p->getSize();
	//	neuronNum++;
	//}
	//for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
	//	SynapseBase * p = *siter;
	//	synapseSize += p->getSize();
	//	synapseNum++;
	//}

	//ret->populationSize = populationSize;
	//ret->neuronSize = neuronSize;
	//ret->synapseSize = synapseSize;
	
	GLIFNeurons *pGLIF = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));
	GExpSynapses *pGExp = (GExpSynapses*)malloc(sizeof(GExpSynapses));
	pGLIF->allocNeurons(neuronNum);
	pGLIF->allocConnects(synapseNum);
	pGExp->allocSynapses(synapseNum);

	int idx = 0;
	for (piter = pPopulations.begin(); piter != pPopulations.end();  piter++) {
		PopulationBase * p = *piter;
		size_t copied = p->hardCopy(pGLIF, idx);
		idx += copied;
	}
	for (niter = pNeurons.begin(); niter != pNeurons.end();  niter++) {
		NeuronBase * p = *niter;
		size_t copied = p->hardCopy(pGLIF, idx);
		idx += copied;
	}
	idx = 0;
	for (siter = pSynapses.begin(); siter != pSynapses.end();  siter++) {
		SynapseBase * p = *siter;
		int copied = p->hardCopy(pGExp, idx);
		idx += copied;
	}

	map<ID, vector<ID>>::iterator n2sIter;
	map<ID, ID>::iterator s2nIter;
	int loc = 0;
	for (n2sIter = n2sNetwork.begin(); n2sIter != n2sNetwork.end(); n2sIter++) {
		int idx = id2idx(pGLIF->pID, neuronNum, n2sIter->first);
		pGLIF->pSynapsesNum[idx] = n2sIter->second.size();
		pGLIF->pSynapsesLoc[idx] = loc;
		for (int i = 0; i<pGLIF->pSynapsesNum[idx]; i++) {
			int idx2 = id2idx(pGExp->pID, synapseNum, n2sIter->second.at(i)); 
			pGLIF->pSynapsesIdx[loc] = idx2;
			loc++;
			pGExp->pSrc[idx2] = idx;
		}	
	}

	for (s2nIter = s2nNetwork.begin(); s2nIter != s2nNetwork.end(); s2nIter++) {
		int idx = id2idx(pGExp->pID, synapseNum, s2nIter->first);
		pGExp->pDst[idx] = id2idx(pGLIF->pID, neuronNum, s2nIter->second);
	}

	GNetwork * ret = (GNetwork*)malloc(sizeof(GNetwork));
	if (ret == NULL) {
		printf("Malloc GNetwork failed/n");
		return NULL;
	}

	ret->neuronNum = neuronNum;
	ret->synapseNum = synapseNum;
	ret->pNeurons = pGLIF;
	ret->pSynapses = pGExp;
	ret->MAX_DELAY = maxDelay;

	return ret;
}
