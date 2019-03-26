/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <string.h>

#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{
	bool load = false;

	if ((argc > 1) && (0==strcmp(argv[1], "load"))) {
		load = true;
	}

	const int N = 2;
	Network c;
	Population<Constant_spikes> *pn0 = c.createPopulation(0, N, Constant_spikes(ConstantNeuron(0.4) ));
	Population<LIF_curr_exp> *pn1 = c.createPopulation(1, N, LIF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 1.0, 1.0, 15.0e-3, 10.0e-1), 1.0, 1.0));
	Population<LIF_curr_exp> *pn2 = c.createPopulation(2, N, LIF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 1.0, 1.0, 15.0e-3, 0), 1.0, 1.0));

	real * weight0 = NULL;
	real * weight1 = NULL;
	real * delay = NULL;

	if (load) { 
		printf("LOAD DATA...\n");
		weight0 = loadArray("weight0.csv", N*N);
		weight1 = loadArray("weight1.csv", N*N);
		delay = loadArray("delay.csv", N*N);
		printf("LOAD DATA FINISHED\n");
	} else {
		printf("GENERATE DATA...\n");
		weight0 = getRandomArray((real)20e-3, N*N);
		weight1 = getRandomArray((real)20e-3, N*N);
		delay = getConstArray((real)1e-3, N*N);
		printf("GENERATE DATA FINISHED\n");
	}

	c.connect(pn0, pn1, weight0, delay, NULL, N*N);
	c.connect(pn1, pn2, weight1, delay, NULL, N*N);
	STSim st(&c, 1.0e-3);
	SGSim sg(&c, 1.0e-3);
	MGSim mg(&c, 1.0e-3);
	st.run(0.1);
	sg.run(0.1);
	mg.run(0.1);

	if (!load) {
		printf("SAVE DATA...\n");
		saveArray("weight0.csv", weight0, N*N);
		saveArray("weight1.csv", weight1, N*N);
		saveArray("delay.csv", delay, N*N);
		printf("SAVE DATA FINISHED\n");
	}
	
	return 0;
} 
