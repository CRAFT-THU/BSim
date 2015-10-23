/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef NEURONBASE_H
#define NEURONBASE_H

#include "constant.h"

class NeuronBase {
	virtual void reset() = 0;
	virtual void update() = 0;
};

#endif /* NEURONBASE_H */

