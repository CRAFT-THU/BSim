/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GPU_FUNC_H
#define GPU_FUNC_H

#include "../base/type.h"
#include "../utils/BlockSize.h"

int updatePoissonNeuron(void *data, int num, int start_id, BlockSize *pSize);
int updateConstantNeuron(void *data, int num, int start_id, BlockSize *pSize);
int updateLIFNeuron(void *data, int num, int start_id, BlockSize *pSize);
int updateExpSynapses(void *data, int num, int start_id, BlockSize *pSize);
BlockSize * getBlockSize(int nSize, int sSize);


#endif /* GPU_FUNC_H */
