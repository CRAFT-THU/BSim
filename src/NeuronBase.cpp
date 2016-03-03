/* This program is writen by qp09.
 * usually just for fun.
 * Fri December 11 2015
 */

#include <stdio.h>

#include "NeuronBase.h"

NeuronBase::NeuronBase() 
{
	fired = false;
	monitored = false;
}

NeuronBase::~NeuronBase() {}

bool NeuronBase::isFired() {
	return fired;
}

void NeuronBase::monitorOn() {
	monitored = true;
}
