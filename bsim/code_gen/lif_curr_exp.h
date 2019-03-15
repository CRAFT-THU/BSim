

#ifndef LIF_CURR_EXP_H 
#define LIF_CURR_EXP_H 


struct Lif_curr_exp {
	int *p_refract_time;
	int *p_refract_step;
	double *p_Cinh;
	double *p_v_tmp;
	double *p_v_reset;
	double *p_Cexec;
	double *p_C_inh;
	double *p_C;
	double *p_C_exec;
	double *p_v_threshold;
	double *p_i_exec;
	double *p_v;
	double *p_i_inh;
};

extern "C" {
	void update_lif_curr_exp(Lif_curr_exp *data, int num, int start_id, int t);
	Lif_curr_exp * to_gpu_lif_curr_exp(Lif_curr_exp *cpu, int num);
	Lif_curr_exp * from_gpu_lif_curr_exp(Lif_curr_exp *gpu, int num);
}

#endif // LIF_CURR_EXP_H
