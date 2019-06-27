/* This header file is writen by qp09
 * usually just for fun
 * Thu October 22 2015
 */
#ifndef NEURON_H
#define NEURON_H

#include "../interface/Model.h"

class Synapse;

class Neuron : public Model {
public:
	Neuron(int node = 0) : Model(node) {
	}

	virtual ~Neuron() = 0;

	// virtual void monitorOn() {
	// 	monitored = true;
	// }

	// virtual bool isFired() final;
	// virtual int getFireCount() final;
	// virtual real getInputCurrent() final;

	// virtual int fire() final;
	// virtual int recv(real I) = 0;

	// virtual  void setStartExec(int idx) final;
	// virtual  void setStartInhi(int idx) final;
	// virtual  void setEnd(int idx) final;

	virtual Synapse *addSynapse(Synapse *addSynapse) final;
	virtual Synapse *createSynapse(real weight, real delay, SpikeType type, real tau, Neuron *dst) = 0;

	const vector<Synapse*> & getSynapses() const;
protected:
	vector<Synapse*> pSynapses;
	// bool fired;
	// bool monitored;
	// int fire_count;
	// real input_current;
	//int _start_E;
	//int _start_I;
	//int _end;
};

#endif /* NEURON_H */

