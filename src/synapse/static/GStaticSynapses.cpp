#include <stdlib.h>
#include <string.h>

#include "GStaticSynapses.h"

size_t getStaticSize()
{
	return sizeof(GStaticSynapses);
}

void *mallocStatic()
{
	GStaticSynapses * p = (GStaticSynapses*)malloc(sizeof(GStaticSynapses)*1);
	memset(p, 0, sizeof(GStaticSynapses)*1);
	return (void*)p;
}

void *allocStatic(int num)
{
	void *p = mallocStatic();
	allocStaticPara(p, num);
	return p;
}

int freeStatic(void *pCPU)
{
	GStaticSynapses *p = (GStaticSynapses*)pCPU;

	free(p->pDst);
	p->pDst = NULL;

	free(p->pWeight);
	p->pWeight = NULL;

	free(p);
	p = NULL;
	return 0;
}

int allocStaticPara(void *pCPU, int num)
{
	GStaticSynapses *p = (GStaticSynapses*)pCPU;

	p->pDst = (int*)malloc(num*sizeof(int));

	p->pWeight = (real*)malloc(num*sizeof(real));

	return 0;
}

int freeStaticPara(void *pCPU)
{
	GStaticSynapses *p = (GStaticSynapses*)pCPU;

	free(p->pDst);
	p->pDst = NULL;

	free(p->pWeight);
	p->pWeight = NULL;

	return 0;
}

int saveStatic(void *pCPU, int num, FILE *f)
{

	GStaticSynapses *p = (GStaticSynapses*)pCPU;
	fwrite(p->pDst, sizeof(int), num, f);

	fwrite(p->pWeight, sizeof(real), num, f);

	return 0;
}

void *loadStatic(int num, FILE *f)
{
	GStaticSynapses *p = (GStaticSynapses*)malloc(sizeof(GStaticSynapses));

	fread(p->pDst, sizeof(int), num, f);

	fread(p->pWeight, sizeof(real), num, f);

	return p;
}

