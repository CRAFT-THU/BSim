#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"


#include "StaticData.h"

size_t getStaticSize()
{
	return sizeof(StaticData);
}

void *mallocStatic()
{
	StaticData *p = (StaticData*)malloc(sizeof(StaticData)*1);
	memset(p, 0, sizeof(StaticData)*1);
	return (void*)p;
}

int allocStaticPara(void *pCPU, int num)
{
	StaticData *p = (StaticData*)pCPU;

	p->pDst = (int*)malloc(num*sizeof(int));
	memset(p->pDst, 0, num*sizeof(int));

	p->pWeight = (real*)malloc(num*sizeof(real));
	memset(p->pWeight, 0, num*sizeof(real));

	return 0;
}

void *allocStatic(int num)
{
	void *p = mallocStatic();
	allocStaticPara(p, num);
	return p;
}

int freeStaticPara(void *pCPU)
{
	StaticData *p = (StaticData*)pCPU;

	free(p->pDst);
	p->pDst = NULL;

	free(p->pWeight);
	p->pWeight = NULL;

	return 0;
}

int freeStatic(void *pCPU)
{
	StaticData *p = (StaticData*)pCPU;

	freeStaticPara(p);
	free(p);
	p = NULL;
	return 0;
}

int saveStatic(void *pCPU, int num, FILE *f)
{

	StaticData *p = (StaticData*)pCPU;
	fwrite(p->pDst, sizeof(int), num, f);

	fwrite(p->pWeight, sizeof(real), num, f);

	return 0;
}

void *loadStatic(int num, FILE *f)
{
	StaticData *p = (StaticData*)allocStatic(num);

	fread(p->pDst, sizeof(int), num, f);

	fread(p->pWeight, sizeof(real), num, f);

	return p;
}

bool isEqualStatic(void *p1, void *p2, int num)
{
	StaticData *t1 = (StaticData*)p1;
	StaticData *t2 = (StaticData*)p2;

	bool ret = true;
	ret = ret && isEqualArray(t1->pDst, t2->pDst, num);

	ret = ret && isEqualArray(t1->pWeight, t2->pWeight, num);

	return ret;
}

