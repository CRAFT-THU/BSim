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

int freeConnection(Connection * pCPU);

Connection * cudaAllocConnection(Connection * pCPU);
int cudaFetchConnection(Connection *pCPU, Connection *pGPU);
int cudaFreeConnection(Connection *pGPU);

int saveConnection(Connection *conn, FILE *f);
Connection * loadConnection(FILE *f);

bool isEqualConnection(Connection *c1, Connection *c2);

struct LolConnection {
	int nNum;
	int sNum;

	int maxDelay;
	int minDelay;

	int **pIndex;
	int *pNum;
};

// LolConnection * allocLolConnection(int nNum, int sNum, int maxDelay, int minDelay);
LolConnection * allocLolConnection(Connection *pConn);

int freeLolConnection(LolConnection * pCPU);

LolConnection * cudaAllocLolConnection(LolConnection * pCPU);
// int cudaFetchLolConnection(LolConnection *pCPU, LolConnection *pGPU);
int cudaFreeLolConnection(LolConnection *pGPU);

// int saveLolConnection(Connection *conn, FILE *f);
// LolConnection * loadLolConnection(FILE *f);
// 
// bool isEqualLolConnection(LolConnection *c1, LolConnection *c2);

#endif /* CONNECTION_H */

