/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{
	bool load = false;

	if ((argc > 1) && (0==strcmp(argv[1], "load"))) {
		load = true;
	}

	const int N = 3;
	Network c;

	Population<Constant_spikes> *pn0 = c.createPopulation(0, N, Constant_spikes(ConstantNeuron(0.2, 0, 0.101), 1.0, 1.0));
	Constant_spikes * n = static_cast<Constant_spikes*>(pn0->getNeuron(0));
	n->setRate(0.8);

	Population<Constant_spikes> *pn1 = c.createPopulation(1, N, Constant_spikes(ConstantNeuron(0.2, 0.1, 0.201), 1.0, 1.0));
	n = static_cast<Constant_spikes*>(pn1->getNeuron(1));
	n->setRate(0.8);

	Population<Constant_spikes> *pn2 = c.createPopulation(2, N, Constant_spikes(ConstantNeuron(0.2, 0.2, 0.301), 1.0, 1.0));
	n = static_cast<Constant_spikes*>(pn2->getNeuron(2));
	n->setRate(0.8);

	Population<DecisionMaker> *pn3 = c.createPopulation(1, N, DecisionMaker(DecideNeuron(), 1.0, 1.0));

	real * weight0 = NULL;
	real * delay = NULL;

	if (load) { 
		printf("LOAD DATA...\n");
		weight0 = loadArray("weight0.csv", N);
		delay = loadArray("delay.csv", N);
		printf("LOAD DATA FINISHED\n");
	} else {
		printf("GENERATE DATA...\n");
		//real * array = getConstArray(weight_value, num);
		weight0 = getConstArray((real)1.0, N);
		//weight2 = getRandomArray((real)20e-3, N*N);
		delay = getConstArray((real)1e-3, N);
		printf("GENERATE DATA FINISHED\n");
	}

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connectOne2One(pn0, pn3, weight0, delay, NULL, N);
	c.connectOne2One(pn1, pn3, weight0, delay, NULL, N);
	c.connectOne2One(pn2, pn3, weight0, delay, NULL, N);

	//STSim st(&c, 1.0e-3);
	SGSim sg(&c, 1.0e-3);

	sg.run(0.301);
	//st.run(0.1);

	if (!load) {
		printf("SAVE DATA...\n");
		saveArray("weight0.csv", weight0, N);
		saveArray("delay.csv", delay, N);
		printf("SAVE DATA FINISHED\n");
	}
	
	return 0;
} 
