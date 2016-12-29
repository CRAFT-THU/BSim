/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "../neuron/Neuron.h"
#include "../synapse/Synapse.h"

//CPU SIM
#include "SingleThreadSimulator.h"

//GPU SIM
#include "SingleGPUSimulator.h"
#include "MultiGPUSimulator.h"


typedef SingleThreadSimulator STSim;

typedef SingleGPUSimulator SGSim;
typedef MultiGPUSimulator MGSim;

#endif /* SIMULATOR_H */

