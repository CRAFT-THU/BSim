
#include <stdlib.h>

#include "Connection.h"

int saveConnection(N2SConnection *conn, FILE *f)
{
	fwrite(&(conn->nNum), sizeof(int), 1, f);
	fwrite(&(conn->sNum), sizeof(int), 1, f);
	fwrite(&(conn->maxDelay), sizeof(int), 1, f);
	fwrite(&(conn->minDelay), sizeof(int), 1, f);

	fwrite(conn->delayStart, sizeof(int), conn->nNum, f);
	fwrite(conn->delayNum, sizeof(int), conn->nNum, f);
	fwrite(conn->revDelayStart, sizeof(int), conn->nNum, f);
	fwrite(conn->revDelayNum, sizeof(int), conn->nNum, f);
	fwrite(conn->revMap2Sid, sizeof(int), conn->sNum, f);

	return 0;
}

N2SConnection * loadConnection(FILE *f)
{
	N2SConnection *conn = (N2SConnection *)malloc(sizeof(N2SConnection));

	fread(&(conn->nNum), sizeof(int), 1, f);
	fread(&(conn->sNum), sizeof(int), 1, f);
	fread(&(conn->maxDelay), sizeof(int), 1, f);
	fread(&(conn->minDelay), sizeof(int), 1, f);

	conn->delayStart = (int*)malloc(sizeof(int)*conn->nNum);
	conn->delayNum = (int*)malloc(sizeof(int)*conn->nNum);
	conn->revDelayStart = (int*)malloc(sizeof(int)*conn->nNum);
	conn->revDelayNum = (int*)malloc(sizeof(int)*conn->nNum);
	conn->revMap2Sid = (int*)malloc(sizeof(int)*conn->sNum);
	fread(conn->delayStart, sizeof(int), conn->nNum, f);
	fread(conn->delayNum, sizeof(int), conn->nNum, f);
	fread(conn->revDelayStart, sizeof(int), conn->nNum, f);
	fread(conn->revDelayNum, sizeof(int), conn->nNum, f);
	fread(conn->revMap2Sid, sizeof(int), conn->sNum, f);

	return conn;
}
