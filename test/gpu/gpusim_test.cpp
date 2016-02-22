/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "../../include/BSim.h"

using namespace std;

int main()
{
	Network c;
	IF_curr_exp *pn1 = c.create(IF_curr_exp(nidPool.getID(), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 2.0e-3f));
	IF_curr_exp *pn2 = c.create(IF_curr_exp(nidPool.getID(), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0));
	c.connect(pn1, pn2, 5.0e-4, 1e-3, Excitatory);
	SGSim s(&c, 1.0e-3f);
	s.run(0.1f);

	return 0;
} 
