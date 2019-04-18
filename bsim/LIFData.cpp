#include <stdlib.h>

#include "LIFData.h"

void *mallocLIF()
{
	void *p = malloc(sizeof(LIFData));
	return p;
}

void *allocLIF(int n)
{
	void *p = mallocLIF();
	allocLIFPara(p, n);
	return p;
}

int freeLIF(void *pCpu, int n)
{
	LIFData *p = (LIFData*)pCpu;

	free(p->pRefracStep);
	free(p->pRefracTime);

	free(p->pVm);
	free(p->pCi);
	free(p->pCe);
	free(p->pC_i);
	free(p->pC_e);
	free(p->pVtmp);
	free(p->pIi);
	free(p->pIe);
	free(p->pVthresh);
	free(p->pVreset);
	free(p->pCm);

	free(p);
	return 0;
}

int allocLIFPara(void *pCpu, int n)
{
	LIFData *p = (LIFData*)pCpu;

	p->pRefracStep = (int*)malloc(n*sizeof(int));
	p->pRefracTime = (int*)malloc(n*sizeof(int));

	p->pVm = (real*)malloc(n*sizeof(real));
	p->pCi = (real*)malloc(n*sizeof(real));
	p->pCe = (real*)malloc(n*sizeof(real));
	p->pC_i = (real*)malloc(n*sizeof(real));
	p->pC_e = (real*)malloc(n*sizeof(real));
	p->pVtmp = (real*)malloc(n*sizeof(real));
	p->pIi = (real*)malloc(n*sizeof(real));
	p->pIe = (real*)malloc(n*sizeof(real));
	p->pVthresh = (real*)malloc(n*sizeof(real));
	p->pVreset = (real*)malloc(n*sizeof(real));
	p->pCm = (real*)malloc(n*sizeof(real));

	return 0;
}

int freeLIFPara(void *pCpu, int n)
{
	LIFData *p = (LIFData*)pCpu;

	free(p->pRefracStep);
	free(p->pRefracTime);

	free(p->pVm);
	free(p->pCi);
	free(p->pCe);
	free(p->pC_i);
	free(p->pC_e);
	free(p->pVtmp);
	free(p->pIi);
	free(p->pIe);
	free(p->pVthresh);
	free(p->pVreset);
	free(p->pCm);

	return 0;
}

int saveLIF(void *pCpu, int n, FILE *f)
{

	LIFData *p = (LIFData*)pCpu;
	fwrite(p->pRefracStep, sizeof(int), n, f);
	fwrite(p->pRefracTime, sizeof(int), n, f);

	fwrite(p->pVm, sizeof(real), n, f);
	fwrite(p->pCi, sizeof(real), n, f);
	fwrite(p->pCe, sizeof(real), n, f);
	fwrite(p->pC_i, sizeof(real), n, f);
	fwrite(p->pC_e, sizeof(real), n, f);
	fwrite(p->pVtmp, sizeof(real), n, f);
	fwrite(p->pIi, sizeof(real), n, f);
	fwrite(p->pIe, sizeof(real), n, f);
	fwrite(p->pVthresh, sizeof(real), n, f);
	fwrite(p->pVreset, sizeof(real), n, f);
	fwrite(p->pCm, sizeof(real), n, f);

	return 0;
}

int loadLIF(int n, FILE *f)
{
	LIFData *p = (LIFData*)malloc(sizeof(LIFData));

	fread(p->pRefracStep, sizeof(int), n, f);
	fread(p->pRefracTime, sizeof(int), n, f);

	fread(p->pVm, sizeof(real), n, f);
	fread(p->pCi, sizeof(real), n, f);
	fread(p->pCe, sizeof(real), n, f);
	fread(p->pC_i, sizeof(real), n, f);
	fread(p->pC_e, sizeof(real), n, f);
	fread(p->pVtmp, sizeof(real), n, f);
	fread(p->pIi, sizeof(real), n, f);
	fread(p->pIe, sizeof(real), n, f);
	fread(p->pVthresh, sizeof(real), n, f);
	fread(p->pVreset, sizeof(real), n, f);
	fread(p->pCm, sizeof(real), n, f);

	return p;
}

