/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef SIMINFO_H
#define SIMINFO_H

#include <vector>

#include "ID.h"
#include "constant.h"

using std::vector;

struct SimInfo {
	unsigned int currCycle;
	real dt;
	vector<ID> fired;
};

#endif /* SIMINFO_H */

