
struct CrossNodeMap {
	int *idx2index;
	int *crossNodeMap;
};

struct CrossNodeData {
	int maxNeuronNum;
	int firedNeuronNum;
	int *firedNeuronIdx;
};

struct DistriNetwork {
	int simCycle;
	int nodeIdx;
	int nodeNum;
	GNetwork * network;
	CrossNodeMap *crossNodeMap;
	CrossNodeData *crossNodeData;
};
