/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATORS_H
#define SIMULATORS_H

#include "Neurons.h"
#include "Synapses.h"

//CPU SIM
#include "../src/sim/SingleThreadSimulator.h"

//GPU SIM
#include "../src/sim/SingleGPUSimulator.h"
#include "../src/sim/MultiGPUSimulator.h"


typedef SingleThreadSimulator STSim;

typedef SingleGPUSimulator SGSim;
typedef MultiGPUSimulator MGSim;

#endif /* SIMULATORS_H */

