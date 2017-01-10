
#include <assert.h>

#include "../third_party/cuda/helper_cuda.h"
#include "gpu_utils.h"

#define CHECK(p1, p2, para) \
	do { \
		assert((p1->para) == (p2->para)); \
	} while(0)

#define CHECK_ARRAY(p1, p2, n) \
	do { \
		for (int i_t=0; i_t<(n); i++) {\
			assert(((p1)[i_t]) == ((p2)[i_t])); \
		}\
	} while(0)

#define CHECK_CROSS_ARRAY(g, c, size) \
	do { \
		unsigned char * mem_t = (unsigned char*)malloc(size); \
		checkCudaErrors(cudaMemcpy(mem_t, (g), size, cudaMemcpyDeviceToHost)); \
		assert(0==memcmp(mem_t, (c), (size))); \
		free(mem_t); \
	} while(0)


int checkGPUNetwork(GNetwork *g, GNetwork *c)
{
	CHECK(g, c, nTypeNum);
	CHECK(g, c, sTypeNum);
	CHECK(g, c, noffsets);
	CHECK(g, c, soffsets);
	CHECK(g, c, neuronNums);
	CHECK(g, c, synapseNums);

	CHECK(g, c, nTypes);
	CHECK(g, c, sTypes);
	CHECK(g, c, gNeuronNums);
	CHECK(g, c, gSynapseNums);

	return 1;

	int totalNeuronNum = g->neuronNums[g->nTypeNum+1];
	int totalSynapseNum = g->synapseNums[g->sTypeNum+1];
	int MAX_DELAY = pCpuNet->MAX_DELAY;

	N2SConnection p;
	checkCudaErrors(cudaMemcpy(&p, g->pN2SConnection, sizeof(N2SConnection), cudaMemcpyDeviceToHost));

	CHECK_CROSS_ARRAY(p.pSynapsesIdx, c->pN2SConnection->pSynapsesIdx, sizeof(int)*totalSynapseNum);
	CHECK_CROSS_ARRAY(p.delayStart, c->pN2SConnection->delayStart, sizeof(int)*totalNeuronNum*MAX_DELAY);
	CHECK_CROSS_ARRAY(p.delayNum, c->pN2SConnection->delayNum, sizeof(int)*totalNeuronNum*MAX_DELAY);

	return 2;

	return 0;
}
