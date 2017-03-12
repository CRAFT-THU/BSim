/* This program is writen by qp09.
 * usually just for fun.
 * 一 四月 18 2016
 */

#include <mpi.h>
#include <assert.h>
#include <iostream>

#include "../utils/TypeFunc.h"
#include "GNetwork.h"

GNetwork *initGNetwork(int ntype_num, int stype_num) {
	GNetwork *ret_net = (GNetwork *)malloc(sizeof(GNetwork));

	ret_net->pNeurons = (void**)malloc(sizeof(void*)*ntype_num);
	assert(ret_net->pNeurons != NULL);
	ret_net->pSynapses = (void**)malloc(sizeof(void*)*stype_num);
	assert(ret_net->pSynapses != NULL);

	ret_net->pN2SConnection = (N2SConnection*)malloc(sizeof(N2SConnection));

	ret_net->neuronNums = (int*)malloc(sizeof(int)*(ntype_num + 1));
	assert(ret_net->neuronNums != NULL);
	ret_net->synapseNums = (int*)malloc(sizeof(int)*(stype_num + 1));
	assert(ret_net->synapseNums != NULL);
	ret_net->neuronNums[0] = 0;
	ret_net->synapseNums[0] = 0;

	ret_net->nTypeNum = ntype_num;
	ret_net->sTypeNum = stype_num;

	ret_net->nTypes = (Type*)malloc(sizeof(Type)*ntype_num);
	assert(ret_net->nTypes != NULL);
	ret_net->sTypes = (Type*)malloc(sizeof(Type)*stype_num);
	assert(ret_net->sTypes != NULL);

	ret_net->MAX_DELAY = 1;

	return ret_net;
}

int copyNetwork(GNetwork *dNet, GNetwork *sNet, int rank, int rankSize)
{
	dNet->pNeurons = sNet->pNeurons;
	dNet->pSynapses = sNet->pSynapses;

	for (int i=0; i<dNet->nTypeNum; i++) {
		//int size = dNet->neuronNums[i+1] - dNet->neuronNums[i];
		//dNet->nOffsets[i] = 0;
		//Copy neurons
		//copyType[dNet->nTypes[i]](dNet, allNet, i, size);
	}
	for (int i=0; i<dNet->sTypeNum; i++) {
		//int size = dNet->neuronNums[i+1] - dNet->neuronNums[i];
		//dNet->sOffsets[i] = 0;
		//Copy synapses
		//copyType[network->sTypes[i]](network, allNet, i, size);
	}
	
	return 0;
}


int mpiSendNetwork(GNetwork *network, int rank, int rankSize) 
{
	//for (int idx = 0; idx < rankSize; idx++) {
	//	if (idx == rank) {
	//		continue;
	//	}

	//	printf("SERVER: %d SENDING...\n", rank);
	//	for (int i=0; i<network->nTypeNum; i++) {
	//		//int num_i = network->gNeuronNums[i+1] - network->gNeuronNums[i];
	//		int size = num_i/rankSize;
	//		int range = num_i%size;

	//		//int nOffsets = 0;
	//		if (idx <= range) {
	//			//nOffsets = (size+1)*idx;
	//		} else {
	//			//nOffsets = (size+1)*range + size*(idx-range);
	//		}
	//		//Copy neurons
	//		//MPI_Send(&nOffsets, 1, MPI_INT, idx, 2*idx*network->nTypeNum + i, MPI_COMM_WORLD);
	//		//MPI_Send(&size, 1, MPI_INT, idx, 2*idx*network->nTypeNum + i + 1, MPI_COMM_WORLD);
	//		//mpiSendType[network->nTypes[i]](network->pNeurons[i], idx, nOffsets, size);
	//	}
	//	printf("SERVER: %d SENDED NEURONS\n", rank);
	//	for (int i=0; i<network->sTypeNum; i++) {
	//		//int num_i = network->gSynapseNums[i+1] - network->gSynapseNums[i];
	//		//int size = num_i/rankSize;
	//		//int range = num_i%size;

	//		//int sOffsets = 0;
	//		//if (idx < range) {
	//			sOffsets = (size+1)*range;
	//		} else {
	//			sOffsets = (size+1)*range + size*(idx-range);
	//		}
	//		MPI_Send(&sOffsets, 1, MPI_INT, idx, 2*idx*network->nTypeNum + i, MPI_COMM_WORLD);
	//		MPI_Send(&size, 1, MPI_INT, idx, 2*idx*network->nTypeNum + i + 1, MPI_COMM_WORLD);
	//		//Copy synapse
	//		mpiSendType[network->sTypes[i]](network->pSynapses[i], idx, sOffsets, size);
	//	}
	//	printf("SERVER: %d SENDED SYNAPSES\n", rank);
	//}

	return 0;
}

