/* This program is writen by qp09.
 * usually just for fun.
 * Tue January 12 2016
 */

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>

#include "../utils/json/json.h"
#include "NengoReader.h"

#define NEURONTYPE Nengo_lowpass
//#define NEURONTYPE IF_curr_exp

using std::ifstream;
using std::cout;
using std::endl;

NengoNetwork * readNengoNetwork(string filename)
{
	ifstream fin(filename);
	assert(fin.is_open());

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(fin, root, false)) {
		printf("File %s parse failed!\n", filename.c_str());
		return NULL;
	}

	NengoNetwork * res = new NengoNetwork;

	//Json::FastWriter writer;
	//std::cout << writer.write(root) << std::endl;

	int inputSize = root["Inputs"].size();
	int outputSize = root["Outputs"].size();
	int populationSize = root["Populations"].size();
	int projectionSize = root["Projections"].size();
	const Json::Value input = root["Inputs"];
	const Json::Value output = root["Outputs"];
	const Json::Value population = root["Populations"];
	const Json::Value projection = root["Projections"];

	//Inputs
	for (int i=0; i<inputSize; i++) {
		res->createInput(input[i]);
	}

	//Populations
	for (int i=0; i<populationSize; i++) {
		res->createPopulation(population[i]);
	}

	//Projections
	for (int i=0; i<projectionSize; i++) {
		int srcPop = projection[i]["src"].asInt();
		int dstPop = projection[i]["dst"].asInt();
		NengoBase *pSrc = res->findPopulation(srcPop);
		NengoBase *pDst = res->findPopulation(dstPop);
		int srcSize = pSrc->getSize();
		int dstSize = pDst->getSize();
		real *weights = new real[srcSize * dstSize];
		const Json::Value connection = projection[i]["connections"];
		int num = connection.size();
		for (int j=0; j<num; j++) {
			int srcN = connection[j]["src"].asInt();
			int dstN = connection[j]["dst"].asInt();
			double weight = connection[j]["weight"].asDouble();
			weights[dstN * srcSize + srcN] = weight;
		}
		res->connect(pSrc, pDst, weights, 0.005);
	}

	//Output
	for (int i=0; i<outputSize; i++) {
		int id = output[i]["id"].asInt();
		const Json::Value neurons = output[i]["neurons"];
		const Json::Value weights = output[i]["weights"];
		int neuronsSize = neurons.size();
		real *weight = new real[neuronsSize];
		for (int j=0; j<neuronsSize; j++) {
			weight[j] = weights[0][j].asDouble();
		}
		res->probe(id, weight);
	}

	fin.close();
	
	return res;
}

int readNengoInput(NengoNetwork *network, string filename)
{
	ifstream fin(filename);
	assert(fin.is_open());

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(fin, root, false)) {
		printf("File %s parse failed!\n", filename.c_str());
		return -1;
	}

	int cycles = root["Cycles"].asInt();
	Json::Value inputs = root["Inputs"];
	int size = inputs.size();
	for (int i=0; i<size; i++) {
		int cycle = inputs[i]["cycle"].asInt();
		Json::Value  neurons = inputs[i]["neurons"];
		int neuronsSize = neurons.size();

		for (int j=0; j<neuronsSize; j++) {
			int pId = neurons[j]["populationID"].asInt();
			int nId = neurons[j]["neuronID"].asInt();
			InputPopulation *pN = (InputPopulation*)network->findPopulation(pId);
			pN->addFireTime(nId, cycle);
		}
	}

	fin.close();

	return cycles;

}
