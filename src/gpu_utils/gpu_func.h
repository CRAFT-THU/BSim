/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GPU_FUNC_H
#define GPU_FUNC_H

#include "../base/type.h"
#include "../utils/BlockSize.h"

int cudaUpdatePoisson(void *data, int num, int start_id, int t, BlockSize *pSize);
int cudaUpdateConstant(void *data, int num, int start_id, int t, BlockSize *pSize);
int cudaUpdateArray(void *data, int num, int start_id, int t, BlockSize *pSize);
int cudaUpdateLIFE(void *data, int num, int start_id, int t, BlockSize *pSize);
int cudaUpdateTJ(void *data, int num, int start_id, int t, BlockSize *pSize);
int cudaUpdateMax(void *data, int num, int start_id, int t, BlockSize *pSize);
int cudaUpdateStatic(void *data, int num, int start_id, int t, BlockSize *pSize);

int addCrossNeurons(int *ids, int num);
int cudaDeliverNeurons(int *idx2index, int *crossnode_index2idx, int *global_cross_data, int *fired_n_num, int node_num, int neuron_num);

BlockSize * getBlockSize(int nSize, int sSize);


#endif /* GPU_FUNC_H */
