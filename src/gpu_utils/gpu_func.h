/* This header file is writen by qp09
 * usually just for fun
 * Wed March 09 2016
 */
#ifndef GPU_FUNC_H
#define GPU_FUNC_H

#include "../third_party/cuda/helper_cuda.h"
#include "../base/constant.h"
#include "BlockSize.h"

extern int (*cudaAllocType[TypeSize])(void *, void *, int);
extern int (*cudaFreeType[TypeSize])(void *);
extern int (*updateType[TypeSize])(void *, int, int, BlockSize*);
extern int (*copyType[TypeSize])(void *, void *, int, int);

extern BlockSize * getBlockSize(int nSize, int sSize);

template<typename T>
T* hostMalloc(int size)
{
	T * ret;
	checkCudaErrors(cudaMallocHost((void**)&(ret), sizeof(T) * size));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(T)*(size)));
	return ret;
}

template<typename T>
T* gpuMalloc(int size)
{ 
	T * ret;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(T) * size));
	checkCudaErrors(cudaMemset(ret, 0, sizeof(T)*(size)));
	return ret;
}

template<typename T>
void hostFree(T* cpu)
{
	checkCudaErrors(cudaFreeHost(cpu));
}

template<typename T>
void gpuFree(T* gpu)
{
	checkCudaErrors(cudaFree(gpu));
}

template<typename T>
T* copyToGPU(T* cpu, int size)
{
	T * ret;
	checkCudaErrors(cudaMalloc((void**)&(ret), sizeof(T) * size));
	checkCudaErrors(cudaMemcpy(ret, cpu, sizeof(T)*size, cudaMemcpyHostToDevice));

	return ret;
}

template<typename T>
void copyToGPU(T* gpu, T* cpu, int size)
{
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(T)*size, cudaMemcpyHostToDevice));
}

template<typename T>
T* copyFromGPU(T* gpu, int size)
{
	T * ret = static_cast<T*>(malloc(sizeof(T)*size));
	checkCudaErrors(cudaMemcpy(ret, gpu, sizeof(T)*size, cudaMemcpyDeviceToHost));

	return ret;
}

template<typename T>
void copyFromGPU(T* cpu, T* gpu, int size)
{
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(T)*size, cudaMemcpyDeviceToHost));
}

#endif /* GFUNC_H */
