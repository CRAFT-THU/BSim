/* This header file is writen by qp09
 * usually just for fun
 * Wed January 06 2016
 */
#ifndef INPUTPOPULATION_H
#define INPUTPOPULATION_H

#include <stdio.h>
#include <vector>
#include <deque>

#include "../utils/json/json.h"
#include "NengoBase.h"

using std::vector;
using std::deque;

class InputPopulation : public NengoBase {
public:
	InputPopulation(Json::Value input);
	~InputPopulation();

	virtual ID getID();
	virtual int getSize();
	virtual Type getType();


	virtual int reset(SimInfo &info);
	virtual int update(SimInfo &info);

	virtual real* getOutput();
	virtual real* getInput();

	int addFireTime(int idx, int cycle);

	const static Type type;
protected:
	vector<deque<int>> fireTime;
	real *fire_array;
	real _dt;
	ID id;
	int size;
	FILE *file;
};

#endif /* INPUTPOPULATION_H */

