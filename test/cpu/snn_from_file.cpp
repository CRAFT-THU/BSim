/* This program is writen by qp09.
 * usually just for fun.
 * Mon February 22 2016
 */

#include "../../include/BSim.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	//if (argc != 3) {
	//	printf("Usuage: %s net.json input.json\n", argv[0]);
	//	return -1;
	//}

	Network *net = readNetwork("network.json"/*argv[1]*/);
	int cycles = readInput(net, "input.json" /*argv[2]*/);

	double dt = 1.0e-3f;

	STSim s(net, 1.0e-3f);
	s.run(cycles*dt);

	return 0;
} 



