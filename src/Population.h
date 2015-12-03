/* This header file is writen by qp09
 * usually just for fun
 * Fri October 23 2015
 */
#ifndef POPULATION_H
#define POPULATION_H

#include "NeuronBase.h"
#include <vector>
using std::vector;

class PopulationBase {
public:
	PopulationBase() {};
	virtual ~PopulationBase() {};
	virtual int update() = 0;
	virtual int monitor() = 0;
	virtual int getNum() = 0;
	virtual int reset(real dt) = 0;
	virtual size_t getSize() = 0;
	virtual size_t hardCopy(unsigned char *data) = 0;
	//virtual vector<NeuronBase*> * getNeurons() = 0;
};

template<class Neuron>
class Population : public PopulationBase {
public:
	Population(int N, Neuron templ);
	~Population();
	int update();
	int monitor();
	int getNum();
	size_t getSize();
	int reset(real dt);
	size_t hardCopy(unsigned char *data);
	NeuronBase* getNeuron(int id);
protected:
	int N;
	int size;
	vector<Neuron> neurons;
};

#endif /* POPULATION_H */