int mpiRecvNetwork(GNetwork *network, int rank, int rankSize) 
{
	//MPI_Status Status;
	//printf("SERVER: %d RECEIVING...\n", rank);
	//for (int i=0; i<network->nTypeNum; i++) {

	//	int nOffsets = 0;
	//	int size = 0;

	//	MPI_Recv(&nOffsets, 1, MPI_INT, 0, 2*rank*network->nTypeNum + i, MPI_COMM_WORLD, &Status);
	//	network->nOffsets[i] = nOffsets;
	//	MPI_Recv(&size, 1, MPI_INT, 0, 2*rank*network->nTypeNum + i + 1, MPI_COMM_WORLD, &Status);
	//	//Copy neurons
	//	mpiRecvType[network->nTypes[i]](&(network->pNeurons[i]), 0, size);
	//}
	//printf("SERVER: %d RECEIVED NEURONS\n", rank);
	//for (int i=0; i<network->sTypeNum; i++) {
	//	int sOffsets = 0;
	//	int size = 0;

	//	MPI_Recv(&sOffsets, 1, MPI_INT, 0, 2*rank*network->nTypeNum + i, MPI_COMM_WORLD, &Status);
	//	network->sOffsets[i] = sOffsets;
	//	MPI_Recv(&size, 1, MPI_INT, 0, 2*rank*network->nTypeNum + i + 1, MPI_COMM_WORLD, &Status);
	//	//Copy synapse
	//	mpiRecvType[network->sTypes[i]](&(network->pSynapses[i]), 0, size);
	//}
	//printf("SERVER: %d RECEIVED SYNAPSES\n", rank);

	return 0;
}


int printNetwork(GNetwork *net, int rank)
{
	printf("NETWORK PRINT START...\n");
	printf("Server: %d, nTypeNum: %d, sTypeNum: %d, MAX_DELAY: %d\n", rank, net->nTypeNum, net->sTypeNum, net->MAX_DELAY);

	printf("NTypes:");
	for(int i=0; i<net->nTypeNum; i++) {
		printf("%d ", net->nTypes[i]);
	}
	printf("\n");
	printf("STypes:");
	for(int i=0; i<net->sTypeNum; i++) {
		printf("%d ", net->sTypes[i]);
	}
	printf("\n");

	printf("NNum:");
	for(int i=0; i<net->nTypeNum+1; i++) {
		printf("%d ", net->neuronNums[i]);
	}
	printf("\n");
	printf("SNum:");
	for(int i=0; i<net->sTypeNum+1; i++) {
		printf("%d ", net->synapseNums[i]);
	}
	printf("\n");

	printf("Neurons:");
	for(int i=0; i<net->nTypeNum; i++) {
		printf("%d: %p\n", i, net->pNeurons[i]);
		//for (int i=0; i<(net->neuronNums[i+1]-net->neuronNums[i]); i++){
		//}
	}
	printf("\n");
	printf("Synapses:");
	for(int i=0; i<net->sTypeNum; i++) {
		printf("%d: %p\n", i, net->pSynapses[i]);
	}
	printf("\n");

	printf("NETWORK PRINT END...\n");

	return 0;
}
