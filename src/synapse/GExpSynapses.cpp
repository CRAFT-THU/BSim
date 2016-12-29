/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>
#include "mpi.h"

#include "../utils/macros.h"
#include "../utils/IDPool.h"
#include "GExpSynapses.h"

//void *createExpSynapses()
//{
//	return malloc(sizeof(GExpSynapses));
//}
//
//size_t getExpSize()
//{
//	return sizeof(GExpSynapses);
//}
//
//int allocExpConnects(void *pCpu, int *pSynapsesDst, int *notUsed1, int *notUsed2, int notUsed3)
//{
//	GExpSynapses *p = (GExpSynapses*)pCpu;
//	p->pDst = pSynapsesDst;
//
//	return 0;
//}

SYNAPSE_GPU_FUNC_BASIC(Exp)

int allocExpSynapses(void *pSynapses, int S)
{
	GExpSynapses *p = (GExpSynapses*)pSynapses;
	p->num = S;
	//p->pID = (ID*)malloc(S*sizeof(ID));
	//p->pType = (Type*)malloc(S*sizeof(Type));
	p->p_weight = (real*)malloc(S*sizeof(real));
	p->p_delay_steps = (int*)malloc(S*sizeof(int));
	//p->p_delay = (real*)malloc(S*sizeof(real));
	p->p_C1 = (real*)malloc(S*sizeof(real));
	p->p__C1 = (real*)malloc(S*sizeof(real));
	//p->p_tau_syn = (real*)malloc(S*sizeof(real));
	p->p_I_syn = (real*)malloc(S*sizeof(real));
	//p->p__dt = (real*)malloc(S*sizeof(real));
	//p->pSrc = (int *)malloc(S*sizeof(int));
	//p->pDst = (int *)malloc(S*sizeof(int));

	return 0;
}

void sendExpSynapses(void *data, int rank, int offset, int size)
{
	GExpSynapses *synapses = (GExpSynapses *)data;
	MPI_Send(&(size), 1, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(synapses->pID[offset]), sizeof(ID)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(synapses->pType[offset]), sizeof(Type)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_weight[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_delay_steps[offset]), sizeof(int)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(synapses->p_delay[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	MPI_Send(&(synapses->p_C1[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(synapses->p__C1[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	//MPI_Send(&(synapses->p_tau_syn[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(synapses->p_I_syn[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);


	//MPI_Send(&(synapses->p__dt[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	MPI_Send(&(synapses->pDst[offset]), size, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
}

void recvExpSynapses(void **data, int rank, int rankSize)
{
	GExpSynapses *synapses = (GExpSynapses *)malloc(sizeof(GExpSynapses));

	MPI_Status status;
	MPI_Recv((&synapses->num), 1, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	allocExpSynapses(synapses, synapses->num);

	int size = synapses->num;

	//MPI_Recv(synapses->pID, sizeof(ID)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(synapses->pType, sizeof(Type)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_weight, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_delay_steps, sizeof(int)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(synapses->p_delay, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_C1, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p__C1, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	//MPI_Recv(synapses->p_tau_syn, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapses->p_I_syn, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	//MPI_Recv(synapses->p__dt, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	int *dst = (int*)malloc(sizeof(int) * synapses->num);
	MPI_Recv(dst, size, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	allocExpConnects(synapses, dst, NULL, NULL, 0);

	*data = (void*)synapses;
}
