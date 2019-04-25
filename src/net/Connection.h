/* This header file is writen by qp09
 * usually just for fun
 * Thu December 29 2016
 */
#ifndef CONNECTION_H
#define CONNECTION_H

#include <stdio.h>

struct Connection {
	//int *pSynapsesIdx; 
	//int synapsesNum; 
	int nNum;
	int sNum;

	int maxDelay;
	int minDelay;

	int *pDelayStart;
	int *pDelayNum;

	int *pDelayStartRev;
	int *pDelayNumRev; 
	int *pSidMapRev;
};

Connection * allocConnection(int nNum, int sNum, int maxDelay, int minDelay);

Connection * cudaAllocConnection(Connection * pCPU);

int saveConnection(Connection *conn, FILE *f);
Connection * loadConnection(FILE *f);

#endif /* CONNECTION_H */

