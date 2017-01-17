/* This program is writen by qp09.
 * usually just for fun.
 * Sat March 12 2016
 */

#include "../neuron/GNeuron.h"
#include "../synapse/GSynapse.h"
#include "Gfunc.h"


void* (*createType[])() = { createConstantNeurons, createLIFNeurons, /*createNengoNeurons, createInputNeurons, createPossionNeurons, createProbeNeurons, createBasicSynapses, createAlphaSynapses,*/ createExpSynapses/*, createLowpassSynapses*/ };

size_t (*getSize[])() = { getConstantSize, getLIFSize, /*getNengoSize, getInputSize, getPossionSize, getProbeSize, getBasicSize, getAlphaSize,*/ getExpSize/*, getLowpassSize*/ };

int (*allocType[])(void *, int) = { allocConstantNeurons, allocLIFNeurons, /*allocNengoNeurons, allocInputNeurons, allocPossionNeurons, allocProbeNeurons, allocBasicSynapses, allocAlphaSynapses,*/ allocExpSynapses/*, allocLowpassSynapses*/ };

int (*addConnection[])(void *, int *) = { NULL, NULL, /*NULL, NULL, NULL, NULL, addBasicConnection, addAlphaConnection,*/ addExpConnection/*, addLowpassConnection*/ };
