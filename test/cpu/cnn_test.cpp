
#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{

	Network c;
	Population<Constant_spikes> *pn0 = c.createPopulation(0, 36, Constant_spikes(ConstantNeuron(0.5), 1.0, 1.0));
	Population<LIF_brian> *pn1 = c.createPopulation(1, 36, LIF_brian(LIFENeuron(0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 0.0e-3), 1.0, 1.0));

	real * weight0 = NULL;
	real * delay = NULL;

	//real * array = getConstArray(weight_value, num);
	weight0 = getConstArray((real)20e-4, 9);
	//weight2 = getRandomArray((real)20e-3, N*N);
	delay = getConstArray((real)1e-3, 9);

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connectConv(pn0, pn1, weight0, delay, NULL, 6, 6, 3, 3);
	//c.connect(pn0, pn1, weight0, delay, NULL, 6);

	STSim st(&c, 1.0e-3);
	st.run(0.01);

	//SGSim sg(&c, 1.0e-3);
	//sg.run(0.1);

	return 0;
} 
