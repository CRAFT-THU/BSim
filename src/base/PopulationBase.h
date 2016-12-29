/* This header file is writen by qp09
 * usually just for fun
 * Mon January 18 2016
 */
#ifndef POPULATIONBASE_H
#define POPULATIONBASE_H

#include "../neuron/NeuronBase.h"

class PopulationBase : public Base {
public:
	PopulationBase();
	virtual ~PopulationBase() = 0;

	virtual void monitorOn();

	virtual int getNum() = 0;
	virtual size_t getSize() = 0;
	virtual NeuronBase* findNeuron(ID id) = 0;

protected:
	bool monitored;
};

#endif /* POPULATIONBASE_H */

