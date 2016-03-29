/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "NengoProbe.h"
//#include "GNengoProbes.h"

using std::vector;

const Type NengoProbe::type = Probe;

NengoProbe::NengoProbe(ID id, int size, real *weight, real *fire_array)
{
	this->id = id;
	this->file = NULL;
	this->size = size;
	this->weights = weight;
	this->probes= fire_array;
}

NengoProbe::~NengoProbe()
{
	delete[] weights;
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

int NengoProbe::init(real dt)
{
	_dt = dt;
	
	return 0;
}

int NengoProbe::update(SimInfo &info)
{
	if (file == NULL) {
		char filename[128];
		memset(filename, 0, 128);
		sprintf(filename, "Probe_%d_%d.log", id.groupId, id.id);
		file = fopen(filename, "w+");

		if (file == NULL) {
			printf("Open File: %s failed\n", filename);
			return 0;
		}

	}
	real w_t = 0;
	for (int i=0; i<size; i++) {
		w_t += weights[i] * probes[i];
	}

	real C1 = -0.90483742;
	real _C1 = 0.09516258;
	vm =  -C1 * vm + w_t * _C1;
	fprintf(file, " %f\n", vm); 

	return 0;
}


int NengoProbe::reset(SimInfo &info)
{
	return init(info.dt);
}

ID NengoProbe::getID()
{
	return id;
}

Type NengoProbe::getType()
{
	return type;
}

int NengoProbe::getSize()
{
	return size;
}

real* NengoProbe::getOutput()
{
	return NULL;
}

real* NengoProbe::getInput()
{
	return NULL;
}
