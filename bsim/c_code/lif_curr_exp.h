

#include "blocksize.h"
#ifndef LIF_CURR_EXP_H 
#define LIF_CURR_EXP_H 


struct Lif_curr_exp {
	int *p_refract_step;
	float *p_v;
};

extern "C" {
	void update_lif_curr_exp(Lif_curr_exp *data, int num, int start_id, BlockSize *size);
	Lif_curr_exp * to_gpu_lif_curr_exp(Lif_curr_exp *cpu, int num);
}

__global__ void find_lif_curr_exp_gpu(Lif_curr_exp *data, int num, int start_id);
__global__ void update_lif_curr_exp_gpu(Lif_curr_exp *data, int num, int start_id);

#endif // LIF_CURR_EXP_H
