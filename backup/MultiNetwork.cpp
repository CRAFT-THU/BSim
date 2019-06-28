/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

//#include "utils.h"
#include "MultiNetwork.h"

MultiNetwork::MultiNetwork(Network *net, int node_num)
{
	_crossnode_neurons_send.resize(node_num);
	_crossnode_neurons_recv.resize(node_num);
	_crossnode_neuron2idx.resize(node_num);

	_global_ntype_num.resize(node_num);
	_global_stype_num.resize(node_num);
	
	this->_node_num = node_num;
	this->_network = net;
}

MultiNetwork::~MultiNetwork()
{
	_crossnode_neurons_send.clear();
	_crossnode_neurons_recv.clear();
	_crossnode_neuron2idx.clear();

	_global_ntype_num.clear();
	_global_stype_num.clear();
}

CrossNodeData* MultiNetwork::arrangeCrossNodeData(int node_num)
{
	CrossNodeData * cross_data = (CrossNodeData*)malloc(sizeof(CrossNodeData) * node_num * node_num);
	assert(cross_data != NULL);

	for (int i=0; i<_node_num; i++) {
		for (int j=0; j<_node_num; j++) {
			// i->j 
			int i2j = j * _node_num + i;
			cross_data[i2j]._fired_n_num = 0;

			int count = 0;
			for (auto iter = _crossnode_neurons_send[i].begin(); iter != _crossnode_neurons_send[i].end(); iter++) {
				if (_crossnode_neurons_recv[j].find(*iter) != _crossnode_neurons_recv[j].end()) {
					count++;
				}
			}
			cross_data[i2j]._max_n_num = count;
			cross_data[i2j]._fired_n_idxs = (int*)malloc(sizeof(int)*count);
			assert(cross_data[i2j]._fired_n_idxs != NULL || count == 0);
		}
	}


	for (int i=0; i<_node_num; i++) {
		int idx = i*_node_num + i;
		for (int j=0; j<_node_num; j++) {
			if (j != i) {
				cross_data[idx]._max_n_num += cross_data[i*_node_num+j]._max_n_num;
			}
		}

		cross_data[idx]._fired_n_idxs = (int*)malloc(sizeof(int)*cross_data[idx]._max_n_num);
		assert(cross_data[idx]._fired_n_idxs != NULL || cross_data[idx]._max_n_num == 0);
	}

	return cross_data;
}
