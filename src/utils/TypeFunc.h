/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef TYPEFUNC_H
#define TYPEFUNC_H

#include <stdio.h>
#include "type.h"
#include "../net/Connection.h"
#include "BlockSize.h"

extern size_t (*getTypeSize[TYPESIZE])();
extern void* (*mallocType[TYPESIZE])();
extern void* (*allocType[TYPESIZE])(int num);
extern int (*freeType[TYPESIZE])(void *pCPU);
extern int (*allocTypePara[TYPESIZE])(void *pCPU, int num);
extern int (*freeTypePara[TYPESIZE])(void *pCPU);
extern int (*saveType[TYPESIZE])(void *pCPU, int num, FILE *f);
extern void* (*loadType[TYPESIZE])(int num, FILE *f);
// extern int (*addTypeConnection[TYPESIZE])(void *, int *);
extern void (*updateType[TYPESIZE])(Connection *, void *, real *, real *, int *, int*, int, int, int);

extern bool (*isEqualType[TYPESIZE])(void *p1, void *p2, int num);

extern void *(*cudaAllocType[TYPESIZE])(void *pCPU, int num);
// extern int (*cudaTypeToGPU[TYPESIZE])(void *pCPU, void *pGPU, int num);
extern int (*cudaFetchType[TYPESIZE])(void *pCPU, void *pGPU, int num);
extern int (*cudaFreeType[TYPESIZE])(void *);
// extern void (*cudaFindType[TYPESIZE])(void *, int, int);
// extern void (*cudaUpdateNeuron[TYPESIZE])(void *, real *, real *, int *, int*, int, int, int, BlockSize *);
// extern void (*cudaUpdateSynapse[TYPESIZE])(void *, void *, real *, real *, int *, int*, int, int, int, BlockSize *);
extern void (*cudaUpdateType[TYPESIZE])(LolConnection *, void *, real *, real *, int *, int*, int, int, int, BlockSize *);

extern int (*mpiSendType[TYPESIZE])(void *data, int rank, int offset, int size);
extern int (*mpiRecvType[TYPESIZE])(void **data, int rank, int size);

extern BlockSize * getBlockSize(int nSize, int sSize);

#endif /* TYPEFUNC_H */

