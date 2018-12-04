

#ifndef STDPSYNAPSE_H 
#define STDPSYNAPSE_H 


#include "blocksize.h"


struct Stdpsynapse {
	int *p_last_update;
	int *p_dst;
	int *p_delay;
	float *p_tau_pre;
	float *p_tau_post;
	float *p_d_apre;
	float *p_d_apost;
	float *p_apre;
	float *p_weight;
	float *p_apost;
};

extern "C" {
	void update_stdpsynapse(Stdpsynapse *data, int num, int start_id, BlockSize *size);
	Stdpsynapse * to_gpu_stdpsynapse(Stdpsynapse *cpu, int num);
	Stdpsynapse * from_gpu_stdpsynapse(Stdpsynapse *gpu, int num);
}

__global__ void find_stdpsynapse_gpu(Stdpsynapse *data, int num, int start_id);
__global__ void update_stdpsynapse_gpu(Stdpsynapse *data, int num, int start_id);

#endif // STDPSYNAPSE_H
