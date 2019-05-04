/* This program is writen by qp09.
 * usually just for fun.
 * 一 四月 18 2016
 */

#include <mpi.h>
#include <assert.h>
#include <iostream>

#include "../utils/TypeFunc.h"
#include "../utils/FileOp.h"
#include "GNetwork.h"

GNetwork * deepcopyNetwork(GNetwork * net) {
	GNetwork * ret = allocNetwork(net->nTypeNum, net->sTypeNum);

	for (int i=0; i<net->nTypeNum; i++) {
		ret->pNTypes[i] = net->pNTypes[i];
		ret->pNeuronNums[i] = net->pNeuronNums[i];
		ret->ppNeurons[i] = allocType[net->pNTypes[i]](net->pNeuronNums[i+1]-net->pNeuronNums[i]);
	}
	ret->pNeuronNums[net->nTypeNum] = net->pNeuronNums[net->nTypeNum];

	for (int i=0; i<net->sTypeNum; i++) {
		ret->pSTypes[i] = net->pSTypes[i];
		ret->pSynapseNums[i] = net->pSynapseNums[i];
		ret->ppSynapses[i] = allocType[net->pSTypes[i]](net->pSynapseNums[i+1]-net->pSynapseNums[i]);
	}
	ret->pSynapseNums[net->sTypeNum] = net->pSynapseNums[net->sTypeNum];

	ret->pConnection = allocConnection(net->pConnection->nNum, net->pConnection->sNum, 
			net->pConnection->maxDelay, net->pConnection->minDelay);

	return ret;
}

GNetwork * allocNetwork(int nTypeNum, int sTypeNum) {
	GNetwork *ret = (GNetwork *)malloc(sizeof(GNetwork));
	assert(ret != NULL);

	ret->nTypeNum = nTypeNum;
	ret->sTypeNum = sTypeNum;

	// ret->maxDelay = 1;
	// ret->minDelay = 1e7;

	ret->pNTypes = (Type *)malloc(sizeof(Type)*nTypeNum);
	assert(ret->pNTypes != NULL);
	ret->pSTypes = (Type *)malloc(sizeof(Type)*sTypeNum);
	assert(ret->pSTypes != NULL);

	ret->pNeuronNums = (int*)malloc(sizeof(int)*(nTypeNum + 1));
	assert(ret->pNeuronNums != NULL);
	ret->pSynapseNums = (int*)malloc(sizeof(int)*(sTypeNum + 1));
	assert(ret->pSynapseNums != NULL);

	ret->pNeuronNums[0] = 0;
	ret->pSynapseNums[0] = 0;

	ret->ppNeurons = (void **)malloc(sizeof(void*)*nTypeNum);
	assert(ret->ppNeurons != NULL);
	ret->ppSynapses = (void **)malloc(sizeof(void*)*sTypeNum);
	assert(ret->ppSynapses != NULL);

	ret->pConnection = NULL;

	return ret;
}


void freeGNetwork(GNetwork * network)
{
	for (int i=0; i<network->nTypeNum; i++) {
		freeType[network->pNTypes[i]](network->ppNeurons[i]);
	}
	free(network->ppNeurons);

	for (int i=0; i<network->sTypeNum; i++) {
		freeType[network->pSTypes[i]](network->ppSynapses[i]);
	}
	free(network->ppSynapses);

	freeConnection(network->pConnection);

	free(network->pNeuronNums);
	free(network->pSynapseNums);

	free(network->pNTypes);
	free(network->pSTypes);
}

