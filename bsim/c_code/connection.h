

#ifndef CONNECTION_H
#define CONNECTION_H


struct CConnection {
	int *delay_start;
	int *delay_num;
	int *rev_delay_start;
	int *rev_delay_num;
	int *rev_map2sid;
	int n_length;
	int s_length;
	};

extern "C" {
	CConnection * to_gpu_connection(CConnection *cpu);
	CConnection * from_gpu_connection(CConnection *gpu);
}

#endif // CONNECTION_H
