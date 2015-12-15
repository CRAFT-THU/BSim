/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 30 2015
 */

#include <iostream>
#include "../include/SNNsim.h"

using namespace std;

int main(int argc, char **argv)
{
	Network c;
	IF_curr_exp *pn1 = c.create(IF_curr_exp(nidPool.getID(), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 2.0e-3f));
	IF_curr_exp *pn2 = c.create(IF_curr_exp(nidPool.getID(), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0));
	c.connect(pn1, pn2, 5.0e-4, 1e-3, Excitatory);
	STSim s(&c, 1.0e-3f);
	s.run(0.1f);

	return 0;
} 



