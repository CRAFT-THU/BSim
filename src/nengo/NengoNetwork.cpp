/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 19 2016
 */

#include "NengoNetwork.h"

NengoNetwork::NengoNetwork()
{
	pPopulations.clear();
	pSynapses.clear();
	populationNum = 0;
}

NengoNetwork::~NengoNetwork()
{
	if (!pPopulations.empty()) {
		vector<NengoBase*>::iterator iter;
		for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
			NengoBase * t = *iter;
			delete t;
		}
	}

	if (!pSynapses.empty()) {
		vector<NengoLowpass*>::iterator iter;
		for (iter = pSynapses.begin(); iter != pSynapses.end(); iter++) {
			NengoLowpass * t = *iter;
			delete t;
		}
	}

	pPopulations.clear();
	pSynapses.clear();
}

InputPopulation* NengoNetwork::createInput(Json::Value input)
{
	InputPopulation *pp1 = new InputPopulation(input);
	if (find(pPopulations.begin(), pPopulations.end(), pp1) == pPopulations.end()) {
		pPopulations.push_back(pp1);
		populationNum++;
	}

	return pp1;
}

NengoPopulation* NengoNetwork::createPopulation(Json::Value population)
{
	NengoPopulation *pp1 = new NengoPopulation(population);
	if (find(pPopulations.begin(), pPopulations.end(), pp1) == pPopulations.end()) {
		pPopulations.push_back(pp1);
		populationNum++;
	}

	return pp1;
}

int NengoNetwork::connect(NengoBase *pSrc, NengoBase *pDst, real *weight, real tau)
{
	int srcSize = pSrc->getSize();
	int dstSize = pDst->getSize();
	if (find(pPopulations.begin(), pPopulations.end(), pSrc) == pPopulations.end()) {
		pPopulations.push_back(pSrc);
		populationNum++;
	}
	if (find(pPopulations.begin(), pPopulations.end(), pDst) == pPopulations.end()) {
		pPopulations.push_back(pDst);
		populationNum++;
	}

	NengoLowpass *pConnect = new NengoLowpass(sidPool.getID(), srcSize, dstSize, weight, tau, pSrc->getOutput(), pDst->getInput());
	pSynapses.push_back(pConnect);

	return 0;
}

int NengoNetwork::probe(int populationID, real *weight)
{
	NengoBase *pDst = findPopulation(populationID);
	NengoProbe *pProbe = new NengoProbe(ID(-2, pDst->getID().groupId), pDst->getSize(), weight, pDst->getOutput());
	pPopulations.push_back(pProbe);

	return 0;
}

NengoBase* NengoNetwork::findPopulation(int populationID)
{
	NengoBase *pP= NULL;
	vector<NengoBase*>::iterator iter;
	for (iter = pPopulations.begin(); iter != pPopulations.end(); iter++) {
		NengoBase * t = *iter;
		if (t->getID().groupId == populationID) {
			pP = *iter;
		}
		if (pP != NULL) {
			break;
		}
	}

	if (pP == NULL) {
		printf("Cann't find population: %d\n", populationID);
		return NULL;
	}

	return pP;
}
