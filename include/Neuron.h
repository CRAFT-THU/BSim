/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NEURON_H
#define NEURON_H


#include "../src/neuron/ConstantNeuron.h"
#include "../src/neuron/PoissonNeuron.h"
#include "../src/neuron/ArrayNeuron.h"
#include "../src/neuron/LIFENeuron.h"
#include "../src/neuron/TJNeuron.h"
#include "../src/neuron/MaxNeuron.h"

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

#endif /* NEURON_H */
