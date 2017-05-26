
#include "GNetwork.h"

struct CrossNodeMap {
	int *_idx2index;
	int *_crossnode_index2idx;
	int _cross_size;
};

struct CrossNodeData {
	int _max_n_num;
	int _fired_n_num;
	int *_fired_n_idxs;
};

struct DistriNetwork {
	int _sim_cycle;
	int _node_idx;
	int _node_num;
	GNetwork * _network;
	CrossNodeMap *_crossnode_map;
	//CrossNodeData *_crossnode_data;
};

// Pointers inited to NULL, specific funcs in MultiNetwork will deal with these pointers later 
DistriNetwork* initDistriNet(int num);

// Not NULL pointers are also freed.
DistriNetwork* freeDistriNet(int num);
