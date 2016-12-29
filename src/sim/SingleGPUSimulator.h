/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SINGLEGPUSIMULATOR_H
#define SINGLEGPUSIMULATOR_H

#include "SimulatorBase.h"

class SingleGPUSimulator : public SimulatorBase {
public:
	SingleGPUSimulator(Network *network, real dt);
	~SingleGPUSimulator();
	virtual int run(real time);
};

#endif /* SINGLEGPUSIMULATOR_H */

