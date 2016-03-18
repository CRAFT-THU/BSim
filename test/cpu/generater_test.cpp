/* This program is writen by qp09.
 * usually just for fun.
 * Mon January 18 2016
 */

#include "../../include/BSim.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	//void random(int populationNum, double connectionRate, double exciteRate, int minSize, double minWeight, double minDelay, int sizeRange = 0, double weightRange = 0, double delayRange = 0);
	balanced(2, 0.8, 0.8, 2, 4.0e-3, 1,  10, 1.0e-3, 2);
	unbalanced(2, 0.8, 0.8, 2, 4.0e-3, 1, 10, 1.0e-3, 2);
	return 0;
} 



