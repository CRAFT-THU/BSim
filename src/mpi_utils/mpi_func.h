/* This header file is writen by qp09
 * usually just for fun
 * Sun May 01 2016
 */
#ifndef MPI_FUNC_H
#define MPI_FUNC_H

#include "../net/GNetwork.h"
#include "../neuron/GNeuron.h"
#include "../synapse/GSynapse.h"

extern void (*sendType[TYPESIZE])(void *data, int rank, int offset, int size);
extern void (*recvType[TYPESIZE])(void **data, int rank, int size);

#endif /* MPI_FUNC_H */

