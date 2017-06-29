
#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{

	Network c;
	Population<Poisson_spikes> *pn0 = c.createPopulation(0, 16, Poisson_spikes(PoissonNeuron(0.7, 2, 0.0, 0.1), 1.0, 1.0));
	Population<Max_pooling> *pn1 = c.createPopulation(1, 4, Max_pooling(MaxNeuron(4), 1.0, 1.0));

	c.connectPooling(pn0, pn1, 4, 4, 2, 2);

	STSim st(&c, 1.0e-3);
	st.run(0.15);

	//SGSim sg(&c, 1.0e-3);
	//sg.run(0.1);

	return 0;
} 
