/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 30 2015
 */

#include "../../include/BSim.h"

using namespace std;

int main()
{
	Network c;
	Population<IF_curr_exp> *pn1 = c.createPopulation(0, 1, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 2.0e-3f), ID(0, nidTag.getTag())));
	Population<IF_curr_exp> *pn2 = c.createPopulation(1, 1, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0), ID(0, nidTag.getTag())));

	c.connect(pn1, pn2, 5.0e-4f, 1.0e-3f, Excitatory);
	STSim s(&c, 1.0e-3f);
	s.run(0.1f);

	return 0;
} 



