/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef TYPEFUNC_H
#define TYPEFUNC_H

#include "../base/type.h"
#include "BlockSize.h"

extern void* (*createType[TYPESIZE])();
extern size_t (*getTypeSize[TYPESIZE])();
extern int (*allocType[TYPESIZE])(void *, int);
extern int (*freeType[TYPESIZE])(void *);
extern int (*addTypeConnection[TYPESIZE])(void *, int *);

extern int (*cudaAllocType[TYPESIZE])(void *, void *, int);
extern int (*cudaFreeType[TYPESIZE])(void *);
extern int (*cudaUpdateType[TYPESIZE])(void *, int, int, BlockSize*);
extern void (*mpiSendType[TYPESIZE])(void *data, int rank, int offset, int size);
extern void (*mpiRecvType[TYPESIZE])(void **data, int rank, int size);

extern BlockSize * getBlockSize(int nSize, int sSize);

#endif /* TYPEFUNC_H */

