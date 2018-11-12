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
	//createPopulation(int id, int N, LIFENeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)), ID(0, 0), real tau_syn_E, real tau_syn_I);
	Population<LIF_brian> *pn0 = c.createPopulation(N, LIF_brian(LIFENeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 100.0e-1), 1.0, 1.0));
	Population<LIF_brian> *pn1 = c.createPopulation(N, LIF_brian(LIFENeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 0.0e-3), 1.0, 1.0));
	//Population<LIF_brian> *pn2 = c.createPopulation(N, LIF_brian(LIFENeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 0.0e-3), 1.0, 1.0));
	//Population<LIF_brian> *pn3 = c.createPopulation(N, LIF_brian(LIFENeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 0.0e-3), 1.0, 1.0));

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
		//weight0 = new real[N*N];
		//weight1 = new real[N*N];
		//for (int i=0; i<N*N; i++) {
		//	weight0[i] = 1e-6*((i+1));
		//	weight1[i] = 1e-6*((N*N-i));
		//}
		weight0 = getRandomArray((real)1e-5, N*N);
		//weight0 = getConstArray((real)10000e-5, N*N);
		weight1 = getRandomArray((real)2e-5, N*N);
		delay = getConstArray((real)1e-3, N*N);
		printf("GENERATE DATA FINISHED\n");
	}

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connect(pn0, pn1, weight0, delay, NULL, N*N);
	//c.connect(pn1, pn2, weight1, delay, NULL, N*N);
	//c.connect(pn2, pn3, weight1, delay, NULL, N*N);
	STSim st(&c, 1.0e-3);
	SGSim sg(&c, 1.0e-3);
	//sg.compare_run(0.1);
	st.run(0.1);
	sg.run(0.1);

	if (!load) {
		printf("SAVE DATA...\n");
		saveArray("weight0.csv", weight0, N*N);
		saveArray("weight1.csv", weight1, N*N);
		saveArray("delay.csv", delay, N*N);
		printf("SAVE DATA FINISHED\n");
	}
	
	return 0;
} 
