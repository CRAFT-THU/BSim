/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SINGLEGPUSIMULATOR_H
#define SINGLEGPUSIMULATOR_H

#include "../base/SimulatorBase.h"

class SingleGPUSimulator : public SimulatorBase {
public:
	SingleGPUSimulator(Network *network, real dt);
	~SingleGPUSimulator();

	using SimulatorBase::run;
	virtual int run(real time, FireInfo &log);
	int runMultiNets(real time, FireInfo &log);
	//int compare_run(real time);
	//int cudaAllocGlobal();
	//int cudaFreeGlobal();
};

#endif /* SINGLEGPUSIMULATOR_H */

