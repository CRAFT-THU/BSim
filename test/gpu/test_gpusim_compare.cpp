/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{
	const int N = 900;
	Network c;
	//CreatePopulation(LIFNeuron::LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real v_thresh, real i_offset), real tau_syn_E, real tau_syn_I));
	//Population<Constant_curr> *pn0 = c.createPopulation(0, N, Constant_curr(ConstantNeuron(ID(0, 0), 0.4f), ID(0, 0)));
	Population<IF_curr_exp> *pn0 = c.createPopulation(0, N, IF_curr_exp(LIFNeuron(-74e-3f, -74e-3f, -60e-3f, 0.2e-9f, 10e-3f, 0.0f, -54e-3f, 200.0e-9f), 5e-3f, 100e-3f));

	Population<IF_curr_exp> *pn1 = c.createPopulation(1, N, IF_curr_exp(LIFNeuron(-74e-3f, -74e-3f, -60e-3f, 0.2e-9f, 10e-3f, 0.0f, -54e-3f, 0.0f), 5e-3f, 100e-3f));

	//real * array = getConstArray(weight_value, num);
	real * weight = getConstArray((real)4e-9, N*N);
	real * delay = getConstArray((real)1e-4, N*N);

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connect(pn0, pn1, weight, delay, NULL, N*N);


	// CPU
	//STSim st(&c, 1.0e-4f);
	//st.run(10.0f);
	
	// GPU
	// Simulator(Network, dt)
	SGSim sg(&c, 1.0e-4f);
	// Simulator.run(runtime)
	sg.run(10.0f);

	// Multi GPU
	//MGSim mg(&c, 1.0e-4f);
	//mg.run(1.0f);

	//printf("SAVE DATA...\n");
	//saveArray("weight0.csv", weight, N*N);
	//saveArray("delay.csv", delay, N*N);
	//printf("SAVE DATA FINISHED\n");
	
	return 0;
} 
