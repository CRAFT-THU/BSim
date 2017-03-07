/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>
#include "mpi.h"

#include "../utils/macros.h"
#include "../utils/IDPool.h"
#include "GLIFNeurons.h"

NEURON_GPU_FUNC_BASIC(LIF)

int allocLIF(void *pCpu, int N)
{
	GLIFNeurons *p = (GLIFNeurons *)pCpu;
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	p->p_i_syn = (real*)malloc(N*sizeof(real));
	p->p_vm = (real*)malloc(N*sizeof(real));
	p->p_C1 = (real*)malloc(N*sizeof(real));
	p->p_C2 = (real*)malloc(N*sizeof(real));
	p->p_i_tmp = (real*)malloc(N*sizeof(real));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	p->p_refrac_time = (int*)malloc(N*sizeof(int));
	
	return 0;
}

void mpiSendLIF(void *data, int rank, int offset, int size)
{
	GLIFNeurons *neurons = (GLIFNeurons *)data;
	MPI_Send(&(size), 1, MPI_INT, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_v_reset[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);

	MPI_Send(&(neurons->p_v_thresh[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_i_syn[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);

	MPI_Send(&(neurons->p_vm[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_C1[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_C2[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_i_tmp[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_refrac_step[offset]), size, MPI_INT, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_refrac_time[offset]), size, MPI_INT, rank, sendTag.getID().getId(), MPI_COMM_WORLD);
}

void mpiRecvLIF(void **data, int rank, int rankSize)
{
	GLIFNeurons *neurons = (GLIFNeurons *)malloc(sizeof(GLIFNeurons));

	MPI_Status status;
	int size = 0;
	MPI_Recv((&size), 1, MPI_INT, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	allocLIF(neurons, size);

	MPI_Recv(neurons->p_v_reset, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_v_thresh, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_i_syn, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);

	MPI_Recv(neurons->p_vm, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_C1, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_C2, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_i_tmp, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_refrac_step, size, MPI_INT, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_refrac_time, size, MPI_INT, rank, recvTag.getID().getId(), MPI_COMM_WORLD, &status);

	*data = (void*)neurons;
}
