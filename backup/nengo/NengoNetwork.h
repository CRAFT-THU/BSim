/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NENGONETWORK_H
#define NENGONETWORK_H

#include <vector>
#include <map>
#include <algorithm>

#include "InputPopulation.h"
#include "NengoPopulation.h"
#include "NengoProbe.h"
#include "NengoLowpass.h"

using std::map;
using std::pair;
using std::find;
using std::vector;


class NengoNetwork {
public:
	NengoNetwork();
	~NengoNetwork();

	InputPopulation* createInput(Json::Value input);
	NengoPopulation* createPopulation(Json::Value population);
	int connect(NengoBase *pSrc, NengoBase *pDst, real *weight, real tau);
	int probe(int populationID, real *weight);
	
	NengoBase* findPopulation(int populationID);

//protected:
	vector<NengoBase*> pPopulations;
	vector<NengoLowpass*> pSynapses;
	int populationNum;
};


#endif /* NENGONETWORK_H */
