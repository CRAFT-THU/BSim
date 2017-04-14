/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "ArrayNeuron.h"
#include "GArrayNeurons.h"

const Type ArrayNeuron::type = Array;

ArrayNeuron::ArrayNeuron(ID id) : NeuronBase(id)
{
	file = NULL;
	fired = false;
	monitored = false;
	idx = 0;
}

ArrayNeuron::ArrayNeuron(ID id, int *array, int num) : NeuronBase(id)
{
	file = NULL;
	fired = false;
	monitored = false;
	idx = 0;
	fireTime.assign(array, array+num);
}

ArrayNeuron::ArrayNeuron(const ArrayNeuron &templ, ID id) : NeuronBase(id)
{
	file = NULL;
	fired = false;
	monitored = templ.monitored;
	idx = 0;
	fireTime.assign(templ.fireTime.begin(), templ.fireTime.end());
}

ArrayNeuron::~ArrayNeuron()
{
	fireTime.clear();
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int ArrayNeuron::reset(SimInfo &info)
{
	fired = false;
	idx = 0;
	return 0;
}

int ArrayNeuron::recv(real I)
{
	return 0;
}

Type ArrayNeuron::getType()
{
	return type;
}

int ArrayNeuron::update(SimInfo &info)
{
	fired = false;
	if (idx < (int)fireTime.size()) {
		if (fireTime[idx] <= info.currCycle) {
			fired = true;
			fire();
			info.fired.push_back(getID());
			idx++;
		}
	}
	return 0;
}

void ArrayNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			int size = fireTime.size();
			if (size > 0) {
				char filename[128];
				sprintf(filename, "ArrayNeuron_%s.log", getID().getInfo().c_str());
				file = fopen(filename, "w+");
				if (file == NULL) {
					printf("Open File: %s failed\n", filename);
					return;
				}
				fprintf(file, "Size: %d\n", size);
				for (int i=0; i<size; i++) {
					fprintf(file, "%d\n", fireTime[i]);
				}
			}
		}
	}
	return;
}

int ArrayNeuron::addFireTime(int cycle)
{
	fireTime.push_back(cycle);
	return 0;
}

vector<int> & ArrayNeuron::getFireTimes()
{
	return fireTime;
}

size_t ArrayNeuron::getSize() 
{
	return sizeof(GArrayNeurons);
}

int ArrayNeuron::getData(void *data)
{
	return 0;
}

int ArrayNeuron::hardCopy(void *data, int idx, int base)
{
	GArrayNeurons *p = (GArrayNeurons*) data;
	setID(idx+base);

	p->p_start[idx] = idx;
	p->p_end[idx] = fireTime.size();

	return 1;
}
