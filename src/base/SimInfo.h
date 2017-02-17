/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef SIMINFO_H
#define SIMINFO_H

#include <vector>

#include "type.h"
#include "../utils/ID.h"

using std::vector;

struct SimInfo {
	int currCycle;
	real dt;
	vector<ID> fired;
	vector<real> input;
};

#endif /* SIMINFO_H */

