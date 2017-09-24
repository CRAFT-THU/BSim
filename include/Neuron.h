/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NEURON_H
#define NEURON_H


#include "../src/neuron/constant/ConstantNeuron.h"
#include "../src/neuron/poisson/PoissonNeuron.h"
#include "../src/neuron/array/ArrayNeuron.h"
#include "../src/neuron/lif/LIFENeuron.h"
#include "../src/neuron/tj/TJNeuron.h"
#include "../src/neuron/max/MaxNeuron.h"

#include "GNeuron.h"
#include "Synapse.h"

#include "../src/base/CompositeNeuron.h"

typedef CompositeNeuron<ConstantNeuron, StaticSynapse> Constant_spikes;
typedef CompositeNeuron<PoissonNeuron, StaticSynapse> Poisson_spikes;
typedef CompositeNeuron<ArrayNeuron, StaticSynapse> Array_spikes;

typedef CompositeNeuron<MaxNeuron, StaticSynapse> Max_pooling;
typedef CompositeNeuron<LIFENeuron, StaticSynapse> IF_curr_exp;
typedef CompositeNeuron<LIFENeuron, StaticSynapse> LIF_brian;
typedef CompositeNeuron<TJNeuron, StaticSynapse> TJ_curr_static;

typedef CompositeNeuron<MemNeuron, StaticSynapse> Mem;

typedef CompositeNeuron<FFTNeuron, StaticSynapse> FFT;

typedef CompositeNeuron<DecideNeuron, StaticSynapse> DecisionMaker;

#endif /* NEURON_H */
