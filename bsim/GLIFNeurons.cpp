#include <stdlib.h>

#include "GLIFNeurons.h"

void *mallocLIF()
{
	GLIFNeurons * p = (GLIFNeurons*)malloc(sizeof(GLIFNeurons)*1);
	memset(p, 0, sizeof(GLIFNeurons)*1);
	return (void*)p;
}

void *allocLIF(int num)
{
	void *p = mallocLIF();
	allocLIFPara(p, num);
	return p;
}

int freeLIF(void *pCPU, int num)
{
	GLIFNeurons *p = (GLIFNeurons*)pCPU;

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

int allocLIFPara(void *pCPU, int num)
{
	GLIFNeurons *p = (GLIFNeurons*)pCPU;

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

int freeLIFPara(void *pCPU, int num)
{
	GLIFNeurons *p = (GLIFNeurons*)pCPU;

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

int saveLIF(void *pCPU, int num, FILE *f)
{

	GLIFNeurons *p = (GLIFNeurons*)pCPU;
	fwrite(p->pRefracStep, sizeof(int), num, f);
	fwrite(p->pRefracTime, sizeof(int), num, f);

	fwrite(p->pVm, sizeof(real), num, f);
	fwrite(p->pCi, sizeof(real), num, f);
	fwrite(p->pCe, sizeof(real), num, f);
	fwrite(p->pC_i, sizeof(real), num, f);
	fwrite(p->pC_e, sizeof(real), num, f);
	fwrite(p->pVtmp, sizeof(real), num, f);
	fwrite(p->pIi, sizeof(real), num, f);
	fwrite(p->pIe, sizeof(real), num, f);
	fwrite(p->pVthresh, sizeof(real), num, f);
	fwrite(p->pVreset, sizeof(real), num, f);
	fwrite(p->pCm, sizeof(real), num, f);

	return 0;
}

int loadLIF(int num, FILE *f)
{
	GLIFNeurons *p = (GLIFNeurons*)malloc(sizeof(GLIFNeurons));

	fread(p->pRefracStep, sizeof(int), num, f);
	fread(p->pRefracTime, sizeof(int), num, f);

	fread(p->pVm, sizeof(real), num, f);
	fread(p->pCi, sizeof(real), num, f);
	fread(p->pCe, sizeof(real), num, f);
	fread(p->pC_i, sizeof(real), num, f);
	fread(p->pC_e, sizeof(real), num, f);
	fread(p->pVtmp, sizeof(real), num, f);
	fread(p->pIi, sizeof(real), num, f);
	fread(p->pIe, sizeof(real), num, f);
	fread(p->pVthresh, sizeof(real), num, f);
	fread(p->pVreset, sizeof(real), num, f);
	fread(p->pCm, sizeof(real), num, f);

	return p;
}

