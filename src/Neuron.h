/* This header file is writen by qp09
 * usually just for fun
 * Sat October 24 2015
 */
#ifndef NEURON_H
#define NEURON_H

#include "Synapse.h"

#include "NeuronBase.h"
#include "PossionNeuron.h"
#include "InputNeuron.h"
#include "GLIFNeurons.h"
#include "IF_curr_exp.h"
#include "NengoNeuron.h"
#include "ProbeNeuron.h"

#include "CompositeNeuron.h"

typedef CompositeNeuron<InputNeuron, LowpassSynapse> Input_lowpass;
typedef CompositeNeuron<ProbeNeuron, LowpassSynapse> Probe_lowpass;
typedef CompositeNeuron<NengoNeuron, LowpassSynapse> Nengo_lowpass2;

#endif /* NEURON_H */

