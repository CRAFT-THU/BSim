/* This header file is writen by qp09
 * usually just for fun
 * Mon February 22 2016
 */
#ifndef SIMINFO_H
#define SIMINFO_H

#include <string>
#include <vector>
#include <map>

#include "type.h"
//#include "../utils/ID.h"

using std::map;
using std::string;
using std::vector;

struct SimInfo {
	SimInfo(real _dt, int cycle = 0) : currCycle(cycle), dt(_dt) {}
	~SimInfo() {
		fired.clear();
		input.clear();
	}

	int currCycle;
	real dt;
	vector<ID> fired;
	vector<real> input;
};

struct ArrayInfo {
	int size;
	void * data;
};

typedef map<string, ArrayInfo> FireInfo;

int logFireInfo(FireInfo log, string type, string name = "all");
int logFireInfo(FireInfo log, string type, int start, int batch, int size, string name);


#endif /* SIMINFO_H */

