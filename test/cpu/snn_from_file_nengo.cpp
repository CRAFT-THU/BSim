/* This program is writen by qp09.
 * usually just for fun.
 * Mon February 22 2016
 */

#include "../../src/nengo/NengoReader.h"
#include "../../src/nengo/SingleNengoSimulator.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
	//if (argc != 3) {
	//	printf("Usuage: %s net.json input.json\n", argv[0]);
	//	return -1;
	//}

	NengoNetwork *net = readNengoNetwork("network.json"/*argv[1]*/);
	int cycles = readNengoInput(net, "input.json" /*argv[2]*/);

	double dt = 1.0e-3f;

	SingleNengoSimulator s(net, dt);
	s.run(cycles * dt);

	return 0;
} 



