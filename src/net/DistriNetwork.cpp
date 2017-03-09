/* This program is writen by qp09.
 * usually just for fun.
 * 四 三月 09 2017
 */

#include <stdlib.h>
#include <assert.h>

#include "DistriNetwork.h"

DistriNetwork* initDistriNet(int num)
{
	DistriNetwork * ret_net = (DistriNetwork*)malloc(sizeof(DistriNetwork) * num);

	for (int i=0; i<num; i++) {
		ret_net[i]._network = (GNetwork *)malloc(sizeof(GNetwork));
		assert(ret_net[i]._network != NULL);

		ret_net[i]._crossnode_map = (CrossNodeMap*)malloc(sizeof(CrossNodeMap));
		assert(ret_net[i]._crossnode_map != NULL);
		ret_net[i]._crossnode_map->_idx2index = NULL;
		ret_net[i]._crossnode_map->_crossnode_index2idx = NULL;

		ret_net[i]._crossnode_data = (CrossNodeData*)malloc(sizeof(CrossNodeData)*num);
		assert(ret_net[i]._crossnode_data != NULL);
		for (int j=0; j<num; j++) {
			ret_net[i]._crossnode_data[j]._max_n_num = 0;
			ret_net[i]._crossnode_data[j]._fired_n_num = 0;
			ret_net[i]._crossnode_data[j]._fired_n_idxs = NULL;
		}
	}

	return ret_net;
}

void freeDistriNet(DistriNetwork * net, int num)
{
	for (int i=0; i<num; i++) {
		for (int j=0; j<num; j++) {
			free(net[i]._crossnode_data[j]._fired_n_idxs);
		}
		free(net[i]._crossnode_data);

		free(net[i]._crossnode_map->_idx2index);
		free(net[i]._crossnode_map->_crossnode_index2idx);
		free(net[i]._crossnode_map); 

		//TODO free GNetwork
	}
	free(net);
}
