/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef MULTIGPUSIMULATOR_H
#define MULTIGPUSIMULATOR_H

#include "../interface/Simulator.h"

class MultiGPUSimulator : public Simulator {
public:
	MultiGPUSimulator(Network *network, real dt);
	~MultiGPUSimulator();
	using Simulator::run;
	virtual int run(real time, FireInfo &log);
	// virtual int single_run(real time);
	int init(int argc, char**argv);
protected:
	int rank;
	int rankSize;
};

#endif /* MULTIGPUSIMULATOR_H */

