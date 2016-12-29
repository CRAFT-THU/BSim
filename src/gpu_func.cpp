/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "GNeuron.h"
#include "GSynapse.h"
#include "gpu_func.h"


void* (*createType[])() = { createConstantNeurons, createLIFNeurons, /*createNengoNeurons, createInputNeurons, createPossionNeurons, createProbeNeurons,*/ createBasicSynapses, createAlphaSynapses, createExpSynapses/*, createLowpassSynapses*/ };

size_t (*getSize[])() = { getConstantSize, getLIFSize, /*getNengoSize, getInputSize, getPossionSize, getProbeSize,*/ getBasicSize, getAlphaSize, getExpSize/*, getLowpassSize*/ };

int (*allocType[])(void *, int) = { allocConstantNeurons, allocLIFNeurons, /*allocNengoNeurons, allocInputNeurons, allocPossionNeurons, allocProbeNeurons,*/ allocBasicSynapses, allocAlphaSynapses, allocExpSynapses/*, allocLowpassSynapses*/ };

int (*allocConnect[])(void *, int *, int *, int *, int) = { allocConstantConnects, allocLIFConnects, /*allocNengoConnects, allocInputConnects, allocPossionConnects, allocProbeConnects,*/ allocBasicConnects, allocAlphaConnects, allocExpConnects/*, allocLowpassConnects*/ };

