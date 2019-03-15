

#ifndef STATIC_H 
#define STATIC_H 


struct Static {
	int *p_dst;
	double *p_weight;
};

extern "C" {
	void update_static(Static *data, int num, int start_id, int t);
	Static * to_gpu_static(Static *cpu, int num);
	Static * from_gpu_static(Static *gpu, int num);
}

#endif // STATIC_H
