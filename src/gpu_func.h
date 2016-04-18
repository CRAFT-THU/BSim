/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GFUNC_H
#define GFUNC_H

#include "BlockSize.h"
#include "GLIFNeurons.h"
#include "GExpSynapses.h"
#include "GAlphaSynapses.h"

extern void* (*createType[TypeSize])();
extern size_t (*getSize[TypeSize])();
extern int (*allocType[TypeSize])(void *, int);
extern int (*allocConnect[TypeSize])(void *, int *, int *, int *, int);
extern int (*cudaAllocType[TypeSize])(void *, void *);
extern int (*cudaFreeType[TypeSize])(void *);
extern int (*updateType[TypeSize])(void *, int, int, BlockSize *pSize);

extern BlockSize * getBlockSize(int nSize, int sSize);

#endif /* GFUNC_H */
