/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

#include "Neuron.h"
#include "Synapse.h"

//CPU SIM
#include "../src/sim/SingleThreadSimulator.h"

//GPU SIM
#include "../src/sim/SingleGPUSimulator.h"
#include "../src/sim/MultiGPUSimulator.h"


typedef SingleThreadSimulator STSim;

typedef SingleGPUSimulator SGSim;
typedef MultiGPUSimulator MGSim;

#endif /* SIMULATOR_H */

