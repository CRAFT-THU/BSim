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
	MultiNetwork(Network *net, int node_num = 1);
	~MultiNetwork();

	DistriNetwork * buildNetworks(bool auto_splited = true);
	CrossNodeData* arrangeCrossNodeData(int node_num);
	CrossNodeData* arrangeCrossNodeDataGPU(int node_num);
	//int addConnectionInfo(ID nID, int nid, int offset, int *delayStart, int *delayNum, int *pSynapsesIdx, int nodeIdx =0);

private:
	void splitNetwork();
	void countTypeNum();
        GNetwork* arrangeData(int node);
        N2SConnection* arrangeConnect(int n_num, int s_num, int node_idx);
	CrossNodeMap* arrangeCrossNodeMap(int n_num, int node_idx, int node_num);

public:
	//Cross Node
	//map<ID, int> _nID2node;
	//map<ID, int> _sID2node;
	vector<set<NeuronBase*> > _crossnode_neurons_send;
	vector<set<NeuronBase*> > _crossnode_neurons_recv;
	vector<map<NeuronBase*, int> > _crossnode_neuron2idx;

	//Per Node
	//vector<map<int, ID> > _global_idx2nID;
	//vector<map<int, ID> > _global_idx2sID;
	vector<map<Type, int> >	_global_ntype_num;
	vector<map<Type, int> > _global_stype_num;

	//Class Var
	Network * _network;
	int _node_num;
};

#endif /* MULTINETWORK_H */
