/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SIMULATOR_H
#define SIMULATOR_H

//CPU SIM
#include "SingleGPUSimulator.h"

//GPU SIM
#include "SingleThreadSimulator.h"

typedef SingleThreadSimulator STSim;

typedef SingleGPUSimulator SGSim;

#endif /* SIMULATOR_H */

