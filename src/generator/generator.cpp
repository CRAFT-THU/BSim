/* This program is writen by qp09.
 * usually just for fun.
 * Sun January 17 2016
 */

#include <time.h>
#include <fstream>
#include <vector>

#include "generator.h"
#include "../third_party/json/json.h"

using std::vector;

double RAND(double range)
{
	return ((double)rand()/RAND_MAX)*range;
}

void balanced(int populationNum, double connectionRate, double exciteRate, int minSize, double minWeight, double minDelay, int sizeRange, double weightRange, double delayRange)
{
	srand((unsigned)time(NULL));
	Json::Value root;
	Json::Value populations(Json::arrayValue);
	Json::Value projections(Json::arrayValue);

	vector<int> sizeVec(populationNum);

	for (int i=0; i<populationNum; i++) {
		Json::Value population;
		population["id"] = i;
		population["neuron_type"] = 0;
		int size = minSize + (int)(RAND(sizeRange) + 0.5);
		population["size"] = size;
		sizeVec[i] = size;
		population["parameters"]["v_init"] = 1.0;
		population["parameters"]["v_rest"] = 0.0;
		population["parameters"]["v_reset"] = 0.0;
		population["parameters"]["cm"] = 1.0e-1;
		population["parameters"]["tau_m"] = 10.0e-3;
		population["parameters"]["tau_refrac"] = 5.0e-3;
		population["parameters"]["tau_syn_E"] = 1.0;
		population["parameters"]["tau_syn_I"] = 1.0;
		population["parameters"]["v_thresh"] = 15.0e-3;
		population["parameters"]["i_offset"] = 1e-4;
		populations.append(population);
	}

	for (int i=0; i<populationNum - 1; i++) {
		for (int j=i; j<populationNum; j++) {
			Json::Value projection;
			projection["src"] = i;
			projection["dst"] = j;
			Json::Value connections(Json::arrayValue);
			for (int k = 0; k < sizeVec[i]; k++) {
				if (RAND(1.0) <= connectionRate) {
					for (int l =0; l<sizeVec[j]; l++) {
						if (RAND(1.0) <= connectionRate) {
							Json::Value connection;
							connection["src"] = k;
							connection["dst"] = l;
							double weight = minWeight + RAND(weightRange);
							double delay = minDelay + RAND(delayRange);
							if (RAND(1.0) > exciteRate) {
								weight = 0.0-weight;
							}
							connection["weight"] = weight;
							connection["delay"] = delay;
							connections.append(connection);
						}
					}
				}
			}
			projection["connection"] = connections;
			projections.append(projection);
		}
	}
	
	root["Population"] = populations;
	root["Projection"] = projections;

	std::ofstream fout("balanced_net.json");
	Json::StyledWriter writer;
	fout << writer.write(root);
	fout.close();
}

void unbalanced(int populationNum, double connectionRate, double exciteRate, int minSize, double minWeight, double minDelay, int sizeRange, double weightRange, double delayRange)
{
	srand((unsigned)time(NULL));
	Json::Value root;
	Json::Value populations(Json::arrayValue);
	Json::Value projections(Json::arrayValue);

	vector<int> sizeVec(populationNum);

	for (int i=0; i<populationNum; i++) {
		Json::Value population;
		population["id"] = i;
		population["neuron_type"] = 0;
		int size = minSize + (int)(RAND(sizeRange) + 0.5);
		population["size"] = size;
		sizeVec[i] = size;
		population["parameters"]["v_init"] = 1.0;
		population["parameters"]["v_rest"] = 0.0;
		population["parameters"]["v_reset"] = 0.0;
		population["parameters"]["cm"] = 1.0e-1;
		population["parameters"]["tau_m"] = 10.0e-3;
		population["parameters"]["tau_refrac"] = 5.0e-3;
		population["parameters"]["tau_syn_E"] = 1.0;
		population["parameters"]["tau_syn_I"] = 1.0;
		population["parameters"]["v_thresh"] = 15.0e-3;
		population["parameters"]["i_offset"] = 1e-4;
		populations.append(population);
	}

	for (int i=0; i<populationNum - 1; i++) {
		for (int j=0; j<=i; j++) {
			Json::Value projection;
			projection["src"] = i;
			projection["dst"] = j;
			Json::Value connections(Json::arrayValue);
			for (int k = 0; k < sizeVec[i]; k++) {
				if (RAND(1.0) <= connectionRate) {
					for (int l =0; l<sizeVec[j]; l++) {
						if (RAND(1.0) <= connectionRate) {
							Json::Value connection;
							connection["src"] = k;
							connection["dst"] = l;
							double weight = minWeight + RAND(weightRange);
							double delay = minDelay + RAND(delayRange);
							if (RAND(1.0) > exciteRate) {
								weight = 0.0-weight;
							}
							connection["weight"] = weight;
							connection["delay"] = delay;
							connections.append(connection);
						}
					}
				}
			}
			projection["connection"] = connections;
			projections.append(projection);
		}
	}
	
	root["Population"] = populations;
	root["Projection"] = projections;

	std::ofstream fout("unbalanced_net.json");
	Json::StyledWriter writer;
	fout << writer.write(root);
	fout.close();
}
