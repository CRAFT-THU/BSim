#include <stdlib.h>
#include <string.h>

#include "GStaticSynapses.h"

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

int freeStatic(void *pCPU, int num)
{
	GStaticSynapses *p = (GStaticSynapses*)pCPU;

	free(p->pDst);

	free(p->pWeight);

	free(p);
	return 0;
}

int allocStaticPara(void *pCPU, int num)
{
	GStaticSynapses *p = (GStaticSynapses*)pCPU;

	p->pDst = (int*)malloc(n*sizeof(int));

	p->pWeight = (real*)malloc(n*sizeof(real));

	return 0;
}

int freeStaticPara(void *pCPU, int num)
{
	GStaticSynapses *p = (GStaticSynapses*)pCPU;

	free(p->pDst);

	free(p->pWeight);

	return 0;
}

int saveStatic(void *pCPU, int num, FILE *f)
{

	GStaticSynapses *p = (GStaticSynapses*)pCPU;
	fwrite(p->pDst, sizeof(int), num, f);

	fwrite(p->pWeight, sizeof(real), num, f);

	return 0;
}

int loadStatic(int num, FILE *f)
{
	GStaticSynapses *p = (GStaticSynapses*)malloc(sizeof(GStaticSynapses));

	fread(p->pDst, sizeof(int), num, f);

	fread(p->pWeight, sizeof(real), num, f);

	return p;
}

