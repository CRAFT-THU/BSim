/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

//#include "utils.h"
#include "MultiNetwork.h"

MultiNetwork::MultiNetwork(Network *net, int node_num)
{
	_crossnode_IDs_send.resize(node_num);
	_crossnode_IDs_receive.resize(node_num);
	_crossnode_nID2idx.resize(node_num);
	//_crossnode_idx2idx.resize(node_num);

	//crossNodeMap = NULL;
	//crossNodeData = NULL;
	
	_global_idx2nID.resize(node_num);
	_global_idx2sID.resize(node_num);
	_global_ntype_num.resize(node_num);
	_global_stype_num.resize(node_num);
	
	this->_node_num = node_num;
	this->_network = net;
}

MultiNetwork::~MultiNetwork()
{
	//An external pointer, should not delete here
	//delete _network;

	//if (crossNodeMap != NULL) {
	//	for (int i=0; i<node_num; i++) {
	//		delete [] (crossNodeMap[i].idx2index);
	//		delete [] (crossNodeMap[i].crossNodeMap);
	//	}
	//	delete [] crossNodeMap;
	//}

	//if (crossNodeData != NULL) {
	//	for (int i=0; i<node_num*node_num; i++) {
	//		delete [] (crossNodeData[i].firedNeuronIdx);
	//	}
	//	delete [] crossNodeData;
	//}

	//_nID2node.clear();
	//_sID2node.clear();
	_crossnode_IDs_send.clear();
	_crossnode_IDs_receive.clear();
	//_crossnode_nID2nodes.clear();
	_crossnode_IDs_send.clear();
	_crossnode_IDs_receive.clear();
	_crossnode_nID2idx.clear();
	//_crossnode_idx2idx.clear();

	_global_idx2nID.clear();
	_global_idx2sID.clear();
	_global_ntype_num.clear();
	_global_stype_num.clear();
}
