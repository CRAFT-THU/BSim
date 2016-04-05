/* This program is writen by qp09.
 * usually just for fun.
 * Fri October 23 2015
 */

#include <math.h>

#include "../utils/json/json.h"
#include "NengoLowpass.h"
#include "../constant.h"

const Type NengoLowpass::type = Lowpass;

NengoLowpass::NengoLowpass(ID id, int src_size, int dst_size, real *weight, real tau_syn, real *fired, real *i_syn)
{
	this->id = id;
	this->weight = weight;
	this->tau_syn = tau_syn;
	this->src_size = src_size;
	this->dst_size = dst_size;
	this->fired = fired;
	this->output = new real[dst_size];
	this->i_syn = i_syn;
	file = NULL;
}

NengoLowpass::~NengoLowpass()
{
	delete[] weight;
	delete[] output;
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}

}

int NengoLowpass::reset(SimInfo &info) {
	for (int i=0; i<dst_size; i++) {
		output[i] = 0;
	}
	init(info.dt);

	return 0;
}

int NengoLowpass::init(real dt) {
	_dt = dt;
	if (tau_syn > 0.008) {
		C1 = -0.90483742;
		_C1 = 0.09516258;
	} else {
		C1 = -0.81873075307798182;
		_C1 = 0.18126924692201818;
	}

	return 0;
}

int NengoLowpass::update(SimInfo &info)
{
	
	list<int>::iterator iter;

	if (file == NULL) {
		char fname[128];
		sprintf(fname, "Synapse_%d_%d.log", id.groupId, id.id);
		file = fopen(fname, "w+");
	}

	for (int i=0; i<dst_size; i++) {
		real i_tmp = 0;
		for (int j=0; j<src_size; j++) {
			i_tmp += (weight[i*src_size+j] * fired[j]);
		}
		fprintf(file, "%lf ", i_tmp); 
		output[i] = -C1 * output[i];
		output[i] += i_tmp*_C1;
		i_syn[i] += output[i];
	}
	fprintf(file, "\n");
	for (int i=0; i<dst_size; i++) {
		fprintf(file, "%lf ", output[i]); 
	}
	fprintf(file, "\n");

	return 0;
}


ID NengoLowpass::getID()
{
	return id;
}

Type NengoLowpass::getType()
{
	return type;
}

int NengoLowpass::getSize()
{
	return dst_size;
}
