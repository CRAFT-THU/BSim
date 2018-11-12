/* This program is writen by qp09.
 * usually just for fun.
 * Wed January 06 2016
 */

#include <stdlib.h>
#include <math.h>

#include "FFTNeuron.h"
#include "GFFTNeurons.h"

const Type FFTNeuron::type = FFT;

FFTNeuron::FFTNeuron() : NeuronBase()
{
	this->fire_count = 0;
	file = NULL;
	fired = false;
	monitored = false;

}

FFTNeuron::FFTNeuron(const FFTNeuron &templ) : NeuronBase()
{
	file = NULL;
	fired = false;
	this->fire_count = templ.fire_count;
	monitored = templ.monitored;
}

FFTNeuron::~FFTNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
		file = NULL;
	}
}

int FFTNeuron::reset(SimInfo &info)
{
	fired = false;
	this->fire_count = 0;
	
	return 0;
}

int FFTNeuron::recv(real I) {
	return 0;
}

Type FFTNeuron::getType()
{
	return type;
}

int FFTNeuron::update(SimInfo &info)
{
	//TODO
	fired = false;
	if (info.currCycle > fire_count) {
		fired = true;
		fire_count++;
		fire();
	}
	info.fired.push_back(getID());
	return 0;
}

void FFTNeuron::monitor(SimInfo &info)
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "FFTNeuron_%d.log", getID());
			file = fopen(filename, "w+");
			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}
		} else {
			if (fired) {
				fprintf(file, "%d:%d\n", info.currCycle, fire_count);
			}
		}
	}
	return;
}


size_t FFTNeuron::getSize() {
	return sizeof(GFFTNeurons);
}

int FFTNeuron::getData(void *data)
{
	return 0;
}

int FFTNeuron::hardCopy(void *data, int idx, int base)
{
	GFFTNeurons *p = (GFFTNeurons*) data;
	setID(idx+base);

	p->p_fire_count[idx] = fire_count;


	return 1;
}

