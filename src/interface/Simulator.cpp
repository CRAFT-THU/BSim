
#include "Simulator.h"

GNetwork * Simulator::getNet()
{
	SimInfo info(_dt);
	reset();
    return _network->buildNetwork(info);
}

int Simulator::reset()
{
	SimInfo info(_dt);
	_network->reset(info);

	return 0;
}

int Simulator::run (real time) {
	FireInfo fire_info;
	ArrayInfo zero_array = {0, NULL};
	fire_info["count"] = zero_array;
	int ret = run(time, fire_info);
	logFireInfo(fire_info, "count", "fire");

	return ret;
}

