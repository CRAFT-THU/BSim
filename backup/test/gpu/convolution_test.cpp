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

	Network c;

	Population<Constant_spikes> *pn0 = c.createPopulation(0, 9, Constant_spikes(ConstantNeuron(0.5, 0, 0.101), 1.0, 1.0));
	for (int i = 0; i < pn0->getNum(); i++) {
		Constant_spikes * n = static_cast<Constant_spikes*>(pn0->getNeuron(i));
		n->setRate(i/9.0*0.5+0.5);
	}


	Population<Memory> *pn1 = c.createPopulation(1, 9, Memory(MemNeuron(0.0), 1.0, 1.0));

	real * weight0 = NULL;
	real * delay = NULL;

	if (load) { 
		//printf("LOAD DATA...\n");
		weight0 = loadArray("weight0.csv", 4);
		delay = loadArray("delay.csv", 4);
		//printf("LOAD DATA FINISHED\n");
	} else {
		//printf("GENERATE DATA...\n");
		weight0 = new real[4];
		weight0[0] = 0.02;
		weight0[1] = 0.03;
		weight0[2] = 0.04;
		weight0[3] = 0.05;

		delay = getConstArray((real)1e-3, 4);
		//printf("GENERATE DATA FINISHED\n");
	}

	c.connectConv(pn0, pn1, weight0, delay, NULL, 3, 3, 2, 2);

	SGSim sg(&c, 1.0e-3);

	ArrayInfo zero_array = {0, NULL};
	FireInfo fire_info;
	fire_info["count"] = zero_array;
	//fire_info["X"] = zero_array;
	fire_info["Y"] = zero_array;

	sg.run(0.11, fire_info);
	int *rate = (int*) fire_info["count"].data;
	//int *X = (int*) fire_info["X"].data;
	real *Y = (real*) fire_info["Y"].data;

	printf("Input:\n");
	for (int i=0; i<3; i++) {
		printf("\t\t");
		for (int j=0; j<3; j++) {
			printf("%d ", rate[i*3 + j]);
		}
		printf("\n");
	}

	//printf("Input2:\n");
	//for (int i=0; i<3; i++) {
	//	printf("\t\t");
	//	for (int j=0; j<3; j++) {
	//		printf("%d ", X[i*3 + j]);
	//	}
	//	printf("\n");
	//}

	printf("Kernel:\n");
	for (int i=0; i<2; i++) {
		printf("\t\t");
		for (int j=0; j<2; j++) {
			printf("%f ", weight0[i*2 + j]);
		}
		printf("\n");
	}

	printf("Result:\n");
	for (int i=0; i<3; i++) {
		printf("\t\t");
		for (int j=0; j<3; j++) {
			printf("%f ", Y[9 + i*3 + j]);
		}
		printf("\n");
	}

	printf("Check:\n");
	for (int i=0; i<3; i++) {
		printf("\t\t");
		for (int j=0; j<3; j++) {
			real res  = 0;
			for (int k =0; k<2; k++) {
				for (int l = 0; l<2; l++) {
					if (i+k < 3 && j+l < 3) {
						res += rate[(i+k)*3 + j+l] * weight0[k*2 + l];
					}
				}
			}
			printf("%f ", res);
		}
		printf("\n");
	}
	


	//if (!load) {
	//	printf("SAVE DATA...\n");
	//	saveArray("weight0.csv", weight0, 4);
	//	saveArray("delay.csv", delay, 4);
	//	printf("SAVE DATA FINISHED\n");
	//}
	
	return 0;
} 
