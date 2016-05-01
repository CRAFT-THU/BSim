/* This program is writen by qp09.
 * usually just for fun.
 * Sun May 01 2016
 */

#include "mpi_func.h"

void (*sendType[])(void *data, int rank, int offset, int size) = { sendLIFNeurons, /*sendNengoNeurons, sendInputNeurons, sendPossionNeurons, sendProbeNeurons,*/ sendAlphaSynapses, sendExpSynapses/*, sendLowpassSynapses*/ };
void (*recvType[])(void **data, int rank, int size) = { recvLIFNeurons, /*recvNengoNeurons, recvInputNeurons, recvPossionNeurons, recvProbeNeurons,*/ recvAlphaSynapses, recvExpSynapses/*, recvLowpassSynapses*/ };
