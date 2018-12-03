

#include "helper_cuda.h"
#include "cudamemop.h"


int* gpu_malloc_int(int size)
{
	int * gpu = NULL;
	checkCudaErrors(cudaMalloc((void**)&(gpu), sizeof(int)*size));
	return gpu;
}

void cpu2gpu_int(int *cpu, int *gpu, int size)
{
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(int)*size, cudaMemcpyHostToDevice));
}

void gpu2cpu_int(int *gpu, int *cpu, int size)
{
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(int)*size, cudaMemcpyDeviceToHost));
}

int* to_gpu_int(int *cpu, int size)
{
	int * gpu = NULL;
	checkCudaErrors(cudaMalloc((void**)&(gpu), sizeof(int)*size));
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(int)*size, cudaMemcpyHostToDevice));
	return gpu;
}

int* from_gpu_int(int *gpu, int size)
{
	int * cpu = static_cast<int*>(malloc(sizeof(int)*size));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(int)*size, cudaMemcpyDeviceToHost));
	return cpu;
}

float* gpu_malloc_float(int size)
{
	float * gpu = NULL;
	checkCudaErrors(cudaMalloc((void**)&(gpu), sizeof(float)*size));
	return gpu;
}

void cpu2gpu_float(float *cpu, float *gpu, int size)
{
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(float)*size, cudaMemcpyHostToDevice));
}

void gpu2cpu_float(float *gpu, float *cpu, int size)
{
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(float)*size, cudaMemcpyDeviceToHost));
}

float* to_gpu_float(float *cpu, int size)
{
	float * gpu = NULL;
	checkCudaErrors(cudaMalloc((void**)&(gpu), sizeof(float)*size));
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(float)*size, cudaMemcpyHostToDevice));
	return gpu;
}

float* from_gpu_float(float *gpu, int size)
{
	float * cpu = static_cast<float*>(malloc(sizeof(float)*size));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(float)*size, cudaMemcpyDeviceToHost));
	return cpu;
}

double* gpu_malloc_double(int size)
{
	double * gpu = NULL;
	checkCudaErrors(cudaMalloc((void**)&(gpu), sizeof(double)*size));
	return gpu;
}

void cpu2gpu_double(double *cpu, double *gpu, int size)
{
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(double)*size, cudaMemcpyHostToDevice));
}

void gpu2cpu_double(double *gpu, double *cpu, int size)
{
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(double)*size, cudaMemcpyDeviceToHost));
}

double* to_gpu_double(double *cpu, int size)
{
	double * gpu = NULL;
	checkCudaErrors(cudaMalloc((void**)&(gpu), sizeof(double)*size));
	checkCudaErrors(cudaMemcpy(gpu, cpu, sizeof(double)*size, cudaMemcpyHostToDevice));
	return gpu;
}

double* from_gpu_double(double *gpu, int size)
{
	double * cpu = static_cast<double*>(malloc(sizeof(double)*size));
	checkCudaErrors(cudaMemcpy(cpu, gpu, sizeof(double)*size, cudaMemcpyDeviceToHost));
	return cpu;
}


