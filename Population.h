/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef POPULATION_H
#define POPULATION_H
template<class Neuron>
class Population {
public:
	Population(int N, Neuron templ);
	~Population();
protected:
	int N;
	vector<Neuron> neurons;
}

#endif /* POPULATION_H */

