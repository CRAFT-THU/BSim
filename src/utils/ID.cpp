/* This program is writen by qp09.
 * usually just for fun.
 * Wed October 28 2015
 */

#include "ID.h"
#include "NeuronBase.h"
#include "SynapseBase.h"

ID::ID() {
	id = 0;
}

ID::~ID() {
}

int ID::getID() {
	return id++;
}
