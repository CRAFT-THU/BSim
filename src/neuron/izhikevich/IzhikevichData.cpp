#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"


#include "IzhikevichData.h"

size_t getIzhikevichSize()
{
	return sizeof(IzhikevichData);
}


void *mallocIzhikevich()
{
	IzhikevichData *p = (IzhikevichData*)malloc(sizeof(IzhikevichData)*1);
	memset(p, 0, sizeof(IzhikevichData)*1);
	return (void*)p;
}


int allocIzhikevichPara(void *pCPU, int num)
{
	IzhikevichData *p = (IzhikevichData*)pCPU;

	p->pV = (real*)malloc(num*sizeof(real));
	memset(p->pV, 0, num*sizeof(real));
	p->pU = (real*)malloc(num*sizeof(real));
	memset(p->pU, 0, num*sizeof(real));
	p->pA = (real*)malloc(num*sizeof(real));
	memset(p->pA, 0, num*sizeof(real));
	p->pB = (real*)malloc(num*sizeof(real));
	memset(p->pB, 0, num*sizeof(real));
	p->pC = (real*)malloc(num*sizeof(real));
	memset(p->pC, 0, num*sizeof(real));
	p->pD = (real*)malloc(num*sizeof(real));
	memset(p->pD, 0, num*sizeof(real));

	return 0;
}


void *allocIzhikevich(int num)
{
	void *p = mallocIzhikevich();
	allocIzhikevichPara(p, num);
	return p;
}


int freeIzhikevichPara(void *pCPU)
{
	IzhikevichData *p = (IzhikevichData*)pCPU;

	free(p->pV);
	p->pV = NULL;
	free(p->pU);
	p->pU = NULL;
	free(p->pA);
	p->pA = NULL;
	free(p->pB);
	p->pB = NULL;
	free(p->pC);
	p->pC = NULL;
	free(p->pD);
	p->pD = NULL;

	return 0;
}


int freeIzhikevich(void *pCPU)
{
	IzhikevichData *p = (IzhikevichData*)pCPU;

	freeIzhikevichPara(p);
	free(p);
	p = NULL;
	return 0;
}


int saveIzhikevich(void *pCPU, int num, FILE *f)
{

	IzhikevichData *p = (IzhikevichData*)pCPU;
	fwrite(p->pV, sizeof(real), num, f);
	fwrite(p->pU, sizeof(real), num, f);
	fwrite(p->pA, sizeof(real), num, f);
	fwrite(p->pB, sizeof(real), num, f);
	fwrite(p->pC, sizeof(real), num, f);
	fwrite(p->pD, sizeof(real), num, f);

	return 0;
}


void *loadIzhikevich(int num, FILE *f)
{
	IzhikevichData *p = (IzhikevichData*)allocIzhikevich(num);

	fread(p->pV, sizeof(real), num, f);
	fread(p->pU, sizeof(real), num, f);
	fread(p->pA, sizeof(real), num, f);
	fread(p->pB, sizeof(real), num, f);
	fread(p->pC, sizeof(real), num, f);
	fread(p->pD, sizeof(real), num, f);

	return p;
}


bool isEqualIzhikevich(void *p1, void *p2, int num)
{
	IzhikevichData *t1 = (IzhikevichData*)p1;
	IzhikevichData *t2 = (IzhikevichData*)p2;

	bool ret = true;
	ret = ret && isEqualArray(t1->pV, t2->pV, num);
	ret = ret && isEqualArray(t1->pU, t2->pU, num);
	ret = ret && isEqualArray(t1->pA, t2->pA, num);
	ret = ret && isEqualArray(t1->pB, t2->pB, num);
	ret = ret && isEqualArray(t1->pC, t2->pC, num);
	ret = ret && isEqualArray(t1->pD, t2->pD, num);

	return ret;
}


