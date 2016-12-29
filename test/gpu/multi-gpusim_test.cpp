/* This program is writen by qp09.
 * usually just for fun.
 * Tue December 15 2015
 */

#include "mpi.h"
#include "unistd.h"
#include "../../include/BSim.h"

using namespace std;

const int SIZE = 10000;

int main(int argc, char **argv)
{
	int rank = -1;
	int rankSize = -1;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &rankSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	char hostname[256];
	gethostname(hostname, 255);

	printf("INIT processor %d of %d on host %s\n", rank, rankSize, hostname);

	Network c;
	if (rank == 0) {
		Population<IF_curr_exp> *pn1 = c.createPopulation(nidPool.getID(), SIZE, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 10.0e-1f), ID(0, 0)));
		Population<IF_curr_exp> *pn2 = c.createPopulation(nidPool.getID(), SIZE, IF_curr_exp(LIFNeuron(ID(0, 0), 0.0f, 0.0f, 0.0f, 1.0e-1f, 10.0e-3f, 0.0f, 1.0f, 1.0f, 15.0e-3f, 0), ID(0,0)));
		real * weight = getRandomArray((real)5e-3, SIZE*SIZE);
		real * delay = getConstArray((real)1e-3, SIZE*SIZE);
		c.connect(pn1, pn2, weight, delay, NULL, SIZE*SIZE);
		STSim st(&c, 1.0e-3f);
		st.run(0.1f);
	}

	int sync = 0;
	if (rank == 0) {
		sync = 1;
		for (int idx = 1; idx < rankSize; idx++) {
			MPI_Send(&sync, 1, MPI_INT, idx, 0, MPI_COMM_WORLD);
		}
		printf("SERVER: %d SEND SYNC %d\n", rank, sync);
	} else {
		MPI_Status Status;
		MPI_Recv(&sync, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &Status);
		printf("SERVER: %d RECEIVED SYNC %d\n", rank, sync);
	}

	MGSim mg(&c, 1.0e-3f);
	mg.init(argc, argv);
	mg.run(0.1f);
	
	FILE *logFile = fopen("weight.csv", "w+");
	if (logFile == NULL) {
		printf("ERROR: Open file weight.csv failed\n");
		return -1;
	}

	//for (int i =0; i<10000; i++) {
	//	fprintf(logFile, "%lf", weight[i]);
	//	if (i%100 == 99) {
	//		fprintf(logFile, "\n");
	//	} else {
	//		fprintf(logFile, ", ");
	//	}
	//}
	//fflush(logFile);
	//fclose(logFile);

	MPI_Finalize();

	return 0;
} 
