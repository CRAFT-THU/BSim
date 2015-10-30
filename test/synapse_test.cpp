/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 30 2015
 */

#include <iostream>
#include "../Simulator.h"

using namespace std;

int main(int argc, char **argv)
{
	//LIFNeuron::LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	IF_curr_exp n1(0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 2.0e-3f);
	IF_curr_exp n2(0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 2.0e-3f);
	Connector c;
	c.connect(&n1, &n2, 5.0e-4, 1e-3, Excitatory);
	Simulator s(&c, 1.0e-3f);
	s.run(0.1f);

	return 0;
} 



