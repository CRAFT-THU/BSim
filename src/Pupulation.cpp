/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include "Population.h"

template<class Neuron>
Population<Neuron>::Population(int N, Neuron templ) {
}

template<class Neuron>
Population<Neuron>::~Population() {
	neurons.clear();
}

