/* This program is writen by qp09.
 * usually just for fun.
 * Mon January 18 2016
 */

#include <iostream>

#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{

	Network c;
	Population<IF_curr_exp> *popu =  c.createPopulation(ID(0), 10, IF_curr_exp(ID(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
	return 0;
} 



