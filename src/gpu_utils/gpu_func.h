/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GPU_FUNC_H
#define GPU_FUNC_H

#include "../base/constant.h"
#include "BlockSize.h"

extern int (*cudaAllocType[TypeSize])(void *, void *);
extern int (*cudaFreeType[TypeSize])(void *);
extern int (*updateType[TypeSize])(void *, int, int, BlockSize *pSize);
extern int (*copyType[TypeSize])(void *, void *, int, int);

extern BlockSize * getBlockSize(int nSize, int sSize);

#endif /* GFUNC_H */
