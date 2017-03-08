/* This header file is writen by qp09
 * usually just for fun
 * 三 二月 22 2017
 */
#ifndef MULTINETWORK_H
#define MULTINETWORK_H

#include "Network.h"

struct CrossNodeMap {
	int *idx2index;
	int *crossNodeMap;
};

struct CrossNodeData {
	int maxNeuronNum;
	int firedNeuronNum;
	int *firedNeuronIdx;
};

class MultiNetwork {
public:
	MultiNetwork(Network *net);
	~MultiNetwork();

	void splitNetwork(int nodeNum);
	GNetwork* buildNetworks(int nodeNum, bool autoSplited = true);

public:
	Network * network;

	map<ID, set<int>> crossNodeInfo;
	//map<int, set<int>> crossNodeInfoGPU;

	map<ID, int> nid2node;
	map<ID, int> sid2node;

	vector<map<int, ID> > globalIdx2nid;
	vector<map<int, ID> > globalIdx2sid;
	vector<map<int, vector<int> > > crossNodeIdx2Idx;
	CrossNodeMap *crossNodeMap;
	CrossNodeData *crossNodeData;

	int nodeNum;
};


#endif /* MULTINETWORK_H */

