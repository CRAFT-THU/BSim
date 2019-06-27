/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SINGLEGPUSIMULATOR_H
#define SINGLEGPUSIMULATOR_H

#include "../interface/Simulator.h"

class SingleGPUSimulator : public Simulator{
public:
	SingleGPUSimulator(Network *network, real dt);
	~SingleGPUSimulator();

	using Simulator::run;
	virtual int run(real time, FireInfo &log);
	int runMultiNets(real time, int parts, FireInfo &log);
	//int compare_run(real time);
	//int cudaAllocGlobal();
	//int cudaFreeGlobal();
};

#endif /* SINGLEGPUSIMULATOR_H */

