/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "SingleThreadSimulator.h"

SingleThreadSimulator::SingleThreadSimulator(Network *network, real dt)
	: SimulatorBase(network, dt)
{
}

SingleThreadSimulator::~SingleThreadSimulator()
{
}

int SingleThreadSimulator::run(real time)
{
	int sim_cycle =  round((time)/dt);

	FILE *v_file = fopen("v.data", "w+");
	if (v_file == NULL) {
		printf("Open file v.data failed\n");
		return -1;
	}

	FILE *input_e_file = fopen("input_e.data", "w+");
	if (input_e_file == NULL) {
		printf("Open file input_e.data failed\n");
		return -1;
	}

	FILE *input_i_file = fopen("input_i.data", "w+");
	if (input_i_file == NULL) {
		printf("Open file input_i.data failed\n");
		return -1;
	}

	FILE *ie_file = fopen("ie.data", "w+");
	if (ie_file == NULL) {
		printf("Open file ie.data failed\n");
		return -1;
	}

	FILE *ii_file = fopen("ii.data", "w+");
	if (ii_file == NULL) {
		printf("Open file ii.data failed\n");
		return -1;
	}

	FILE *log_file = fopen("Sim.log", "w+");
	if (log_file == NULL) {
		printf("Open file Sim.log failed\n");
		return -1;
	}
	
	//FILE *outFile = fopen("Output.csv", "w+");
	//if (outFile == NULL) {
	//	printf("Open file Sim.log failed\n");
	//	return -2;
	//} else {
	//	fprintf(outFile, "Cycle");
	//	int size = network->pOutputs.size();
	//	for (int i=0; i<size; i++) {
	//		fprintf(outFile, ",%s", network->pOutputs[i]->getID().getInfo().c_str());
	//	}
	//	fprintf(outFile, "\n");
	//}

	reset();
	vector<SynapseBase*>::iterator iterS;
	vector<NeuronBase*>::iterator iterN;
	vector<PopulationBase*>::iterator iterP;

	SimInfo info;
	info.currCycle = 0;
	info.dt = dt;

	printf("Start runing for %d cycles\n", sim_cycle);
	struct timeval ts, te;
	gettimeofday(&ts, NULL);

	for (int cycle=0; cycle<sim_cycle; cycle++) {
		//printf("\rCycle: %d", cycle);
		//fflush(stdout);

		info.currCycle = cycle;
		info.fired.clear();
		info.input.clear();

		//Update
		network->update(info);

		//Log info
		network->monitor(info);

#ifdef LOG_DATA
		int isize = info.input.size();
		//fprintf(dataFile, "Cycle %d: ", info.currCycle);
		int total_record = 5;
		for (int i=0; i<isize; i++) {
			if (i%total_record == 0) {
				fprintf(v_file, "%.10lf \t", info.input[i]);
			} else if (i%total_record == 1) {
				fprintf(input_e_file, "%.10lf \t", info.input[i]);
			} else if (i%total_record == 2) {
				fprintf(input_i_file, "%.10lf \t", info.input[i]);
			} else if (i%total_record == 3) {
				fprintf(ie_file, "%.10lf \t", info.input[i]);
			} else {
				fprintf(ii_file, "%.10lf \t", info.input[i]);
			}
		}

		fprintf(v_file, "\n");
		fprintf(input_e_file, "\n");
		fprintf(input_i_file, "\n");
		fprintf(ie_file, "\n");
		fprintf(ii_file, "\n");

		int size = info.fired.size();
		//fprintf(logFile, "Cycle %d: ", info.currCycle);
		for (int i=0; i<size; i++) {
			if (info.fired[i]) {
				fprintf(log_file, "%d ", i);
			}
		}
		fprintf(log_file, "\n");

		//size = network->pOutputs.size();
		//fprintf(outFile, "%d", info.currCycle);
		//if (size > 0) {
		//	fprintf(outFile, ",%d", network->pOutputs[0]->isFired());
		//	for (int i=1; i<size; i++) {
		//		fprintf(outFile, ",%d", network->pOutputs[i]->isFired());
		//	}
		//}
		//fprintf(outFile, "\n");
#endif
	}
	gettimeofday(&te, NULL);
	long seconds = te.tv_sec - ts.tv_sec;
	long hours = seconds/3600;
	seconds = seconds%3600;
	long minutes = seconds/60;
	seconds = seconds%60;
	long uSeconds = te.tv_usec - ts.tv_usec;
	if (uSeconds < 0) {
		uSeconds += 1000000;
		seconds = seconds - 1;
	}

	printf("\nSimulation finished in %ld:%ld:%ld.%06lds\n", hours, minutes, seconds, uSeconds);
	fclose(v_file);
	fclose(input_e_file);
	fclose(input_i_file);
	fclose(ie_file);
	fclose(ii_file);
	fclose(log_file);
	//fclose(outFile);

	FILE *rateFile = fopen("Fire.log", "w+");
	if (rateFile == NULL) {
		printf("Open file Fire.log failed\n");
		return -1;
	}

	for (auto piter = network->pPopulations.begin(); piter != network->pPopulations.end(); piter++) {
		PopulationBase * p = *piter;
		for (int i=0; i<p->getNum(); i++) {
			int rate = p->getNeuron(i)->getFireCount();
			fprintf(rateFile, "%d \t", rate);
		}
	}

	fclose(rateFile);

	return 0;
}
