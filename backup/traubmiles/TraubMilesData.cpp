#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"


#include "TraubMilesData.h"

size_t getTraubMilesSize()
{
	return sizeof(TraubMilesData);
}


void *mallocTraubMiles()
{
	TraubMilesData *p = (TraubMilesData*)malloc(sizeof(TraubMilesData)*1);
	memset(p, 0, sizeof(TraubMilesData)*1);
	return (void*)p;
}


int allocTraubMilesPara(void *pCPU, int num)
{
	TraubMilesData *p = (TraubMilesData*)pCPU;

	p->pGNa = (real*)malloc(num*sizeof(real));
	memset(p->pGNa, 0, num*sizeof(real));
	p->pENa = (real*)malloc(num*sizeof(real));
	memset(p->pENa, 0, num*sizeof(real));
	p->pGK = (real*)malloc(num*sizeof(real));
	memset(p->pGK, 0, num*sizeof(real));
	p->pEK = (real*)malloc(num*sizeof(real));
	memset(p->pEK, 0, num*sizeof(real));
	p->pGl = (real*)malloc(num*sizeof(real));
	memset(p->pGl, 0, num*sizeof(real));
	p->pEl = (real*)malloc(num*sizeof(real));
	memset(p->pEl, 0, num*sizeof(real));
	p->pC = (real*)malloc(num*sizeof(real));
	memset(p->pC, 0, num*sizeof(real));
	p->pV = (real*)malloc(num*sizeof(real));
	memset(p->pV, 0, num*sizeof(real));
	p->pM = (real*)malloc(num*sizeof(real));
	memset(p->pM, 0, num*sizeof(real));
	p->pH = (real*)malloc(num*sizeof(real));
	memset(p->pH, 0, num*sizeof(real));
	p->pN = (real*)malloc(num*sizeof(real));
	memset(p->pN, 0, num*sizeof(real));
	p->pDecayE = (real*)malloc(num*sizeof(real));
	memset(p->pDecayE, 0, num*sizeof(real));
	p->pDecayI = (real*)malloc(num*sizeof(real));
	memset(p->pDecayI, 0, num*sizeof(real));
	p->pEE = (real*)malloc(num*sizeof(real));
	memset(p->pEE, 0, num*sizeof(real));
	p->pEI = (real*)malloc(num*sizeof(real));
	memset(p->pEI, 0, num*sizeof(real));

	return 0;
}


void *allocTraubMiles(int num)
{
	void *p = mallocTraubMiles();
	allocTraubMilesPara(p, num);
	return p;
}


int freeTraubMilesPara(void *pCPU)
{
	TraubMilesData *p = (TraubMilesData*)pCPU;

	free(p->pGNa);
	p->pGNa = NULL;
	free(p->pENa);
	p->pENa = NULL;
	free(p->pGK);
	p->pGK = NULL;
	free(p->pEK);
	p->pEK = NULL;
	free(p->pGl);
	p->pGl = NULL;
	free(p->pEl);
	p->pEl = NULL;
	free(p->pC);
	p->pC = NULL;
	free(p->pV);
	p->pV = NULL;
	free(p->pM);
	p->pM = NULL;
	free(p->pH);
	p->pH = NULL;
	free(p->pN);
	p->pN = NULL;
	free(p->pDecayE);
	p->pDecayE = NULL;
	free(p->pDecayI);
	p->pDecayI = NULL;
	free(p->pEE);
	p->pEE = NULL;
	free(p->pEI);
	p->pEI = NULL;

	return 0;
}


int freeTraubMiles(void *pCPU)
{
	TraubMilesData *p = (TraubMilesData*)pCPU;

	freeTraubMilesPara(p);
	free(p);
	p = NULL;
	return 0;
}


int saveTraubMiles(void *pCPU, int num, FILE *f)
{

	TraubMilesData *p = (TraubMilesData*)pCPU;
	fwrite(p->pGNa, sizeof(real), num, f);
	fwrite(p->pENa, sizeof(real), num, f);
	fwrite(p->pGK, sizeof(real), num, f);
	fwrite(p->pEK, sizeof(real), num, f);
	fwrite(p->pGl, sizeof(real), num, f);
	fwrite(p->pEl, sizeof(real), num, f);
	fwrite(p->pC, sizeof(real), num, f);
	fwrite(p->pV, sizeof(real), num, f);
	fwrite(p->pM, sizeof(real), num, f);
	fwrite(p->pH, sizeof(real), num, f);
	fwrite(p->pN, sizeof(real), num, f);
	fwrite(p->pDecayE, sizeof(real), num, f);
	fwrite(p->pDecayI, sizeof(real), num, f);
	fwrite(p->pEE, sizeof(real), num, f);
	fwrite(p->pEI, sizeof(real), num, f);

	return 0;
}


void *loadTraubMiles(int num, FILE *f)
{
	TraubMilesData *p = (TraubMilesData*)allocTraubMiles(num);

	fread(p->pGNa, sizeof(real), num, f);
	fread(p->pENa, sizeof(real), num, f);
	fread(p->pGK, sizeof(real), num, f);
	fread(p->pEK, sizeof(real), num, f);
	fread(p->pGl, sizeof(real), num, f);
	fread(p->pEl, sizeof(real), num, f);
	fread(p->pC, sizeof(real), num, f);
	fread(p->pV, sizeof(real), num, f);
	fread(p->pM, sizeof(real), num, f);
	fread(p->pH, sizeof(real), num, f);
	fread(p->pN, sizeof(real), num, f);
	fread(p->pDecayE, sizeof(real), num, f);
	fread(p->pDecayI, sizeof(real), num, f);
	fread(p->pEE, sizeof(real), num, f);
	fread(p->pEI, sizeof(real), num, f);

	return p;
}


bool isEqualTraubMiles(void *p1, void *p2, int num)
{
	TraubMilesData *t1 = (TraubMilesData*)p1;
	TraubMilesData *t2 = (TraubMilesData*)p2;

	bool ret = true;
	ret = ret && isEqualArray(t1->pGNa, t2->pGNa, num);
	ret = ret && isEqualArray(t1->pENa, t2->pENa, num);
	ret = ret && isEqualArray(t1->pGK, t2->pGK, num);
	ret = ret && isEqualArray(t1->pEK, t2->pEK, num);
	ret = ret && isEqualArray(t1->pGl, t2->pGl, num);
	ret = ret && isEqualArray(t1->pEl, t2->pEl, num);
	ret = ret && isEqualArray(t1->pC, t2->pC, num);
	ret = ret && isEqualArray(t1->pV, t2->pV, num);
	ret = ret && isEqualArray(t1->pM, t2->pM, num);
	ret = ret && isEqualArray(t1->pH, t2->pH, num);
	ret = ret && isEqualArray(t1->pN, t2->pN, num);
	ret = ret && isEqualArray(t1->pDecayE, t2->pDecayE, num);
	ret = ret && isEqualArray(t1->pDecayI, t2->pDecayI, num);
	ret = ret && isEqualArray(t1->pEE, t2->pEE, num);
	ret = ret && isEqualArray(t1->pEI, t2->pEI, num);

	return ret;
}


