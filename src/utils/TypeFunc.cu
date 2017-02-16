
#include "../gpu_utils/gpu_func.h"
#include "../neuron/GNeuron.h"
#include "../synapse/GSynapse.h"
#include "TypeFunc.h"

int (*cudaAllocType[])(void *, void *, int) = { cudaAllocConstantNeurons, cudaAllocPoissonNeurons, cudaAllocLIFNeurons, /*cudaAllocNengoNeurons, cudaAllocInputNeurons, cudaAllocPoissonNeurons, cudaAllocProbeNeurons, cudaAllocBasicSynapses, cudaAllocAlphaSynapses,*/ cudaAllocExpSynapses/*, cudaAllocLowpassSynapses*/ };

int (*cudaFreeType[])(void *) = { cudaFreeConstantNeurons, cudaFreePoissonNeurons, cudaFreeLIFNeurons, /*cudaFreeNengoNeurons, cudaFreeInputNeurons, cudaFreePoissonNeurons, cudaFreeProbeNeurons, cudaFreeBasicSynapses, cudaFreeAlphaSynapses,*/ cudaFreeExpSynapses/*, cudaFreeLowpassSynapses*/ };

int (*updateType[])(void *, int, int, BlockSize*) = { updateConstantNeuron, updatePoissonNeuron, updateLIFNeuron, /*updateBasicSynapses, updateAlphaSynapses,*/ updateExpSynapses };
