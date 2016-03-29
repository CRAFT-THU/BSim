/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "../utils.h"
#include "NengoPopulation.h"

using std::vector;

const Type NengoPopulation::type = Nengo;

NengoPopulation::NengoPopulation(Json::Value population)
{
	size = population["size"].asInt();
	id.groupId = population["id"].asInt();
	id.id = -1;
	v_init = new real[size];
	v_min = new real[size];
	v_reset = new real[size];
	tau_m = new real[size];
	tau_refrac = new real[size];
	v_thresh = new real[size];
	i_offset = new real[size];
	vm = new real[size];
	C1 = new real[size];
	refrac_time = new real[size];
	fire_array = new real[size];

	Json::Value para = population["parameters"];
	dim = para["scaled_encoders"][0].size();
	encoder = new real[dim*size];
	i_syn = new real[size];
	
	for (int i=0; i<size; i++) {
		v_init[i] = testValue(para["voltage"], i).asDouble();
	       	v_min[i] = testValue(para["min_voltage"], i).asDouble();
	       	v_reset[i] = testValue(para["reset"], i).asDouble();
		tau_m[i] = testValue(para["tau_rc"], i).asDouble();
	       	tau_refrac[i] = testValue(para["tau_ref"], i).asDouble();
		v_thresh[i] = testValue(para["threshold"], i).asDouble();
	       	i_offset[i] = testValue(para["bias"], i).asDouble();
		fire_array[i] = 0;

		for (int j=0; j<dim; j++) {
			encoder[j*size+i] = testValue(para["scaled_encoders"], i)[j].asDouble();
		}
		i_syn[i] = 0;
	}

}

NengoPopulation::~NengoPopulation()
{
	delete[] v_init;
	delete[] v_min;
	delete[] v_reset;
	delete[] tau_m;
	delete[] tau_refrac;
	delete[] v_thresh;
	delete[] i_offset;
	delete[] encoder;
	delete[] i_syn;
	delete[] vm;
	delete[] C1;
	delete[] refrac_time;

	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int NengoPopulation::init(real dt)
{
	_dt = dt;
	for (int i=0; i<size; i++) {
		if (tau_m[i] > 0) {
			C1[i] = -expm1(-dt/tau_m[i]);
		} else {
			C1[i] = 1;
		}
		refrac_time[i] = 0;
	}

	return 0;
}

int NengoPopulation::update(SimInfo &info)
{
	if (file == NULL) {
		char fname[128];
		sprintf(fname, "Population_%d_%d.log", id.groupId, id.id);
		file = fopen(fname, "w+");
	}
	for (int i=0; i<size; i++) {
		fire_array[i] = 0.0;
		real I = i_offset[i] + i_syn[i]; // * encoder[size*j+i];
		i_syn[i] = 0;
		real dv = C1[i] * (I - vm[i]);
		vm[i] = vm[i] + dv;

		if (vm[i] < v_min[i]) {
			vm[i] = v_min[i];
		}

		refrac_time[i] -= _dt;

		real tmp = 1-refrac_time[i]/_dt;
		if (tmp < 0) {
			tmp = 0;
		}
		if (tmp > 1) {
			tmp = 1;
		}

		vm[i] = vm[i] * tmp;

		fprintf(file, "%lf ", vm[i]);

		if (vm[i] >= v_thresh[i]) {
			real overshoot = (vm[i] -1)/dv;
			real spiketime = _dt*(1-overshoot);
			fire_array[i] = 1.0/_dt;
			refrac_time[i] = tau_refrac[i] + spiketime;
			vm[i] = 0;
		}

		if (fire_array[i] > 0) {
			info.fired.push_back(ID(this->id.groupId, i));
		}
	}

	fprintf(file, "\n");
	return 0;
}

int NengoPopulation::reset(SimInfo &info)
{
	for (int i=0; i<size; i++) {
		vm[i] = v_init[i];
		refrac_time[i] = 0;
		i_syn[i] = 0;
	}
	return init(info.dt);
}

ID NengoPopulation::getID()
{
	return id;
}

int NengoPopulation::getSize()
{
	return size;
}

Type NengoPopulation::getType()
{
	return type;
}

real* NengoPopulation::getInput()
{
	return i_syn;
}

real* NengoPopulation::getOutput()
{
	return fire_array;
}
