/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "../../include/BSim.h"
#include "../../src/utils.h"

using namespace std;

int main()
{
	const int N = 2;
	Network c;
        //LIFNeuron::LIFNeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	Population<IF_curr_exp> *pn1 = c.createPopulation(nidPool.getID(), N, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 50.0e-4f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 10.0e-1f), ID(0,0)));
	Population<IF_curr_exp> *pn2 = c.createPopulation(nidPool.getID(), N, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 50.0e-4f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0), ID(0, 0)));
	real * weight = getRandomArray((real)20e-3, N*N);
	real * delay = getConstArray((real)1e-3, N*N);
	c.connect(pn1, pn2, weight, delay, NULL, N*N);
	STSim st(&c, 1.0e-3f);
	SGSim sg(&c, 1.0e-3f);
	st.run(0.1f);
	sg.run(0.1f);
	
	FILE *logFile = fopen("weight.csv", "w+");
	if (logFile == NULL) {
		printf("ERROR: Open file weight.csv failed\n");
		return -1;
	}

	for (int i =0; i<10000; i++) {
		fprintf(logFile, "%lf", weight[i]);
		if (i%100 == 99) {
			fprintf(logFile, "\n");
		} else {
			fprintf(logFile, ", ");
		}
	}
	fflush(logFile);
	fclose(logFile);

	return 0;
} 
