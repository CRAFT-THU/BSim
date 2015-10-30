/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <algorithm>
#include "Connector.h"

using std::find;

Connector::Connector()
{
}


Connector::~Connector()
{
	pNeurons.clear();
	pSynapses.clear();
}

int Connector::connect(NeuronBase *pn1)
{
	if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
		pNeurons.push_back(pn1);
	}

	return 1;
}

int Connector::connect(NeuronBase *pn1, NeuronBase *pn2, real weight, real delay, SynType type)
{
	if (find(pNeurons.begin(), pNeurons.end(), pn1) == pNeurons.end()) {
		pNeurons.push_back(pn1);
	}
	if (find(pNeurons.begin(), pNeurons.end(), pn2) == pNeurons.end()) {
		pNeurons.push_back(pn2);
	}

	SynapseBase * p = pn1->addSyn(weight, delay, type, pn2);

	pSynapses.push_back(p);
	
	return 1;
}
