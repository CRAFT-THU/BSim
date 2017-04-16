/* This program is writen by qp09.
 * usually just for fun.
 * Tue October 13 2015
 */

#include "../../include/BSim.h"

int main(int argc, char **argv)
{
	//LIFENeuron::LIFENeuron(ID id, real v_init, real v_rest, real v_reset, real cm, real tau_m, real tau_refrac, real tau_syn_E, real tau_syn_I, real v_thresh, real i_offset)
	LIF_brian n = LIF_brian(LIFENeuron(0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 2.0e-3f), 0.0f, 0.0f);
	Network c;
	c.createPopulation(0, 1, n);
	SingleThreadSimulator s(&c, 1.0e-3f);
	s.run(0.1f);

	return 0;
} 



