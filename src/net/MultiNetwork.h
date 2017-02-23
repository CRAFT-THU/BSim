/* This header file is writen by qp09
 * usually just for fun
 * 三 二月 22 2017
 */
#ifndef MULTINETWORK_H
#define MULTINETWORK_H

#include "Network.h"

class MultiNetwork : public Network {
public:
	void splitNetwork(int nodeNum);
	GNetwork* buildNetworks(int nodeNum, bool autoSplited = false);

public:
	map<ID, set<int>> crossNodeInfo;
	map<int, set<int>> crossNodeInfoGPU;

	map<ID, int> nid2node;
	map<ID, int> sid2node;

	vector<map<int, ID> > globalIdx2nid;
	vector<map<int, ID> > globalIdx2sid;
	vector<map<int, vector<int> > > crossNodeIdx2Idx;
};


#endif /* MULTINETWORK_H */

