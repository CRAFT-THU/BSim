/* This program is writen by qp09.
 * usually just for fun.
 * Tue January 12 2016
 */

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cassert>

#include "./utils/json/json.h"
#include "SNNReader.h"

//#define NEURONTYPE Nengo_lowpass
#define NEURONTYPE IF_curr_exp

using std::ifstream;

Json::Value testValue(Json::Value value, unsigned int idx)
{
	if (value.type() == Json::nullValue) {
		return idx;
	}

	if (value.type() == Json::arrayValue) {
		return value[idx];
	} 

	return value;
}

Network * readNetwork(string filename)
{
	ifstream fin(filename);
	assert(fin.is_open());

	Json::Reader reader;
	Json::Value root;

	if (!reader.parse(fin, root, false)) {
		printf("File %s parse failed!\n", filename.c_str());
		return NULL;
	}

	Network * res = new Network;

	//Json::FastWriter writer;
	//std::cout << writer.write(root) << std::endl;

	unsigned int inputSize = root["Inputs"].size();
	unsigned int outputSize = root["Outputs"].size();
	unsigned int populationSize = root["Populations"].size();
	unsigned int projectionSize = root["Projections"].size();
	const Json::Value input = root["Inputs"];
	const Json::Value output = root["Outputs"];
	const Json::Value population = root["Populations"];
	const Json::Value projection = root["Projections"];

	//Input
	for (unsigned int i=0; i<inputSize; i++) {
		unsigned int id = input[i]["id"].asUInt();
		const Json::Value neurons = input[i]["neurons"];
		unsigned int neuronsSize = neurons.size();
		Population<InputNeuron> *popInput = res->createPopulation(ID(id), neuronsSize, InputNeuron(ID(0)));
		for (unsigned int j=0; j<neuronsSize; j++) {
			InputNeuron n(ID(id, neurons[j].asUInt()));
			n.monitorOn();
			popInput->addNeuron(n);
		}
	}

	//Neurons
	for (unsigned int i=0; i<populationSize; i++) {
		unsigned int num = population[i]["size"].asUInt();
		unsigned int id = population[i]["id"].asUInt();
		Population<NEURONTYPE> *popu =  res->createPopulation(ID(id), num, NEURONTYPE(ID(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
		for (unsigned int j=0; j<num; j++) {
			Json::Value para = population[i]["parameters"];
			NEURONTYPE n(ID(id, testValue(para["id"], j).asUInt()), testValue(para["voltage"], j).asDouble(), testValue(para["v_rest"], j).asDouble(), testValue(para["reset"], j).asDouble(), testValue(para["cm"], j).asDouble(), testValue(para["tau_rc"], j).asDouble(), testValue(para["tau_ref"], j).asDouble(), testValue(para["tau_syn_E"], j).asDouble(), testValue(para["tau_syn_I"], j).asDouble(), testValue(para["threshold"], j).asDouble(), testValue(para["bias"], j).asDouble());
			popu->addNeuron(n);
		}
	}

	//Projections
	for (unsigned int i=0; i<projectionSize; i++) {
		unsigned int srcPop = projection[i]["src"].asUInt();
		unsigned int dstPop = projection[i]["dst"].asUInt();
		const Json::Value connection = projection[i]["connection"];
		unsigned int num = connection.size();
		for (unsigned int j=0; j<num; j++) {
			unsigned int srcN = connection[j]["src"].asUInt();
			unsigned int dstN = connection[j]["dst"].asUInt();
			double weight = connection[j]["weight"].asDouble();
			double delay = connection[j]["delay"].asDouble();
			res->connect(srcPop, srcN, dstPop, dstN, weight, delay);
		}
	}

	//Output
	for (unsigned int i=0; i<outputSize; i++) {
		unsigned int id = output[i]["id"].asUInt();
		const Json::Value neurons = output[i]["neurons"];
		unsigned int neuronsSize = neurons.size();
		for (unsigned int j=0; j<neuronsSize; j++) {
			res->addOutput(id, j);
		}
	}

	fin.close();
	
	return res;
}

int readInput(Network *network, string filename)
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
			InputNeuron *pN = (InputNeuron*)network->findNeuron(pId, nId);
			pN->addFireTime(cycle);
		}
	}

	fin.close();

	return cycles;

}
