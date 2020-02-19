#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"


#include "STDPData.h"

size_t getSTDPSize()
{
	return sizeof(STDPData);
}

void *mallocSTDP()
{
	STDPData *p = (STDPData*)malloc(sizeof(STDPData)*1);
	memset(p, 0, sizeof(STDPData)*1);
	return (void*)p;
}

int allocSTDPPara(void *pCPU, int num)
{
	STDPData *p = (STDPData*)pCPU;

	p->pDst = (int*)malloc(num*sizeof(int));
	memset(p->pDst, 0, num*sizeof(int));
	p->pLastUpdate = (int*)malloc(num*sizeof(int));
	memset(p->pLastUpdate, 0, num*sizeof(int));

	p->pWeight = (real*)malloc(num*sizeof(real));
	memset(p->pWeight, 0, num*sizeof(real));
	p->pAPre = (real*)malloc(num*sizeof(real));
	memset(p->pAPre, 0, num*sizeof(real));
	p->pAPost = (real*)malloc(num*sizeof(real));
	memset(p->pAPost, 0, num*sizeof(real));
	p->pDPre = (real*)malloc(num*sizeof(real));
	memset(p->pDPre, 0, num*sizeof(real));
	p->pDPost = (real*)malloc(num*sizeof(real));
	memset(p->pDPost, 0, num*sizeof(real));
	p->pTauPre = (real*)malloc(num*sizeof(real));
	memset(p->pTauPre, 0, num*sizeof(real));
	p->pTauPost = (real*)malloc(num*sizeof(real));
	memset(p->pTauPost, 0, num*sizeof(real));

	return 0;
}

void *allocSTDP(int num)
{
	void *p = mallocSTDP();
	allocSTDPPara(p, num);
	return p;
}

int freeSTDPPara(void *pCPU)
{
	STDPData *p = (STDPData*)pCPU;

	free(p->pDst);
	p->pDst = NULL;
	free(p->pLastUpdate);
	p->pLastUpdate = NULL;

	free(p->pWeight);
	p->pWeight = NULL;
	free(p->pAPre);
	p->pAPre = NULL;
	free(p->pAPost);
	p->pAPost = NULL;
	free(p->pDPre);
	p->pDPre = NULL;
	free(p->pDPost);
	p->pDPost = NULL;
	free(p->pTauPre);
	p->pTauPre = NULL;
	free(p->pTauPost);
	p->pTauPost = NULL;

	return 0;
}

int freeSTDP(void *pCPU)
{
	STDPData *p = (STDPData*)pCPU;

	freeSTDPPara(p);
	free(p);
	p = NULL;
	return 0;
}

int saveSTDP(void *pCPU, int num, FILE *f)
{

	STDPData *p = (STDPData*)pCPU;
	fwrite(p->pDst, sizeof(int), num, f);
	fwrite(p->pLastUpdate, sizeof(int), num, f);

	fwrite(p->pWeight, sizeof(real), num, f);
	fwrite(p->pAPre, sizeof(real), num, f);
	fwrite(p->pAPost, sizeof(real), num, f);
	fwrite(p->pDPre, sizeof(real), num, f);
	fwrite(p->pDPost, sizeof(real), num, f);
	fwrite(p->pTauPre, sizeof(real), num, f);
	fwrite(p->pTauPost, sizeof(real), num, f);

	return 0;
}

void *loadSTDP(int num, FILE *f)
{
	STDPData *p = (STDPData*)allocSTDP(num);

	fread(p->pDst, sizeof(int), num, f);
	fread(p->pLastUpdate, sizeof(int), num, f);

	fread(p->pWeight, sizeof(real), num, f);
	fread(p->pAPre, sizeof(real), num, f);
	fread(p->pAPost, sizeof(real), num, f);
	fread(p->pDPre, sizeof(real), num, f);
	fread(p->pDPost, sizeof(real), num, f);
	fread(p->pTauPre, sizeof(real), num, f);
	fread(p->pTauPost, sizeof(real), num, f);

	return p;
}

bool isEqualSTDP(void *p1, void *p2, int num)
{
	STDPData *t1 = (STDPData*)p1;
	STDPData *t2 = (STDPData*)p2;

	bool ret = true;
	ret = ret && isEqualArray(t1->pDst, t2->pDst, num);
	ret = ret && isEqualArray(t1->pLastUpdate, t2->pLastUpdate, num);

	ret = ret && isEqualArray(t1->pWeight, t2->pWeight, num);
	ret = ret && isEqualArray(t1->pAPre, t2->pAPre, num);
	ret = ret && isEqualArray(t1->pAPost, t2->pAPost, num);
	ret = ret && isEqualArray(t1->pDPre, t2->pDPre, num);
	ret = ret && isEqualArray(t1->pDPost, t2->pDPost, num);
	ret = ret && isEqualArray(t1->pTauPre, t2->pTauPre, num);
	ret = ret && isEqualArray(t1->pTauPost, t2->pTauPost, num);

	return ret;
}

