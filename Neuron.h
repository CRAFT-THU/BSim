/* This header file is writen by qp09
 * usually just for fun
 * Mon September 28 2015
 */

#ifndef NEURON_H
#define NEURON_H

class Neuron {
public:
	Neuron();
	~Neuron();
protected:
	virtual void presynapsis();
	virtual void integrate() = 0;
	virtual void postsynapsis() = 0;

private:
	int N;
	double *synapsis;
	double axon;
	double threshholdV;
	double resetV;
	double membraneV;
};

#endif /* NEURON_H */

