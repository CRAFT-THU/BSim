/* This program is writen by qp09.
 * usually just for fun.
 * Tue October 13 2015
 */

#include "../../include/BSim.h"

int main(int argc, char **argv)
{
	//LIFNeuron::LIFNeuron(real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	IF_curr_exp n = IF_curr_exp(LIFNeuron(0.0, 0.0, 0.0, 1.0e-1, 10.0e-3, 0.0, 15.0e-3, 2.0e-3), 1.0, 1.0);
	Network c;
	c.createPopulation(0, 1, n);
	SingleThreadSimulator s(&c, 1.0e-3);
	s.run(0.1);

	return 0;
} 



