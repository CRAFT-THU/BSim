/* This header file is writen by qp09
 * usually just for fun
 * 三 二月 22 2017
 */
#ifndef MULTINETWORK_H
#define MULTINETWORK_H

#include "Network.h"
#include "DistriNetwork.h"

class MultiNetwork {
public:
	MultiNetwork(Network *net, int nodeNum = 1);
	~MultiNetwork();

	void splitNetwork(int nodeNum);
	DistriNetwork * buildNetworks(int nodeNum, bool autoSplited = true);
	//int addConnectionInfo(ID nID, int nid, int offset, int *delayStart, int *delayNum, int *pSynapsesIdx, int nodeIdx =0);

private:
	void countTypeNum(int nodeNum, bool autoSplited);


public:
	Network * network;

	map<ID, set<int>> _crossNodeInfo;
	//map<int, set<int>> crossNodeInfoGPU;
	
	vector<map<Type, int> >	_globalNTypeInfo;
	vector<map<Type, int> > _globalSTypeInfo;

	map<ID, int> _nID2node;
	map<ID, int> _sID2node;

	vector<map<int, ID> > _globalIdx2nID;
	vector<map<int, ID> > _globalIdx2sID;
	vector<map<int, vector<int> > > _crossNodeIdx2Idx;

	//CrossNodeMap *_crossNodeMap;
	//CrossNodeData *_crossNodeData;

	int nodeNum;
};


#endif /* MULTINETWORK_H */

