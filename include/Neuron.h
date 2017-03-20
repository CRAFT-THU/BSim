/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NEURON_H
#define NEURON_H


#include "../src/neuron/ConstantNeuron.h"
#include "../src/neuron/PoissonNeuron.h"
#include "../src/neuron/ArrayNeuron.h"
#include "../src/neuron/LIFNeuron.h"
//#include "NengoNeuron.h"
//#include "ProbeNeuron.h"

#include "GNeuron.h"

#include "Synapse.h"

#include "../src/base/CompositeNeuron.h"

typedef CompositeNeuron<ConstantNeuron, ExpSynapse> Constant_curr_exp;
typedef CompositeNeuron<LIFNeuron, ExpSynapse> IF_curr_exp;

//typedef CompositeNeuron<InputNeuron, LowpassSynapse> Input_lowpass;
//typedef CompositeNeuron<ProbeNeuron, LowpassSynapse> Probe_lowpass;
//typedef CompositeNeuron<NengoNeuron, LowpassSynapse> Nengo_lowpass2;

//typedef CompositeNeuron<LIFNeuron, BasicSynapse> IF_curr_basic;
//typedef CompositeNeuron<MaxNeuron, BasicSynapse> MAX_curr_basic;

#endif /* NEURON_H */
