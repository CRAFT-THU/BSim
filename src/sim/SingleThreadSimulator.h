/* This header file is writen by qp09
 * usually just for fun
 * Sun December 13 2015
 */
#ifndef SINGLETHREADSIMULATOR_H
#define SINGLETHREADSIMULATOR_H

#include "../interface/Simulator.h"

class SingleThreadSimulator : public Simulator{
public:
	SingleThreadSimulator(Network *network, real dt);
	~SingleThreadSimulator();

	using Simulator::run;
	virtual int run(real time, FireInfo &log);
};

#endif /* SINGLETHREADSIMULATOR_H */

