/* This program is writen by qp09.
 * usually just for fun.
 * Tue September 29 2015
 */

#include <vector>
#include <math.h>

#include "utils/json/json.h"
#include "ProbeNeuron.h"
//#include "GProbeNeurons.h"

using std::vector;

ProbeNeuron::ProbeNeuron(ID id, real tau_syn_E, real tau_syn_I)
	:tau_syn_E(tau_syn_E), tau_syn_I(tau_syn_I)
{
	this->i_syn = 0;
	this->i_tmp = 0;
	this->id = id;
	this->file = NULL;
	this->monitored = false;
}

ProbeNeuron::ProbeNeuron(const ProbeNeuron &neuron, ID id)
{
	this->tau_syn_E = neuron.tau_syn_E;
	this->tau_syn_I = neuron.tau_syn_I;
	this->i_syn = 0;
	this->i_tmp = 0;
	this->file = NULL;
	this->id = id;
}

ProbeNeuron::~ProbeNeuron()
{
	if (file != NULL) {
		fflush(file);
		fclose(file);
	}
}

int ProbeNeuron::init(real dt)
{
	_dt = dt;
	
	return 0;
}

int ProbeNeuron::update(SimInfo &info)
{
	i_tmp = i_syn;
	i_syn = 0;

	return 0;
}

int ProbeNeuron::recv(real I)
{
	i_syn += I;
	return 0;
}

int ProbeNeuron::reset(SimInfo &info)
{
	i_syn = 0;
	return init(info.dt);
}

ID ProbeNeuron::getID()
{
	return id;
}

real ProbeNeuron::get_vm()
{
	return 0;
}

void ProbeNeuron::monitor(SimInfo &info) 
{
	if (monitored) {
		if (file == NULL) {
			char filename[128];
			sprintf(filename, "Probe_%d_%d.log", id.groupId, id.id);
			file = fopen(filename, "w+");

			if (file == NULL) {
				printf("Open File: %s failed\n", filename);
				return;
			}

		}
		fprintf(file, "Cycle %d: %f\n", info.currCycle-1, i_tmp); 
	}
}

size_t ProbeNeuron::getSize()
{
	return 0;
}

int ProbeNeuron::getData(void *data)
{
	Json::Value *p = (Json::Value *)data;

	(*p)["id"] = id.id;
	(*p)["tau_syn_E"] = tau_syn_E;
	(*p)["tau_syn_I"] = tau_syn_I;

	return 0;
}

int ProbeNeuron::hardCopy(void *data, int idx)
{
	//GProbeNeurons * p = (GProbeNeurons *) data;
	//p->pID[idx] = id;
	//p->pType[idx] = type;
	//p->p_tau_syn_E[idx] = tau_syn_E;
	//p->p_tau_syn_I[idx] = tau_syn_I;
	//p->p_i_syn[idx] = i_syn;
	//p->p__dt[idx] = _dt;
	return 1;
}

int ProbeNeuron::fire()
{
	return 0;
}	

SynapseBase* ProbeNeuron::addSynapse(real weight, real delay, SpikeType type, real tau, NeuronBase *pDest)
{
	return NULL;
}
