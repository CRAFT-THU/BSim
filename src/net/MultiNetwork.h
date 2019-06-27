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

	DistriNetwork * buildNetworks(SimInfo &info, bool auto_splited = true);
	CrossNodeData* arrangeCrossNodeData(int node_num);
	CrossNodeDataGPU* arrangeCrossNodeDataGPU(int node_num);
	//int addConnectionInfo(ID nID, int nid, int offset, int *delayStart, int *delayNum, int *pSynapsesIdx, int nodeIdx =0);

private:
	void splitNetwork();
	void countTypeNum();
	GNetwork* arrangeData(int node, SimInfo &info);
	Connection* arrangeConnect(int n_num, int s_num, int node_idx, SimInfo &info);
	CrossNodeMap* arrangeCrossNodeMap(int n_num, int node_idx, int node_num);

public:
	/** Cross Node Data **/
	// map<ID, int> _nID2node;
	// map<ID, int> _sID2node;
	// Neurons that on this node and would issue spikes to others.
	// Acessed by neurons = _crossnode_neurons_send[node]
	vector<set<Neuron *> > _crossnode_neurons_send;
	// Neurons that on other nodes and would issue spike to this node.
	// Accessed by neurons = _crossnode_neurons_recv[node]
	vector<set<Neuron *> > _crossnode_neurons_recv;
	// Get idx of shadow neuron on destination node, the idxs of shadow neurons are larger than that of real neurons.
	// Accessed by idx = _crossnode_neuron2idx[node][neuron]
	vector<map<Neuron *, int> > _crossnode_neuron2idx;

	/** Per Node Data **/
	// vector<map<int, ID> > _global_idx2nID;
	// vector<map<int, ID> > _global_idx2sID;
	// Number of neurons for different types on different nodes accessed by _global_ntype_num[node][type]
	vector<map<Type, int> >	_global_ntype_num;
	// Number of synapses for different types on different nodes accessed by _global_ntype_num[node][type]
	vector<map<Type, int> > _global_stype_num;

	/** Class Var **/
	// NNs for each node
	Network * _network;
	// Number of nodes
	int _node_num;
};

#endif /* MULTINETWORK_H */
