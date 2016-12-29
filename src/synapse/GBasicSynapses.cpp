/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 27 2016
 */

#include <stdlib.h>
#include "mpi.h"

#include "../utils/macros.h"
#include "../utils/IDPool.h"
#include "GBasicSynapses.h"

SYNAPSE_GPU_FUNC_BASIC(Basic)

int allocBasicSynapses(void *pSynapses, int S)
{
	GBasicSynapses *p = (GBasicSynapses*)pSynapses;
	p->num = S;
	p->p_weight = (real*)malloc(S*sizeof(real));
	p->p_delay_steps = (int*)malloc(S*sizeof(int));

	return 0;
}

void sendBasicSynapses(void *data, int rank, int offset, int size)
{
	GBasicSynapses *synapses = (GBasicSynapses *)data;
	MPI_Send(&(size), 1, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_weight[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_delay_steps[offset]), sizeof(int)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	MPI_Send(&(synapses->pDst[offset]), size, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
}

void recvBasicSynapses(void **data, int rank, int rankSize)
{
	GBasicSynapses *synapses = (GBasicSynapses *)malloc(sizeof(GBasicSynapses));

	MPI_Status status;
	MPI_Recv((&synapses->num), 1, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	allocBasicSynapses(synapses, synapses->num);

	int size = synapses->num;

	MPI_Recv(synapses->p_weight, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_delay_steps, sizeof(int)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	int *dst = (int*)malloc(sizeof(int) * synapses->num);
	MPI_Recv(dst, size, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	allocBasicConnects(synapses, dst, NULL, NULL, 0);

	*data = (void*)synapses;
}
