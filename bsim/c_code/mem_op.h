/* This header file is writen by qp09
 * usually just for fun
 * Thu February 16 2017
 */
#ifndef MEM_OP_H
#define MEM_OP_H

#include "helper_cuda.h"

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

#endif /* MEM_OP_H */

