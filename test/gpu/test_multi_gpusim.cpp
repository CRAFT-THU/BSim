/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{
	const int N = 500;
	Network c;
	Population<Constant_curr> *pn0 = c.createPopulation(0, N, Constant_curr(ConstantNeuron(0.4)));
	Population<IF_curr_exp> *pn1 = c.createPopulation(1, N, IF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 15.0e-3, 10.0e-1), 1.0, 1.0));
	Population<IF_curr_exp> *pn2 = c.createPopulation(2, N, IF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 15.0e-3, 0), 1.0, 1.0));
	Population<IF_curr_exp> *pn3 = c.createPopulation(3, N, IF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 15.0e-3, 0), 1.0, 1.0));
	Population<IF_curr_exp> *pn4 = c.createPopulation(4, N, IF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 15.0e-3, 0), 1.0, 1.0));
	Population<IF_curr_exp> *pn5 = c.createPopulation(5, N, IF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-4, 0.0, 15.0e-3, 0), 1.0, 1.0));

	real * weight = getRandomArray((real)20e-3, N*N);
	real * weight2 = getRandomArray((real)15e-3, N*N);
	real * delay = getConstArray((real)1e-3, N*N);
	c.connect(pn0, pn1, weight, delay, NULL, N*N);
	c.connect(pn1, pn2, weight, delay, NULL, N*N);
	c.connect(pn2, pn3, weight2, delay, NULL, N*N);
	c.connect(pn3, pn4, weight2, delay, NULL, N*N);
	c.connect(pn4, pn5, weight2, delay, NULL, N*N);

	STSim st(&c, 1.0e-3);
	SGSim sg(&c, 1.0e-3);
	MGSim mg(&c, 1.0e-3);
	st.run(0.1);
	sg.run(0.1);
	mg.run(0.1);

	printf("SAVE DATA...\n");
	saveArray("weight0.csv", weight, N*N);
	saveArray("weight1.csv", weight2, N*N);
	saveArray("delay.csv", delay, N*N);
	printf("SAVE DATA FINISHED\n");
	
	return 0;
} 
