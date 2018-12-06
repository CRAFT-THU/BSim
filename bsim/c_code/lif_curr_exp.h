

#ifndef LIF_CURR_EXP_H 
#define LIF_CURR_EXP_H 


#include "blocksize.h"


struct Lif_curr_exp {
	int *p_refract_step;
	int *p_refract_time;
	float *p_v_tmp;
	float *p_v_reset;
	float *p_Cexec;
	float *p_C_exec;
	float *p_C_inh;
	float *p_v_threshold;
	float *p_Cm;
	float *p_Cinh;
	float *p_v;
};

extern "C" {
	void update_lif_curr_exp(Lif_curr_exp *data, int num, int start_id, int t);
	Lif_curr_exp * to_gpu_lif_curr_exp(Lif_curr_exp *cpu, int num);
	Lif_curr_exp * from_gpu_lif_curr_exp(Lif_curr_exp *gpu, int num);
}

#endif // LIF_CURR_EXP_H
