/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GPU_FUNC_H
#define GPU_FUNC_H

#include "../base/type.h"
#include "../utils/BlockSize.h"

int cudaUpdatePoisson(void *data, int num, int start_id, BlockSize *pSize);
int cudaUpdateConstant(void *data, int num, int start_id, BlockSize *pSize);
int cudaUpdateLIF(void *data, int num, int start_id, BlockSize *pSize);
int cudaUpdateAllLIF(void *data, int num, int start_id, BlockSize *pSize);
int cudaUpdateExp(void *data, int num, int start_id, BlockSize *pSize);
int cudaUpdateAllExp(void *data, int num, int start_id, BlockSize *pSize);
int addCrossNeurons(int *ids, int num);

BlockSize * getBlockSize(int nSize, int sSize);


#endif /* GPU_FUNC_H */
