/* This header file is writen by qp09
 * usually just for fun
 * Tue April 05 2016
 */
#ifndef GPU_UTILS_H
#define GPU_UTILS_H

#include "../net/GNetwork.h"

GNetwork* copyDataToGPU(GNetwork *);
int freeGPUData(GNetwork *);

#endif /* GPU_UTILS_H */

