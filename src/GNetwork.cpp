/* This program is writen by qp09.
 * usually just for fun.
 * 一 四月 18 2016
 */

#include <mpi.h>
#include <iostream>

#include "GNetwork.h"
#include "mpi_func.h"

int copyNetwork(GNetwork *dNet, GNetwork *sNet, int rank, int rankSize)
{
	dNet->pNeurons = sNet->pNeurons;
	dNet->pSynapses = sNet->pSynapses;

	for (int i=0; i<dNet->nTypeNum; i++) {
		//int size = dNet->neuronNums[i+1] - dNet->neuronNums[i];
		dNet->nOffsets[i] = 0;
	//	//Copy neurons
	//	//copyType[dNet->nTypes[i]](dNet, allNet, i, size);
	}
	for (int i=0; i<dNet->sTypeNum; i++) {
	//	int size = dNet->neuronNums[i+1] - dNet->neuronNums[i];

		dNet->sOffsets[i] = 0;
	//	//Copy synapses
	//	//copyType[network->sTypes[i]](network, allNet, i, size);
	}
	
	return 0;
}


int mpiSendNetwork(GNetwork *network, int rank, int rankSize) 
{
	for (int idx = 0; idx < rankSize; idx++) {
		if (idx == rank) {
			continue;
		}

		for (int i=0; i<network->nTypeNum; i++) {
			int num_i = network->gNeuronNums[i+1] - network->gNeuronNums[i];
			int size = num_i/rankSize;
			int range = num_i%size;

			int nOffsets = 0;
			if (idx <= range) {
				nOffsets = (size+1)*idx;
			} else {
				nOffsets = (size+1)*range + size*(idx-range);
			}
			//Copy neurons
			MPI_Send(&nOffsets, 1, MPI_INT, idx, 2*idx*network->nTypeNum + i, MPI_COMM_WORLD);
			MPI_Send(&size, 1, MPI_INT, idx, 2*idx*network->nTypeNum + i + 1, MPI_COMM_WORLD);
			sendType[network->nTypes[i]](network->pNeurons[i], idx, nOffsets, size);
		}
		for (int i=0; i<network->sTypeNum; i++) {
			int num_i = network->gSynapseNums[i+1] - network->gSynapseNums[i];
			int size = num_i/rankSize;
			int range = num_i%size;

			int sOffsets = 0;
			if (idx < range) {
				sOffsets = (size+1)*range;
			} else {
				sOffsets = (size+1)*range + size*(idx-range);
			}
			MPI_Send(&sOffsets, 1, MPI_INT, idx, -2*idx*network->nTypeNum - i, MPI_COMM_WORLD);
			MPI_Send(&size, 1, MPI_INT, idx, -2*idx*network->nTypeNum - i - 1, MPI_COMM_WORLD);
			//Copy synapse
			sendType[network->sTypes[i]](network->pSynapses[i], idx, sOffsets, size);
		}
	}
}

int mpiRecvNetwork(GNetwork *network, int rank, int rankSize) 
{
	MPI_Status Status;
	for (int i=0; i<network->nTypeNum; i++) {

		int nOffsets = 0;
		int size = 0;

		MPI_Recv(&nOffsets, 1, MPI_INT, 0, 2*rank*network->nTypeNum + i, MPI_COMM_WORLD, &Status);
		network->nOffsets[i] = nOffsets;
		MPI_Recv(&size, 1, MPI_INT, 0, 2*rank*network->nTypeNum + i + 1, MPI_COMM_WORLD, &Status);
		//Copy neurons
		recvType[network->nTypes[i]](&(network->pNeurons[i]), 0, size);
	}
	for (int i=0; i<network->sTypeNum; i++) {
		int sOffsets = 0;
		int size = 0;

		MPI_Recv(&sOffsets, 1, MPI_INT, 0, -2*rank*network->nTypeNum - i, MPI_COMM_WORLD, &Status);
		network->sOffsets[i] = sOffsets;
		MPI_Recv(&size, 1, MPI_INT, 0, -2*rank*network->nTypeNum - i - 1, MPI_COMM_WORLD, &Status);
		//Copy synapse
		recvType[network->sTypes[i]](&(network->pSynapses[i]), 0, size);
	}
}
