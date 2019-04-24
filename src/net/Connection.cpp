
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

	ret->delayStart = (int*)malloc(sizeof(int)*length);
	assert(ret->delayStart != NULL);
	ret->delayNum = (int*)malloc(sizeof(int)*length);
	assert(ret->delayNum != NULL);

	ret->delayStartRev = (int*)malloc(sizeof(int)*length);
	assert(ret->delayStartRev != NULL);
	ret->delayNumRev = (int*)malloc(sizeof(int)*length);
	assert(ret->delayNumRev != NULL);
	ret->sidMapRev = (int*)malloc(sizeof(int)*sNum);
	assert(ret->sidMapRev != NULL);

	return ret;
}

int saveConnection(Connection *conn, FILE *f)
{
	fwrite(&(conn->nNum), sizeof(int), 1, f);
	fwrite(&(conn->sNum), sizeof(int), 1, f);
	fwrite(&(conn->maxDelay), sizeof(int), 1, f);
	fwrite(&(conn->minDelay), sizeof(int), 1, f);

	fwrite(conn->delayStart, sizeof(int), conn->nNum, f);
	fwrite(conn->delayNum, sizeof(int), conn->nNum, f);

	fwrite(conn->delayStartRev, sizeof(int), conn->nNum, f);
	fwrite(conn->delayNumRev, sizeof(int), conn->nNum, f);
	fwrite(conn->sidMapRev, sizeof(int), conn->sNum, f);

	return 0;
}

Connection * loadConnection(FILE *f)
{
	Connection *conn = (Connection *)malloc(sizeof(Connection));

	fread(&(conn->nNum), sizeof(int), 1, f);
	fread(&(conn->sNum), sizeof(int), 1, f);
	fread(&(conn->maxDelay), sizeof(int), 1, f);
	fread(&(conn->minDelay), sizeof(int), 1, f);

	conn->delayStart = (int*)malloc(sizeof(int)*conn->nNum);
	conn->delayNum = (int*)malloc(sizeof(int)*conn->nNum);

	conn->delayStartRev = (int*)malloc(sizeof(int)*conn->nNum);
	conn->delayNumRev = (int*)malloc(sizeof(int)*conn->nNum);
	conn->sidMapRev = (int*)malloc(sizeof(int)*conn->sNum);

	fread(conn->delayStart, sizeof(int), conn->nNum, f);
	fread(conn->delayNum, sizeof(int), conn->nNum, f);

	fread(conn->delayStartRev, sizeof(int), conn->nNum, f);
	fread(conn->delayNumRev, sizeof(int), conn->nNum, f);
	fread(conn->sidMapRev, sizeof(int), conn->sNum, f);

	return conn;
}
