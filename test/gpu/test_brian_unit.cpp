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
	//createPopulation(int id, int N, LIFENeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)), ID(0, 0), real tau_syn_E, real tau_syn_I);
	Population<LIF_brian> *pn0 = c.createPopulation(0, N, LIF_brian(LIFENeuron(0.0f, 0.0f, 0.0f, 1.0e-1f, 50.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 10.0e-1f), 1.0f, 1.0f));
	Population<LIF_brian> *pn1 = c.createPopulation(1, N, LIF_brian(LIFENeuron(0.0f, 0.0f, 0.0f, 1.0e-1f, 50.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0.0e-3f), 1.0f, 1.0f));

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
		//real * array = getConstArray(weight_value, num);
		weight0 = getConstArray((real)20e-3, N*N);
		weight1 = getConstArray((real)20e-3, N*N);
		delay = getConstArray((real)1e-3, N*N);
		printf("GENERATE DATA FINISHED\n");
	}

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connect(pn0, pn1, weight0, delay, NULL, N*N);
	STSim st(&c, 1.0e-3f);
	SGSim sg(&c, 1.0e-3f);
	st.run(0.1f);
	//sg.compare_run(0.1f);
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
