#include <stdlib.h>
#include <string.h>

#include "../../utils/utils.h"


#include "LIFData.h"

size_t getLIFSize()
{
	return sizeof(LIFData);
}

void *mallocLIF()
{
	LIFData *p = (LIFData*)malloc(sizeof(LIFData)*1);
	memset(p, 0, sizeof(LIFData)*1);
	return (void*)p;
}

int allocLIFPara(void *pCPU, int num)
{
	LIFData *p = (LIFData*)pCPU;

	p->pRefracTime = (int*)malloc(num*sizeof(int));
	memset(p->pRefracTime, 0, num*sizeof(int));
	p->pRefracStep = (int*)malloc(num*sizeof(int));
	memset(p->pRefracStep, 0, num*sizeof(int));

	p->pI_e = (real*)malloc(num*sizeof(real));
	memset(p->pI_e, 0, num*sizeof(real));
	p->pV_i = (real*)malloc(num*sizeof(real));
	memset(p->pV_i, 0, num*sizeof(real));
	p->pCe = (real*)malloc(num*sizeof(real));
	memset(p->pCe, 0, num*sizeof(real));
	p->pV_reset = (real*)malloc(num*sizeof(real));
	memset(p->pV_reset, 0, num*sizeof(real));
	p->pV_e = (real*)malloc(num*sizeof(real));
	memset(p->pV_e, 0, num*sizeof(real));
	p->pV_tmp = (real*)malloc(num*sizeof(real));
	memset(p->pV_tmp, 0, num*sizeof(real));
	p->pI_i = (real*)malloc(num*sizeof(real));
	memset(p->pI_i, 0, num*sizeof(real));
	p->pV_thresh = (real*)malloc(num*sizeof(real));
	memset(p->pV_thresh, 0, num*sizeof(real));
	p->pCi = (real*)malloc(num*sizeof(real));
	memset(p->pCi, 0, num*sizeof(real));
	p->pV_m = (real*)malloc(num*sizeof(real));
	memset(p->pV_m, 0, num*sizeof(real));
	p->pC_e = (real*)malloc(num*sizeof(real));
	memset(p->pC_e, 0, num*sizeof(real));
	p->pC_m = (real*)malloc(num*sizeof(real));
	memset(p->pC_m, 0, num*sizeof(real));
	p->pC_i = (real*)malloc(num*sizeof(real));
	memset(p->pC_i, 0, num*sizeof(real));

	return 0;
}

void *allocLIF(int num)
{
	void *p = mallocLIF();
	allocLIFPara(p, num);
	return p;
}

int freeLIFPara(void *pCPU)
{
	LIFData *p = (LIFData*)pCPU;

	free(p->pRefracTime);
	p->pRefracTime = NULL;
	free(p->pRefracStep);
	p->pRefracStep = NULL;

	free(p->pI_e);
	p->pI_e = NULL;
	free(p->pV_i);
	p->pV_i = NULL;
	free(p->pCe);
	p->pCe = NULL;
	free(p->pV_reset);
	p->pV_reset = NULL;
	free(p->pV_e);
	p->pV_e = NULL;
	free(p->pV_tmp);
	p->pV_tmp = NULL;
	free(p->pI_i);
	p->pI_i = NULL;
	free(p->pV_thresh);
	p->pV_thresh = NULL;
	free(p->pCi);
	p->pCi = NULL;
	free(p->pV_m);
	p->pV_m = NULL;
	free(p->pC_e);
	p->pC_e = NULL;
	free(p->pC_m);
	p->pC_m = NULL;
	free(p->pC_i);
	p->pC_i = NULL;

	return 0;
}

int freeLIF(void *pCPU)
{
	LIFData *p = (LIFData*)pCPU;

	freeLIFPara(p);
	free(p);
	p = NULL;
	return 0;
}

int saveLIF(void *pCPU, int num, FILE *f)
{

	LIFData *p = (LIFData*)pCPU;
	fwrite(p->pRefracTime, sizeof(int), num, f);
	fwrite(p->pRefracStep, sizeof(int), num, f);

	fwrite(p->pI_e, sizeof(real), num, f);
	fwrite(p->pV_i, sizeof(real), num, f);
	fwrite(p->pCe, sizeof(real), num, f);
	fwrite(p->pV_reset, sizeof(real), num, f);
	fwrite(p->pV_e, sizeof(real), num, f);
	fwrite(p->pV_tmp, sizeof(real), num, f);
	fwrite(p->pI_i, sizeof(real), num, f);
	fwrite(p->pV_thresh, sizeof(real), num, f);
	fwrite(p->pCi, sizeof(real), num, f);
	fwrite(p->pV_m, sizeof(real), num, f);
	fwrite(p->pC_e, sizeof(real), num, f);
	fwrite(p->pC_m, sizeof(real), num, f);
	fwrite(p->pC_i, sizeof(real), num, f);

	return 0;
}

void *loadLIF(int num, FILE *f)
{
	LIFData *p = (LIFData*)allocLIF(num);

	fread(p->pRefracTime, sizeof(int), num, f);
	fread(p->pRefracStep, sizeof(int), num, f);

	fread(p->pI_e, sizeof(real), num, f);
	fread(p->pV_i, sizeof(real), num, f);
	fread(p->pCe, sizeof(real), num, f);
	fread(p->pV_reset, sizeof(real), num, f);
	fread(p->pV_e, sizeof(real), num, f);
	fread(p->pV_tmp, sizeof(real), num, f);
	fread(p->pI_i, sizeof(real), num, f);
	fread(p->pV_thresh, sizeof(real), num, f);
	fread(p->pCi, sizeof(real), num, f);
	fread(p->pV_m, sizeof(real), num, f);
	fread(p->pC_e, sizeof(real), num, f);
	fread(p->pC_m, sizeof(real), num, f);
	fread(p->pC_i, sizeof(real), num, f);

	return p;
}

bool isEqualLIF(void *p1, void *p2, int num)
{
	LIFData *t1 = (LIFData*)p1;
	LIFData *t2 = (LIFData*)p2;

	bool ret = true;
	ret = ret && isEqualArray(t1->pRefracTime, t2->pRefracTime, num);
	ret = ret && isEqualArray(t1->pRefracStep, t2->pRefracStep, num);

	ret = ret && isEqualArray(t1->pI_e, t2->pI_e, num);
	ret = ret && isEqualArray(t1->pV_i, t2->pV_i, num);
	ret = ret && isEqualArray(t1->pCe, t2->pCe, num);
	ret = ret && isEqualArray(t1->pV_reset, t2->pV_reset, num);
	ret = ret && isEqualArray(t1->pV_e, t2->pV_e, num);
	ret = ret && isEqualArray(t1->pV_tmp, t2->pV_tmp, num);
	ret = ret && isEqualArray(t1->pI_i, t2->pI_i, num);
	ret = ret && isEqualArray(t1->pV_thresh, t2->pV_thresh, num);
	ret = ret && isEqualArray(t1->pCi, t2->pCi, num);
	ret = ret && isEqualArray(t1->pV_m, t2->pV_m, num);
	ret = ret && isEqualArray(t1->pC_e, t2->pC_e, num);
	ret = ret && isEqualArray(t1->pC_m, t2->pC_m, num);
	ret = ret && isEqualArray(t1->pC_i, t2->pC_i, num);

	return ret;
}

