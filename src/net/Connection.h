/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>

struct N2SConnection {
	//int *pSynapsesIdx; 
	//int synapsesNum; 
	int nNum;
	int sNum;

	int maxDelay;
	int minDelay;

	int *delayStart;
	int *delayNum;
	int *revDelayStart;
	int *revDelayNum;
	int *revMap2Sid;
};

N2SConnection * copyConnectionToGPU(N2SConnection * pCPU);

int saveConnection(N2SConnection *conn, FILE *f);
N2SConnection * loadConnection(FILE *f);

#endif /* CONNECTION_H */

