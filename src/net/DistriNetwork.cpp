/* This program is writen by qp09.
 * usually just for fun.
 * 四 三月 09 2017
 */

#include <stdlib.h>
#include <assert.h>

#include "DistriNetwork.h"

DistriNetwork* initDistriNet(int num, real dt)
{
	DistriNetwork * ret_net = (DistriNetwork*)malloc(sizeof(DistriNetwork) * num);

	for (int i=0; i<num; i++) {
		ret_net[i]._simCycle = 0;
		ret_net[i]._nodeIdx = i;
		ret_net[i]._nodeNum = num;

		ret_net[i]._dt = dt;

		ret_net[i]._network = NULL;
		ret_net[i]._crossnodeMap = NULL; 

		//ret_net[i]._crossnode_data = (CrossNodeData*)malloc(sizeof(CrossNodeData)*num);
		//assert(ret_net[i]._crossnode_data != NULL);
		//for (int j=0; j<num; j++) {
		//	ret_net[i]._crossnode_data[j]._max_n_num = 0;
		//	ret_net[i]._crossnode_data[j]._fired_n_num = 0;
		//	ret_net[i]._crossnode_data[j]._fired_n_idxs = NULL;
		//}
	}

	return ret_net;
}

void freeDistriNet(DistriNetwork * net)
{
	int num = net->_nodeNum;
	for (int i=0; i<num; i++) {
		//if (net[i]._crossnode_data != NULL) {
		//	for (int j=0; j<num; j++) {
		//		free(net[i]._crossnode_data[j]._fired_n_idxs);
		//	}
		//	free(net[i]._crossnode_data);
		//	net[i]._crossnode_data = NULL;
		//}

		if (net[i]._crossnodeMap != NULL) {
			free(net[i]._crossnodeMap->_idx2index);
			free(net[i]._crossnodeMap->_crossnodeIndex2idx);
			free(net[i]._crossnodeMap); 
			net[i]._crossnodeMap = NULL;
		}

		//TODO free GNetwork
	}
	free(net);
}
