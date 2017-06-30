
#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{

	Network c;
	Population<Constant_spikes> *pn0 = c.createPopulation(0, 16, Constant_spikes(ConstantNeuron(0.5), 1.0, 1.0));
	for (int i = 0; i < pn0->getNum(); i++) {
		Constant_spikes * n = static_cast<Constant_spikes*>(pn0->getNeuron(i));
		n->setRate(i/36.0 * 0.7);
	}

	Population<Max_pooling> *pn1 = c.createPopulation(1, 4, Max_pooling(MaxNeuron(4), 1.0, 1.0));

	c.connectPooling(pn0, pn1, 4, 4, 2, 2);

	STSim st(&c, 1.0e-3);
	st.run(0.2);

	//SGSim sg(&c, 1.0e-3);
	//sg.run(0.1);

	return 0;
} 
