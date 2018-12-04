

#ifndef STDP_SYNAPSE_H 
#define STDP_SYNAPSE_H 


#include "blocksize.h"


struct Stdp_synapse {
	int *p_last_update;
	int *p_delay;
	int *p_dst;
	float *p_tau_post;
	float *p_d_apost;
	float *p_tau_pre;
	float *p_d_apre;
	float *p_weight;
	float *p_apre;
	float *p_apost;
};

extern "C" {
	void update_stdp_synapse(Stdp_synapse *data, int num, int start_id, BlockSize *size);
	Stdp_synapse * to_gpu_stdp_synapse(Stdp_synapse *cpu, int num);
	Stdp_synapse * from_gpu_stdp_synapse(Stdp_synapse *gpu, int num);
}

__global__ void find_stdp_synapse_gpu(Stdp_synapse *data, int num, int start_id);
__global__ void update_stdp_synapse_gpu(Stdp_synapse *data, int num, int start_id);

#endif // STDP_SYNAPSE_H
