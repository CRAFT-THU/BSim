
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
	int ret = -1;

	CHECK(g, c, nTypeNum);
	CHECK(g, c, sTypeNum);
	CHECK(g, c, neuronNums);
	CHECK(g, c, synapseNums);

	CHECK(g, c, nTypes);
	CHECK(g, c, sTypes);

	ret = 1;

	//int totalNeuronNum = g->neuronNums[g->nTypeNum+1];
	//int totalSynapseNum = g->synapseNums[g->sTypeNum+1];
	int MAX_DELAY = c->MAX_DELAY;

	N2SConnection p;
	checkCudaErrors(cudaMemcpy(&p, g->pN2SConnection, sizeof(N2SConnection), cudaMemcpyDeviceToHost));

	CHECK_CROSS_ARRAY(p.pSynapsesIdx, c->pN2SConnection->pSynapsesIdx, sizeof(int)*(c->pN2SConnection->s_num));
	CHECK_CROSS_ARRAY(p.delayStart, c->pN2SConnection->delayStart, sizeof(int)*(c->pN2SConnection->n_num)*MAX_DELAY);
	CHECK_CROSS_ARRAY(p.delayNum, c->pN2SConnection->delayNum, sizeof(int)*(c->pN2SConnection->n_num)*MAX_DELAY);

        ret = 2;

	return ret;
}
