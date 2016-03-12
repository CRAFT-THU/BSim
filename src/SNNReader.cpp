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
#include "Neuron.h"

#define NEURONTYPE Nengo_lowpass
//#define NEURONTYPE IF_curr_exp

using std::ifstream;
using std::cout;
using std::endl;

Json::Value testValue(Json::Value value, unsigned int idx)
{
	if (value.type() == Json::nullValue) {
		return idx;
	}

	if (value.type() == Json::arrayValue) {
		if (idx < value.size()) {
			return value[idx];
		} else {
			cout << "Not enough parameters:" << value << "@" << idx << endl;
		}
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
		int id = input[i]["id"].asInt();
		const Json::Value neurons = input[i]["neurons"];
		int neuronsSize = neurons.size();
		Population<Input_lowpass> *popInput = res->createPopulation(ID(id), neuronsSize, Input_lowpass(InputNeuron(ID(0)), ID(0)));
		for (int j=0; j<neuronsSize; j++) {
			Input_lowpass n(InputNeuron(ID(0)), ID(id, neurons[j].asInt()));
			n.monitorOn();
			popInput->addNeuron(n);
		}
	}

	//Populations
	for (int i=0; i<populationSize; i++) {
		int num = population[i]["size"].asInt();
		int id = population[i]["id"].asInt();
		//Population<NEURONTYPE> *popu =  res->createPopulation(ID(id), num, NEURONTYPE(ID(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
		Population<Nengo_lowpass2> *popu =  res->createPopulation(ID(id), num, Nengo_lowpass2(NengoNeuron(ID(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0), ID(0)));
		for (int j=0; j<num; j++) {
			Json::Value para = population[i]["parameters"];
			//NEURONTYPE n(ID(id, testValue(para["id"], j).asInt()), testValue(para["voltage"], j).asDouble(), testValue(para["v_rest"], j).asDouble(), testValue(para["reset"], j).asDouble(), testValue(para["cm"], j).asDouble(), testValue(para["tau_rc"], j).asDouble(), testValue(para["tau_ref"], j).asDouble(), testValue(para["tau_syn_E"], j).asDouble(), testValue(para["tau_syn_I"], j).asDouble(), testValue(para["threshold"], j).asDouble(), testValue(para["bias"], j).asDouble());
			Nengo_lowpass2 n(NengoNeuron(ID(0), testValue(para["voltage"], j).asDouble(), testValue(para["min_voltage"], j).asDouble(), testValue(para["reset"], j).asDouble(), testValue(para["cm"], j).asDouble(), testValue(para["tau_rc"], j).asDouble(), testValue(para["tau_ref"], j).asDouble(), testValue(para["tau_syn_E"], j).asDouble(), testValue(para["tau_syn_I"], j).asDouble(), testValue(para["threshold"], j).asDouble(), testValue(para["bias"], j).asDouble(), testValue(para["scaled_encoders"], j)[0].asDouble()), ID(id, testValue(para["id"], j).asInt()));
			//popu->addNeuron(n);
		}
	}

	//Projections
	for (int i=0; i<projectionSize; i++) {
		int srcPop = projection[i]["src"].asInt();
		int dstPop = projection[i]["dst"].asInt();
		const Json::Value connection = projection[i]["connections"];
		int num = connection.size();
		for (int j=0; j<num; j++) {
			int srcN = connection[j]["src"].asInt();
			int dstN = connection[j]["dst"].asInt();
			double weight = connection[j]["weight"].asDouble();
			double delay = connection[j]["delay"].asDouble();
			res->connect(srcPop, srcN, dstPop, dstN, weight, delay, 0.005);
		}
	}

	//Output
	for (int i=0; i<outputSize; i++) {
		int id = output[i]["id"].asInt();
		const Json::Value neurons = output[i]["neurons"];
		const Json::Value weights = output[i]["weights"];
		int neuronsSize = neurons.size();
		for (int j=0; j<neuronsSize; j++) {
			res->addOutput(id, j);
			res->addProbe(id, j, weights[0][j].asDouble());
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
