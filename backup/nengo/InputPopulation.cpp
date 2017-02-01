/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "InputPopulation.h"

const Type InputPopulation::type = Input;

InputPopulation::InputPopulation(Json::Value input)
{
	file = NULL;
	id.groupId = input["id"].asInt();
	id.id = -1;
	size = input["neurons"].size();
	fireTime.resize(size);
	fire_array = new real[size];

}

InputPopulation::~InputPopulation()
{
	delete[] fire_array;
	for (int i=0; i<size; i++) {
		fireTime[i].clear();
	}
	fireTime.clear();
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int InputPopulation::reset(SimInfo &info)
{
	std::fill_n(fire_array, size, 0);
	_dt = info.dt;
	
	return 0;
}

ID InputPopulation::getID()
{
	return id;
}

int InputPopulation::getSize()
{
	return size;
}

Type InputPopulation::getType()
{
	return type;
}

int InputPopulation::update(SimInfo &info)
{
	for (int i=0; i<size; i++) {
		fire_array[i] = 0.0;
		if (!fireTime[i].empty()) {
			while (fireTime[i].front() <= info.currCycle) {
				info.fired.push_back(ID(this->id.groupId, i));
				fire_array[i] = 1.0/_dt;
				fireTime[i].pop_front();
				if (fireTime[i].empty()) {
					break;
				}
			}
		}
	}
	return 0;
}

int InputPopulation::addFireTime(int idx, int cycle)
{
	if (idx >= size) {
		printf("Neuron ID out of range\n");
	}
	fireTime[idx].push_back(cycle);
	return 0;
}


real* InputPopulation::getOutput()
{
	return fire_array;
}

real* InputPopulation::getInput()
{
	return NULL;
}
