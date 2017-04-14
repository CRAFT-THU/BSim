/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>
#include "mpi.h"

#include "../utils/macros.h"
#include "../utils/TagPool.h"
#include "GExpSynapses.h"

SYNAPSE_GPU_FUNC_BASIC(Exp)

int allocExp(void *pSynapses, int S)
{
	GExpSynapses *p = (GExpSynapses*)pSynapses;
	p->p_weight = (real*)malloc(S*sizeof(real));
	p->p_delay_steps = (int*)malloc(S*sizeof(int));

	p->p_active_steps = (int*)malloc(S*sizeof(int));
	p->p_C1 = (real*)malloc(S*sizeof(real));
	p->p__C1 = (real*)malloc(S*sizeof(real));
	p->p_I_syn = (real*)malloc(S*sizeof(real));

	return 0;
}

void mpiSendExp(void *data, int rank, int offset, int size)
{
	GExpSynapses *synapses = (GExpSynapses *)data;
	MPI_Send(&(size), 1, MPI_INT, rank, sendTag.getTag(), MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_weight[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getTag(), MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_delay_steps[offset]), sizeof(int)*size, MPI_BYTE, rank, sendTag.getTag(), MPI_COMM_WORLD);

	MPI_Send(&(synapses->p_C1[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getTag(), MPI_COMM_WORLD);
	MPI_Send(&(synapses->p__C1[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getTag(), MPI_COMM_WORLD);

	MPI_Send(&(synapses->p_I_syn[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getTag(), MPI_COMM_WORLD);

	MPI_Send(&(synapses->p_dst[offset]), size, MPI_INT, rank, sendTag.getTag(), MPI_COMM_WORLD);
}

void mpiRecvExp(void **data, int rank, int rankSize)
{
	GExpSynapses *synapses = (GExpSynapses *)malloc(sizeof(GExpSynapses));

	MPI_Status status;
	int size = 0;
	MPI_Recv(&size, 1, MPI_INT, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);
	allocExp(synapses, size);

	MPI_Recv(synapses->p_weight, sizeof(real)*size, MPI_BYTE, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_delay_steps, sizeof(int)*size, MPI_BYTE, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_C1, sizeof(real)*size, MPI_BYTE, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p__C1, sizeof(real)*size, MPI_BYTE, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);

	MPI_Recv(synapses->p_I_syn, sizeof(real)*size, MPI_BYTE, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);

	MPI_Recv(synapses->p_dst, size, MPI_INT, rank, recvTag.getTag(), MPI_COMM_WORLD, &status);

	*data = (void*)synapses;
}
