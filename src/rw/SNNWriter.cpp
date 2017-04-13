/* This program is writen by qp09.
 * usually just for fun.
 * Mon January 18 2016
 */

#include <fstream>

#include "../third_party/json/json.h"
#include "SNNWriter.h"

int writeNetwork(string filename, Network *net)
{
	Json::Value root;
	Json::Value populations(Json::arrayValue);
	Json::Value projections(Json::arrayValue);

	vector<PopulationBase*>::iterator iterP;
	for (iterP=net->pPopulations.begin(); iterP!=net->pPopulations.end(); iterP++) {
		PopulationBase * p = *iterP;
		Json::Value population;
		p->getData(&population);
		populations.append(population);
	}

	for (int i=0; i<(int)net->pPopulations.size()-1; i++) {
		for (int j=i; j<(int)net->pPopulations.size(); j++) {
			Json::Value projection;
			projection["src"] = net->pPopulations[i]->getID().getId();
			projection["dst"] = net->pPopulations[j]->getID().getId();
			Json::Value connections(Json::arrayValue);
			int sizeA = net->pPopulations[i]->getNum();
			int sizeB = net->pPopulations[j]->getNum();
			for (int k=0; k<sizeA; k++) {
				for (int l=0; l<sizeB; l++) {
					Json::Value connection;
					connection["src"] = k;
					connection["dst"] = l;
					vector<ID> sids = net->n2sNetwork[ID(net->pPopulations[i]->getID().getId(), k)];
					for (int m =0; m<(int)sids.size(); m++) {
						if (net->s2nNetwork[sids[m]] == ID(net->pPopulations[j]->getID().getId(), l)) {
							for (vector<SynapseBase*>::iterator iter = net->pSynapses.begin(); iter != net->pSynapses.end(); iter++) {
								if ((*iter)->getID() == sids[m]) {
									(*iter)->getData(&connection);
									connections.append(connection);
								}
							}
						}
					}
				}
			}
			projection["connection"] = connections;
			projections.append(projection);
		}
	}

	root["Populaton"] = populations;
	root["Projection"] = projections;

	std::ofstream fout(filename);
	if (fout.is_open()) {
		Json::StyledWriter writer;
		fout << writer.write(root);
	} else {
		printf("Open file %s failed\n", filename.c_str());
	}
	fout.close();
	return 0;
}
