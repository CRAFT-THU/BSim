
#include <stdio.h>

#include "../../include/BSim.h"
#include "../../src/utils/vision.h"
#include "../../src/utils/motor.h"

int * decision(float * fire_rate, int size) 
{

	Network c;
	Population<Constant_spikes> *pn0 = c.createPopulation(0, size, Constant_spikes(ConstantNeuron(0.5, 0, 1.001), 1.0, 1.0));
	for (int i = 0; i < pn0->getNum(); i++) {
		Constant_spikes * n = static_cast<Constant_spikes*>(pn0->getNeuron(i));
		n->setRate(fire_rate[i]);
	}

	Population<Memory> *pn1 = c.createPopulation(1, size, Memory(MemNeuron(0.0), 1.0, 1.0));

	Population<DecisionMaker> *pn2 = c.createPopulation(2, size, DecisionMaker(DecideNeuron(), 1.0, 1.0));

	real *weight0 = getConstArray((real)1.0, size);
	real *delay = getConstArray((real)1e-3, size);

	c.connectOne2One(pn0, pn1, weight0, delay, NULL, size);
	c.connectOne2One(pn1, pn2, weight0, delay, NULL, size);

	ArrayInfo zero_array = {0, NULL};
	FireInfo fire_info;
	fire_info["count"] = zero_array;

	SGSim sg(&c, 1.0e-3);

	sg.run(1.2, fire_info);

	int *rate = (int*) fire_info["count"].data;

	//printf("Result1:\t");
	//for (int i=0; i<size; i++) {
	//	printf("%d ", rate[i]);
	//}
	//printf("\n");

	//printf("Result2:\t");
	//for (int i=0; i<size; i++) {
	//	printf("%d ", rate[size + i]);
	//}
	//printf("\n");

	//printf("Result3:\t");
	//for (int i=0; i<size; i++) {
	//	printf("%d ", rate[size*2 + i]);
	//}
	//printf("\n");

	return rate + fire_info["count"].size - 10;
}

int main(int argc, char **argv)
{
	float * tmp = NULL;
	int size = 0;

	size = vision(&tmp, argc, argv);

	int * fire_rate = decision(tmp, size);

	motion(fire_rate, size);

	return 0;
}
