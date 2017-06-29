
#include "../../include/BSim.h"

int main(int argc, char **argv)
{
	int array[12] = { 0, 1, 5, 13, 20, 50, 69, 71, 82, 85, 90, 99};
	Array_curr n = Array_curr(ArrayNeuron(array, 12));
	Network c;
	c.createPopulation(0, 1, n);
	STSim st(&c, 1.0e-3);
	st.run(0.1);

	return 0;
} 
