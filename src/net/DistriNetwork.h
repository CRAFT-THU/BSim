
#include "GNetwork.h"

struct CrossNodeMap {
	// ID of neurons on this node to index in this map 
	// index = _idx2index[id]
	int *_idx2index;
	// idx in this map to ID of shadow neurons on node j
	// id = _crossnode_index2idx[index * node_num + j], -1 means no shadow neuron on node j
	int *_crossnode_index2idx;
	// _cross_size = node_num * number_of_the_neurons_on_this_node_which_have_crossnode_connections
	int _cross_size;
};

// Assuming node number is N, we would use cross_data=CrossNodeData[N*N], cross_data[i+j*node_num] stores the ID of fired neurons on node i to be sent to node j and cross_data[j+j*node_num] stores allow the IDs received by node j
struct CrossNodeData {
	// Max number of fired neurons
	int _max_n_num;
	// Acutal number of fired neurons
	int _fired_n_num;
	// IDs of fired neurons
	int *_fired_n_idxs;
};

// Assuming node number is N. Parameter[i*node_num+j] stores corresponding paramter on node i to be sent to node[j+j*node_num] stores allow the IDs received by node j
struct CrossNodeDataGPU {
	// Max number of fired neurons
	int *_max_num;
	// Acutal number of fired neurons
	int *_fired_num;
	// IDs of fired neurons
	int **_fired_arrays;
};

struct DistriNetwork {
	int _sim_cycle;
	int _node_idx;
	int _node_num;
	real _dt;
	GNetwork * _network;
	CrossNodeMap *_crossnode_map;
	//CrossNodeData *_crossnode_data;
};

// Pointers inited to NULL, specific funcs in MultiNetwork will deal with these pointers later 
DistriNetwork* initDistriNet(int num);

// Not NULL pointers are also freed.
DistriNetwork* freeDistriNet(int num);
