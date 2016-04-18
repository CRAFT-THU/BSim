/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "../../include/BSim.h"
#include "../../src/utils.h"

using namespace std;

int main()
{
	Network c;
	Population<IF_curr_exp> *pn1 = c.createPopulation(nidPool.getID(), 100, IF_curr_exp(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 10.0e-1f));
	Population<IF_curr_exp> *pn2 = c.createPopulation(nidPool.getID(), 100, IF_curr_exp(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0));
	real * weight = getRandomArray((real)5e-3, 100*100);
	real * delay = getConstArray((real)1e-3, 100*100);
	c.connect(pn1, pn2, weight, delay, NULL, 100*100);
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
