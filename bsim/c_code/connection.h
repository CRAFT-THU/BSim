

#ifndef CONNECTION_H
#define CONNECTION_H


struct CConnection {
	int *delay_start;
	int *delay_num;
	int *rev_delay_start;
	int *rev_delay_num;
	int *rev_map2sid;
	int n_len;
	int r_n_len;
	int s_len;
	};

extern "C" {
	CConnection * to_gpu_connection(CConnection *cpu);
	CConnection * from_gpu_connection(CConnection *gpu);
}

#endif // CONNECTION_H
