/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef TYPEFUNC_H
#define TYPEFUNC_H

#include "../base/type.h"
#include "BlockSize.h"

extern void* (*createType[TYPESIZE])();
extern size_t (*getSize[TYPESIZE])();
extern int (*allocType[TYPESIZE])(void *, int);
extern int (*addConnection[TYPESIZE])(void *, int *);

extern int (*cudaAllocType[TYPESIZE])(void *, void *, int);
extern int (*cudaFreeType[TYPESIZE])(void *);
extern int (*updateType[TYPESIZE])(void *, int, int, BlockSize*);
extern int (*copyType[TYPESIZE])(void *, void *, int, int);

extern BlockSize * getBlockSize(int nSize, int sSize);

#endif /* TYPEFUNC_H */

