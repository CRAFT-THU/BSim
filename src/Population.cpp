/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "Population.h"

template<class Neuron>
Population<Neuron>::Population(int N, Neuron templ) 
{
	if (!neurons.empty()) {
		neurons.clear();
	}
	for (int i=0; i<N; i++) {
		neurons.push_back(templ);
	}
}

template<class Neuron>
Population<Neuron>::~Population() 
{
	neurons.clear();
}

template<class Neuron>
int Population<Neuron>::getSize()
{
	return N;
}

template<class Neuron>
NeuronBase* Population<Neuron>::getNeuron(int id)
{
	return &neurons[id];
}

template<class Neuron>
int Population<Neuron>::update()
{
	int ret = 0;
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		Neuron *p = iter;
		ret += p->update();
	}
	return ret;
}

template<class Neuron>
int Population<Neuron>::reset(real dt)
{
	int ret = 0;
	typename vector<Neuron>::iterator iter;
	for (iter = neurons.begin(); iter != neurons.end(); iter++) {
		Neuron *p = iter;
		ret += p->reset();
	}
	return ret;
}

//template<class Neuron>
//vector<NeuronBase*>* Population<Neuron>::getNeurons()
//{
//	return &pNeurons;
//}