int saveGNetwork(GNetwork *net, char *filename)
{
	FILE * f = openFile(filename, "w+");

	fwrite(&(net->nTypeNum), sizeof(int), 1, f);
	fwrite(&(net->sTypeNum), sizeof(int), 1, f);
	// fwrite(&(net->maxDelay), sizeof(int), 1, f);
	// fwrite(&(net->minDelay), sizeof(int), 1, f);

	fwrite(net->pNTypes, sizeof(Type), net->nTypeNum, f);
	fwrite(net->pSTypes, sizeof(Type), net->sTypeNum, f);
	fwrite(net->pNeuronNums, sizeof(int), net->nTypeNum+1, f);
	fwrite(net->pSynapseNums, sizeof(int), net->sTypeNum+1, f);

	for (int i=0; i<net->nTypeNum; i++) {
		saveType[net->pNTypes[i]](net->ppNeurons[i], net->pNeuronNums[i+1]-net->pNeuronNums[i], f);
	}
	for (int i=0; i<net->sTypeNum; i++) {
		saveType[net->pSTypes[i]](net->ppSynapses[i], net->pSynapseNums[i+1]-net->pSynapseNums[i], f);
	}

	saveConnection(net->pConnection, f);

	closeFile(f);
	return 0;
}

GNetwork *loadGNetwork(char *filename)
{
	int nTypeNum = 0, sTypeNum = 0;
	FILE * f = openFile(filename, "w+");

	fread(&nTypeNum, sizeof(int), 1, f);
	fread(&sTypeNum, sizeof(int), 1, f);

	GNetwork * net = allocNetwork(nTypeNum, sTypeNum);

	// fread(&(net->maxDelay), sizeof(int), 1, f);
	// fread(&(net->minDelay), sizeof(int), 1, f);

	fread(net->pNTypes, sizeof(Type), net->nTypeNum, f);
	fread(net->pSTypes, sizeof(Type), net->sTypeNum, f);
	fread(net->pNeuronNums, sizeof(int), net->nTypeNum+1, f);
	fread(net->pSynapseNums, sizeof(int), net->sTypeNum+1, f);

	for (int i=0; i<net->nTypeNum; i++) {
		net->ppNeurons[i] = loadType[net->pNTypes[i]](net->pNeuronNums[i+1]-net->pNeuronNums[i], f);
	}
	for (int i=0; i<net->sTypeNum; i++) {
		net->ppSynapses[i] = loadType[net->pSTypes[i]](net->pSynapseNums[i+1]-net->pSynapseNums[i], f);
	}

	net->pConnection = loadConnection(f);

	closeFile(f);
	return net;
}

int copyNetwork(GNetwork *dNet, GNetwork *sNet, int rank, int rankSize)
{
	dNet->ppNeurons = sNet->ppNeurons;
	dNet->ppSynapses = sNet->ppSynapses;

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


// int printNetwork(GNetwork *net, int rank)
// {
// 	printf("NETWORK PRINT START...\n");
// 	// printf("Server: %d, nTypeNum: %d, sTypeNum: %d, maxDelay: %d, minDelay: %d\n", rank, net->nTypeNum, net->sTypeNum, net->maxDelay, net->minDelay);
// 
// 	printf("NTypes:");
// 	for(int i=0; i<net->nTypeNum; i++) {
// 		printf("%d ", net->nTypes[i]);
// 	}
// 	printf("\n");
// 	printf("STypes:");
// 	for(int i=0; i<net->sTypeNum; i++) {
// 		printf("%d ", net->sTypes[i]);
// 	}
// 	printf("\n");
// 
// 	printf("NNum:");
// 	for(int i=0; i<net->nTypeNum+1; i++) {
// 		printf("%d ", net->neuronNums[i]);
// 	}
// 	printf("\n");
// 	printf("SNum:");
// 	for(int i=0; i<net->sTypeNum+1; i++) {
// 		printf("%d ", net->synapseNums[i]);
// 	}
// 	printf("\n");
// 
// 	printf("Neurons:");
// 	for(int i=0; i<net->nTypeNum; i++) {
// 		printf("%d: %p\n", i, net->pNeurons[i]);
// 		//for (int i=0; i<(net->neuronNums[i+1]-net->neuronNums[i]); i++){
// 		//}
// 	}
// 	printf("\n");
// 	printf("Synapses:");
// 	for(int i=0; i<net->sTypeNum; i++) {
// 		printf("%d: %p\n", i, net->pSynapses[i]);
// 	}
// 	printf("\n");
// 
// 	printf("NETWORK PRINT END...\n");
// 
// 	return 0;
// }
