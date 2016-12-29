/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef GFUNC_H
#define GFUNC_H

#include "../base/constant.h"

extern void* (*createType[TypeSize])();
extern size_t (*getSize[TypeSize])();
extern int (*allocType[TypeSize])(void *, int);
extern int (*allocConnect[TypeSize])(void *, int *, int *, int *, int);

#endif /* GFUNC_H */

