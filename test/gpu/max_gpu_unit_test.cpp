
#include "../../include/BSim.h"

int main(int argc, char **argv)
{
	int array[12] = { 0, 1, 5, 13, 20, 50, 69, 71, 82, 85, 90, 99};
	int array2[10] = { 2, 5, 12, 21, 30, 60, 71, 72, 80, 95};
	int array3[30] = { 0, 1, 5, 10, 13, 17, 20, 26, 29, 30, 35, 37, 40, 44, 49, 50, 55, 56, 62, 65, 69, 71, 75, 79, 82, 85, 86, 90, 96, 99};
	Network c;
	Array_spikes n1 = Array_spikes(ArrayNeuron(array, 12));
	Array_spikes n2 = Array_spikes(ArrayNeuron(array2, 10));
	Array_spikes n3 = Array_spikes(ArrayNeuron(array3, 30));
	Max_pooling max1 = Max_pooling(MaxNeuron(3),  0, 0);
	Population<Array_spikes> *p1 = c.createPopulation(0, 1, n1);
	Population<Array_spikes> *p2 = c.createPopulation(1, 1, n2);
	Population<Array_spikes> *p3 = c.createPopulation(2, 1, n3);
	
	Population<Max_pooling> *p4 = c.createPopulation(3, 1, max1);
	c.connect(p1, p4, (real)(1), 1.0e-3, Excitatory);
	c.connect(p2, p4, (real)(1<<1), 1.0e-3, Excitatory);
	c.connect(p3, p4, (real)(1<<2), 1.0e-3, Excitatory);

	SGSim sg(&c, 1.0e-3);
	sg.run(0.1);
	STSim st(&c, 1.0e-3);
	st.run(0.1);

	return 0;
} 
