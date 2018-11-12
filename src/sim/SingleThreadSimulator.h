/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SINGLETHREADSIMULATOR_H
#define SINGLETHREADSIMULATOR_H

#include "../base/SimulatorBase.h"

class SingleThreadSimulator : public SimulatorBase {
public:
	SingleThreadSimulator(Network *network, real dt);
	~SingleThreadSimulator();

	using SimulatorBase::run;
	virtual int run(real time, FireInfo &log);
};

#endif /* SINGLETHREADSIMULATOR_H */

