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

	const int N = 2;
	Network c;
	int array[30] = { 0, 1, 5, 10, 13, 17, 20, 26, 29, 30, 35, 37, 40, 44, 49, 50, 55, 56, 62, 65, 69, 71, 75, 79, 82, 85, 86, 90, 96, 99};
	Population<Array_curr> *pn0 = c.createPopulation(0, N, Array_curr(ArrayNeuron(ID(0, 0), array, 30), ID(0, 0)));
	Population<IF_curr_exp> *pn1 = c.createPopulation(1, N, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 50.0e-4f, 0.0f, 15.0e-3f, 0), ID(0,0), 1.0f, 1.0f));
	Population<IF_curr_exp> *pn2 = c.createPopulation(2, N, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 50.0e-4f, 0.0f, 15.0e-3f, 0), ID(0, 0), 1.0f, 1.0f));

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
	STSim st(&c, 1.0e-3f);
	SGSim sg(&c, 1.0e-3f);
	st.run(0.1f);
	sg.compare_run(0.1f);
	sg.run(0.1f);

	if (!load) {
		printf("SAVE DATA...\n");
		saveArray("weight0.csv", weight0, N*N);
		saveArray("weight1.csv", weight1, N*N);
		saveArray("delay.csv", delay, N*N);
		printf("SAVE DATA FINISHED\n");
	}
	
	return 0;
} 
