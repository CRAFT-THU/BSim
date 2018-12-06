#ifndef RUNTIME_H
#define RUNTIME_H


#include "connection.h"


const int MAX_DELAY = 2;
const int MIN_DELAY = 0;
const float G_MAX = 100;
const float G_MIN = -100;
const int LIF_CURR_EXP_BLOCKSIZE = 32;
const int LIF_CURR_EXP_GRIDSIZE = 1;
const int STDP_SYNAPSE_BLOCKSIZE = 128;
const int STDP_SYNAPSE_GRIDSIZE = 1;

const int MAX_BLOCK_SIZE = 1024;
const int FIRED_TABLE_SIZE = 8;
extern __device__ int * g_fired_table;
extern __device__ int * g_fired_table_sizes;
extern __device__ int * g_active_lif_curr_exp_table;
extern __device__ int g_active_lif_curr_exp_table_size;
extern __device__ CConnection * g_connection_stdp_synapse;
extern __device__ float * g_i_exec;
extern __device__ float * g_i_inh;

extern "C" {
	void init_runtime(CConnection *connections);
}

__device__ int commit2globalTable(int *shared_buf, volatile unsigned int size, int *global_buf, int * global_size, int offset);
#endif // RUNTIME_H
