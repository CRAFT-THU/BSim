/* This header file is writen by qp09
 * usually just for fun
 * Fri November 27 2015
 */
#ifndef SNNREADER_H
#define SNNREADER_H

#include <string>

#include "NengoNetwork.h"

using std::string;

NengoNetwork * readNengoNetwork(string filename);
int readNengoInput(NengoNetwork *network, string filename);

#endif /* SNNREADER_H */
