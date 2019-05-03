
#include <stdlib.h>
#include <assert.h>

#include "Connection.h"

Connection * allocConnection(int nNum, int sNum, int maxDelay, int minDelay)
{
	Connection *ret = (Connection*)malloc(sizeof(Connection));
	assert(ret != NULL);

	ret->nNum = nNum;
	ret->nNum = sNum;
	ret->maxDelay = maxDelay;
	ret->minDelay = minDelay;

	int length = (maxDelay - minDelay + 1) * nNum;

	ret->pDelayStart = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayStart != NULL);
	ret->pDelayNum = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayNum != NULL);

	ret->pDelayStartRev = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayStartRev != NULL);
	ret->pDelayNumRev = (int*)malloc(sizeof(int)*length);
	assert(ret->pDelayNumRev != NULL);
	ret->pSidMapRev = (int*)malloc(sizeof(int)*sNum);
	assert(ret->pSidMapRev != NULL);

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

	fwrite(conn->pDelayStart, sizeof(int), conn->nNum, f);
	fwrite(conn->pDelayNum, sizeof(int), conn->nNum, f);

	fwrite(conn->pDelayStartRev, sizeof(int), conn->nNum, f);
	fwrite(conn->pDelayNumRev, sizeof(int), conn->nNum, f);
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

	conn->pDelayStart = (int*)malloc(sizeof(int)*conn->nNum);
	conn->pDelayNum = (int*)malloc(sizeof(int)*conn->nNum);

	conn->pDelayStartRev = (int*)malloc(sizeof(int)*conn->nNum);
	conn->pDelayNumRev = (int*)malloc(sizeof(int)*conn->nNum);
	conn->pSidMapRev = (int*)malloc(sizeof(int)*conn->sNum);

	fread(conn->pDelayStart, sizeof(int), conn->nNum, f);
	fread(conn->pDelayNum, sizeof(int), conn->nNum, f);

	fread(conn->pDelayStartRev, sizeof(int), conn->nNum, f);
	fread(conn->pDelayNumRev, sizeof(int), conn->nNum, f);
	fread(conn->pSidMapRev, sizeof(int), conn->sNum, f);

	return conn;
}
