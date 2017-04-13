/* This header file is writen by qp09
 * usually just for fun
 * 二 二月 28 2017
 */
#ifndef MULTINODESIMULATOR_H
#define MULTINODESIMULATOR_H

#include "SimulatorBase.h"

class MultiGPUSimulator : public SimulatorBase {
public:
	MultiGPUSimulator(Network *network, real dt);
	~MultiGPUSimulator();
	virtual int run(real time);
	int init(int argc, char**argv);
protected:
	int rank;
	int rankSize;
};

#endif /* MULTINODESIMULATOR_H */

