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

	const int N = 500;
	Network c;

	Population<Constant_curr_exp> *pn0 = c.createPopulation(0, 784, Constant_curr_exp(ConstantNeuron(256.0/1000.0), 1.0, 1.0));
	Population<TJ_curr_static> *pn1 = c.createPopulation(1, 128, TJ_curr_static(TJNeuron(0.0, 0.0, 1.0e-1, 0.0, 15.0e-3, 0.0e-3), 1.0, 1.0));
	Population<TJ_curr_static> *pn2 = c.createPopulation(2, 32, TJ_curr_static(TJNeuron(0.0, 0.0, 1.0e-1, 0.0, 15.0e-3, 0.0e-3), 1.0, 1.0));
	Population<TJ_curr_static> *pn3 = c.createPopulation(3, 10, TJ_curr_static(TJNeuron(0.0, 0.0, 1.0e-1, 0.0, 15.0e-3, 0.0e-3), 1.0, 1.0));

	real * weight0 = NULL;
	real * weight1 = NULL;
	real * weight2 = NULL;
	real * delay = NULL;

	if (load) { 
		printf("LOAD DATA...\n");
		weight0 = loadArray("weight0.csv", 784*128);
		weight1 = loadArray("weight1.csv", 128*32);
		weight2 = loadArray("weight2.csv", 32*10);
		delay = loadArray("delay.csv", N*N);
		printf("LOAD DATA FINISHED\n");
	} else {
		printf("GENERATE DATA...\n");
		//real * array = getConstArray(weight_value, num);
		weight0 = getRandomArray((real)20e-4, 784*128);
		weight1 = getRandomArray((real)20e-4, 128*32);
		weight2 = getRandomArray((real)20e-3, 32*10);
		delay = getConstArray((real)1e-3, N*N);
		printf("GENERATE DATA FINISHED\n");
	}

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connect(pn0, pn1, weight0, delay, NULL, 784*128);
	c.connect(pn1, pn2, weight1, delay, NULL, 128*32);
	c.connect(pn2, pn3, weight2, delay, NULL, 32*10);
	STSim st(&c, 1.0e-3);
	SGSim sg(&c, 1.0e-3);
	sg.run(0.1);
	st.run(0.1);

	if (!load) {
		printf("SAVE DATA...\n");
		saveArray("weight0.csv", weight0, 784*128);
		saveArray("weight1.csv", weight1, 128*32);
		saveArray("weight2.csv", weight1, 32*10);
		saveArray("delay.csv", delay, N*N);
		printf("SAVE DATA FINISHED\n");
	}
	
	return 0;
} 
