/* This header file is writen by qp09
 * usually just for fun
 * Tue September 29 2015
 */
#ifndef PROBENEURON_H
#define PROBENEURON_H

#include <stdio.h>

#include "NeuronBase.h"

class ProbeNeuron : public NeuronBase {
public:
	ProbeNeuron(ID id,  real tau_syn_E, real tau_syn_I);
	ProbeNeuron(const ProbeNeuron &neuron, ID id);
	~ProbeNeuron();

	ID getID();

	virtual real get_vm();
	virtual int init(real dt);

	virtual int fire();
	virtual int recv(real I);

	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);
	virtual void monitor(SimInfo &info);

	virtual SynapseBase *addSynapse(real weight, real delay, SpikeType type, NeuronBase *pDest);

	virtual size_t getSize();
	virtual int getData(void *data);
	virtual int hardCopy(void * data, int idx);
protected:
	ID id;
	NeuronType type;
	real _dt;
	real tau_syn_E;
	real tau_syn_I;
	real i_syn;
	real i_tmp;
	FILE* file;
};

#endif /* PROBENEURON_H */

