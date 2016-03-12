/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GFUNC_H
#define GFUNC_H

#include "GLIFNeurons.h"
#include "GExpSynapses.h"
#include "GAlphaSynapses.h"

void* (*createType[])() = { createLIFNeurons, /*createNengoNeurons, createInputNeurons, createPossionNeurons, createProbeNeurons,*/ createAlphaSynapses, createExpSynapses/*, createLowpassSynapses*/ };

size_t (*getSize[])() = { getLIFSize, /*getNengoSize, getInputSize, getPossionSize, getProbeSize,*/ getAlphaSize, getExpSize/*, getLowpassSize*/ };

int (*allocType[])(void *, int) = { allocLIFNeurons, /*allocNengoNeurons, allocInputNeurons, allocPossionNeurons, allocProbeNeurons,*/ allocAlphaSynapses, allocExpSynapses/*, allocLowpassSynapses*/ };

int (*allocConnect[])(void *, int *, int *, int *, int) = { allocLIFConnects, /*allocNengoConnects, allocInputConnects, allocPossionConnects, allocProbeConnects,*/ allocAlphaConnects, allocExpConnects/*, allocLowpassConnects*/ };

int (*cudaAllocType[])(void *, void *) = { cudaAllocLIFNeurons, /*cudaAllocNengoNeurons, cudaAllocInputNeurons, cudaAllocPossionNeurons, cudaAllocProbeNeurons,*/ cudaAllocAlphaSynapses, cudaAllocExpSynapses/*, cudaAllocLowpassSynapses*/ };

int (*cudaFreeType[])(void *) = { cudaFreeLIFNeurons, /*cudaFreeNengoNeurons, cudaFreeInputNeurons, cudaFreePossionNeurons, cudaFreeProbeNeurons,*/ cudaFreeAlphaSynapses, cudaFreeExpSynapses/*, cudaFreeLowpassSynapses*/ };

int updateLIFNeuron(void *, int, int);
int updateAlphaSynapses(void *, int, int);
int updateExpSynapses(void *, int, int);

int (*updateType[])(void *, int, int) = {updateLIFNeuron, /*updateNengoNeurons, updateInputNeurons, updatePossionNeurons, updateProbeNeurons,*/ updateAlphaSynapses, updateExpSynapses/*, updateLowpassSynapses*/ };

#endif /* GFUNC_H */

