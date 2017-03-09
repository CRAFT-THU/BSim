
#include "GNetwork.h"

struct CrossNodeMap {
	int *_idx2index;
	int *_crossnode_index2idx;
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
	CrossNodeData *_crossnode_data;
};

DistriNetwork* initDistriNet(int num);
DistriNetwork* freeDistriNet(int num);
