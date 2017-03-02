/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

//#include "utils.h"
#include "MultiNetwork.h"

MultiNetwork::MultiNetwork(Network *net)
{
	network = net;
	crossNodeMap = NULL;
	crossNodeData = NULL;
	nodeNum = 1;
}

MultiNetwork::~MultiNetwork()
{
	delete network;

	if (crossNodeMap != NULL) {
		for (int i=0; i<nodeNum; i++) {
			delete [] (crossNodeMap[i].idx2index);
			delete [] (crossNodeMap[i].crossNodeMap);
		}
		delete [] crossNodeMap;
	}

	if (crossNodeData != NULL) {
		for (int i=0; i<nodeNum*nodeNum; i++) {
			delete [] (crossNodeData[i].firedNeuronIdx);
		}
		delete [] crossNodeData;
	}

	crossNodeInfo.clear();
	//crossNodeInfoGPU.clear();
	nid2node.clear();
	sid2node.clear();

	globalIdx2nid.clear();
	globalIdx2sid.clear();

	crossNodeIdx2Idx.clear();
}
