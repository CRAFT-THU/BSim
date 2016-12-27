/* This header file is writen by qp09
 * usually just for fun
 * Mon December 26 2016
 */
#ifndef GPU_MACROS_H
#define GPU_MACROS_H

#define GET_DELAY(name, type) \
	__device__ int get_##name##_delay(void *data, int num, int sid) \
	{ \
		G##type##Synapses *d_synapses = (G##type##Synapses*)data; \
		return (int)(d_synapses->p_delay_steps[sid]); \
	}

#define UPDATE_NEURON_SPIKE(name, type) \
	__device__ int update_##name##_spike(void *data, int num, int nid, int start_t, int simTime) \
	{ \
		if (nid >= num) { \
			return -1; \
		} \
		G##type##Neurons *d_neurons = (G##type##Neurons*)data; \
		for (int i=0; i<d_neurons->pSynapsesNum[nid]; i++) { \
			int loc = d_neurons->pSynapsesLoc[nid]; \
			int sid = d_neurons->pSynapsesIdx[i+loc]; \
			int offset = 0; \
			int type = get_type(gGpuNet->synapseNums, gGpuNet->sTypeNum, sid, &offset); \
			if (simTime == start_t + get_delay[gGpuNet->sTypes[type]](gGpuNet->pSynapses[type], gGpuNet->synapseNums[type+1]-gGpuNet->synapseNums[type], offset)) \
				gSynapsesFiredTable[d_neurons->pSynapsesIdx[i+loc]] = true; \
		} \
		return 0; \
	}

#endif /* GPU_MACROS_H */

