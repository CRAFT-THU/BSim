/* This program is writen by qp09.
 * usually just for fun.
 * Mon November 23 2015
 */

#include <iostream>

#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{
	Network *net = readNetwork("random_net.json");
	writeNetwork("write.json", net);

	return 0;
} 



