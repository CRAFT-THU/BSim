/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GPU_FUNC_H
#define GPU_FUNC_H

int addCrossNeurons(int *ids, int num, int time);
int cudaDeliverNeurons(int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num, int neuron_num, int time);

#endif /* GPU_FUNC_H */
