/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include <stdlib.h>
#include "mpi.h"

#include "IDPool.h"
#include "GLIFNeurons.h"

void* createLIFNeurons()
{
	return malloc(sizeof(GLIFNeurons));
}

size_t getLIFSize()
{
	return sizeof(GLIFNeurons);
}

int allocLIFNeurons(void *pCpu, int N)
{
	GLIFNeurons *p = (GLIFNeurons *)pCpu;
	p->num = N;
	//p->pID = (ID*)malloc(N*sizeof(ID));
	//p->pType = (Type*)malloc(N*sizeof(Type));
	//p->p_v_init = (real*)malloc(N*sizeof(real));
	//p->p_v_rest = (real*)malloc(N*sizeof(real));
	p->p_v_reset = (real*)malloc(N*sizeof(real));
	//p->p_cm = (real*)malloc(N*sizeof(real));
	//p->p_tau_m = (real*)malloc(N*sizeof(real));
	//p->p_tau_refrac = (real*)malloc(N*sizeof(real));
	//p->p_tau_syn_E = (real*)malloc(N*sizeof(real));
	//p->p_tau_syn_I = (real*)malloc(N*sizeof(real));
	p->p_v_thresh = (real*)malloc(N*sizeof(real));
	//p->p_i_offset = (real*)malloc(N*sizeof(real));
	p->p_i_syn = (real*)malloc(N*sizeof(real));
	p->p_vm = (real*)malloc(N*sizeof(real));
	//p->p__dt = (real*)malloc(N*sizeof(real));
	p->p_C1 = (real*)malloc(N*sizeof(real));
	p->p_C2 = (real*)malloc(N*sizeof(real));
	p->p_i_tmp = (real*)malloc(N*sizeof(real));
	p->p_refrac_step = (int*)malloc(N*sizeof(int));
	p->p_refrac_time = (int*)malloc(N*sizeof(int));
	//p->pSynapsesNum = (int*)malloc(N*sizeof(int));
	//p->pSynapsesLoc = (int*)malloc(N*sizeof(int));
	
	return 0;
}

int allocLIFConnects(void *pCpu, int *pSynapsesNum, int *pSynapsesLoc, int *pSynapsesIdx, int S)
{
	GLIFNeurons *p = (GLIFNeurons *)pCpu;
	p->synapsesNum = S;
	p->pSynapsesIdx = pSynapsesIdx;
	p->pSynapsesNum = pSynapsesNum;
	p->pSynapsesLoc = pSynapsesLoc;

	return 0;
}

void sendLIFNeurons(void *data, int rank, int offset, int size)
{
	GLIFNeurons *neurons = (GLIFNeurons *)data;
	MPI_Send(&(size), 1, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->synapsesNum), 1, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->pID[offset]), sizeof(ID)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->pType[offset]), sizeof(Type)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_v_init[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_v_rest[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_v_reset[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	//MPI_Send(&(neurons->p_cm[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_tau_m[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_tau_refrac[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_tau_syn_E[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_tau_syn_I[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	MPI_Send(&(neurons->p_v_thresh[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p_i_offset[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_i_syn[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);

	MPI_Send(&(neurons->p_vm[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	//MPI_Send(&(neurons->p__dt[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_C1[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_C2[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_i_tmp[offset]), sizeof(real)*size, MPI_BYTE, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_refrac_step[offset]), size, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->p_refrac_time[offset]), size, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);

	MPI_Send(&(neurons->pSynapsesNum[offset]), size, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(&(neurons->pSynapsesLoc[offset]), size, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
	MPI_Send(neurons->pSynapsesIdx, neurons->synapsesNum, MPI_INT, rank, sendTag.getID().id, MPI_COMM_WORLD);
}

void recvLIFNeurons(void **data, int rank, int rankSize)
{
	GLIFNeurons *neurons = (GLIFNeurons *)malloc(sizeof(GLIFNeurons));

	MPI_Status status;
	MPI_Recv((&neurons->num), 1, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(&(neurons->synapsesNum), 1, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	allocLIFNeurons(neurons, neurons->num);

	int size = neurons->num;

	//MPI_Recv(neurons->pID, sizeof(ID)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->pType, sizeof(Type)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_v_init, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_v_rest, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_v_reset, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	//MPI_Recv(neurons->p_cm, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_tau_m, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_tau_refrac, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_tau_syn_E, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_tau_syn_I, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	MPI_Recv(neurons->p_v_thresh, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p_i_offset, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_i_syn, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	MPI_Recv(neurons->p_vm, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	//MPI_Recv(neurons->p__dt, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_C1, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_C2, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_i_tmp, sizeof(real)*size, MPI_BYTE, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_refrac_step, size, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(neurons->p_refrac_time, size, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	int *synapsesNum = (int*)malloc(sizeof(int) * neurons->num);
	int *synapsesLoc = (int*)malloc(sizeof(int) * neurons->num);
	int *synapsesIdx = (int*)malloc(sizeof(int) * neurons->synapsesNum);
	MPI_Recv(synapsesNum, size, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapsesLoc, size, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);
	MPI_Recv(synapsesIdx, neurons->synapsesNum, MPI_INT, rank, recvTag.getID().id, MPI_COMM_WORLD, &status);

	allocLIFConnects(neurons, synapsesNum, synapsesLoc, synapsesIdx, neurons->synapsesNum);

	*data = (void*)neurons;
}
