
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "../utils/utils.h"

#include "Connection.h"

Connection * allocConnection(int nNum, int sNum, int maxDelay, int minDelay)
{
	Connection *ret = (Connection*)malloc(sizeof(Connection));
	assert(ret != NULL);

	ret->nNum = nNum;
	ret->sNum = sNum;
	ret->maxDelay = maxDelay;
	ret->minDelay = minDelay;

	int length = (maxDelay - minDelay + 1) * nNum;

	ret->pDelayStart = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayStart != NULL);
	memset(ret->pDelayStart, 0, sizeof(int)*length);
	ret->pDelayNum = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayNum != NULL);
	memset(ret->pDelayNum, 0, sizeof(int)*length);

	printf("Size of Connection:\t%lu\n", sizeof(int)*length*2);

	ret->pDelayStartRev = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayStartRev != NULL);
	memset(ret->pDelayStartRev, 0, sizeof(int)*length);
	ret->pDelayNumRev = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayNumRev != NULL);
	memset(ret->pDelayNumRev, 0, sizeof(int)*length);
	ret->pSidMapRev = (int*)malloc(sizeof(int)*sNum);
	assert(ret->pSidMapRev != NULL);
	memset(ret->pSidMapRev, 0, sizeof(int)*sNum);

	return ret;
}

int freeConnection(Connection *pCPU)
{
	free(pCPU->pDelayStart);
	free(pCPU->pDelayNum);
	free(pCPU->pDelayStartRev);
	free(pCPU->pDelayNumRev);
	free(pCPU->pSidMapRev);
	free(pCPU);
	pCPU = NULL;
	return 0;
}

int saveConnection(Connection *conn, FILE *f)
{
	fwrite(&(conn->nNum), sizeof(int), 1, f);
	fwrite(&(conn->sNum), sizeof(int), 1, f);
	fwrite(&(conn->maxDelay), sizeof(int), 1, f);
	fwrite(&(conn->minDelay), sizeof(int), 1, f);

	int length = (conn->maxDelay - conn->minDelay + 1) * conn->nNum;

	fwrite(conn->pDelayStart, sizeof(int), length, f);
	fwrite(conn->pDelayNum, sizeof(int), length, f);

	fwrite(conn->pDelayStartRev, sizeof(int), length, f);
	fwrite(conn->pDelayNumRev, sizeof(int), length, f);
	fwrite(conn->pSidMapRev, sizeof(int), conn->sNum, f);

	return 0;
}

Connection * loadConnection(FILE *f)
{
	Connection *conn = (Connection *)malloc(sizeof(Connection));

	fread(&(conn->nNum), sizeof(int), 1, f);
	fread(&(conn->sNum), sizeof(int), 1, f);
	fread(&(conn->maxDelay), sizeof(int), 1, f);
	fread(&(conn->minDelay), sizeof(int), 1, f);

	int length = (conn->maxDelay - conn->minDelay + 1) * conn->nNum;

	conn->pDelayStart = (int*)malloc(sizeof(int)*length);
	conn->pDelayNum = (int*)malloc(sizeof(int)*length);

	conn->pDelayStartRev = (int*)malloc(sizeof(int)*length);
	conn->pDelayNumRev = (int*)malloc(sizeof(int)*length);
	conn->pSidMapRev = (int*)malloc(sizeof(int)*conn->sNum);

	fread(conn->pDelayStart, sizeof(int), length, f);
	fread(conn->pDelayNum, sizeof(int), length, f);

	fread(conn->pDelayStartRev, sizeof(int), length, f);
	fread(conn->pDelayNumRev, sizeof(int), length, f);
	fread(conn->pSidMapRev, sizeof(int), conn->sNum, f);

	return conn;
}


bool isEqualConnection(Connection *c1, Connection *c2)
{
	bool ret = true;
	ret = ret && (c1->nNum == c2->nNum);
	ret = ret && (c1->sNum == c2->sNum);
	ret = ret && (c1->maxDelay == c2->maxDelay);
	ret = ret && (c1->minDelay == c2->minDelay);

	int length = (c1->maxDelay - c1->minDelay + 1) * c1->nNum;

	ret = ret && isEqualArray(c1->pDelayStart, c2->pDelayStart, length);
	ret = ret && isEqualArray(c1->pDelayNum, c2->pDelayNum, length);

	ret = ret && isEqualArray(c1->pDelayStartRev, c2->pDelayStartRev, length);
	ret = ret && isEqualArray(c1->pDelayNumRev, c2->pDelayNumRev, length);

	ret = ret && isEqualArray(c1->pSidMapRev, c2->pSidMapRev, c1->sNum);

	return ret;
}

LolConnection * allocLolConnection(Connection *pConn)
{
	LolConnection *ret = (LolConnection*)malloc(sizeof(LolConnection));
	assert(ret != NULL);

	ret->nNum = pConn->nNum;
	ret->sNum = pConn->sNum;
	ret->maxDelay = pConn->maxDelay;
	ret->minDelay = pConn->minDelay;

	int length = (ret->maxDelay - ret->minDelay + 1) * ret->nNum;

	ret->pIndex = (int**)malloc(sizeof(int*)*length);
	assert(ret->pIndex != NULL);
	memset(ret->pIndex, 0, sizeof(int)*length);
	ret->pNum = (int*)malloc(sizeof(int)*length);
	assert(ret->pNum != NULL);
	memset(ret->pNum, 0, sizeof(int)*length);

	int size_t_ = 0;
	for (int i=0; i<length; i++) {
		ret->pNum[i] = pConn->pDelayNum[i];
		if (ret->pNum[i] > 0) {
			ret->pIndex[i] = (int*)malloc(sizeof(int)*(ret->pNum[i]));
			size_t_ += ret->pNum[i];
			for (int j=0; j<ret->pNum[i]; j++) {
				ret->pIndex[i][j] = pConn->pDelayStart[i] + j;
			}
		} else {
			ret->pIndex[i] = NULL;
		}
	}

	printf("Size of LolConnection:\t%lu\n", (size_t_ + length) * sizeof(int));


	return ret;
}

int freeLolConnection(LolConnection *pCPU)
{
	int length = (pCPU->maxDelay - pCPU->minDelay + 1) * pCPU->nNum;
	for (int i=0; i<length; i++) {
		free(pCPU->pIndex[i]);
	}
	free(pCPU->pIndex);
	free(pCPU->pNum);
	free(pCPU);
	pCPU = NULL;
	return 0;
}

