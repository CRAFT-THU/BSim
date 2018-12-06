

#ifndef STDP_SYNAPSE_H 
#define STDP_SYNAPSE_H 


#include "blocksize.h"


struct Stdp_synapse {
	int *p_last_update;
	int *p_dst;
	int *p_delay;
	float *p_d_apost;
	float *p_tau_pre;
	float *p_d_apre;
	float *p_tau_post;
	float *p_apre;
	float *p_apost;
	float *p_weight;
};

extern "C" {
	void update_stdp_synapse(Stdp_synapse *data, int num, int start_id, int t);
	Stdp_synapse * to_gpu_stdp_synapse(Stdp_synapse *cpu, int num);
	Stdp_synapse * from_gpu_stdp_synapse(Stdp_synapse *gpu, int num);
}

#endif // STDP_SYNAPSE_H
