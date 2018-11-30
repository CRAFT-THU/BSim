

#ifndef CUDAMEMCPY_H
#define CUDAMEMCPY_H


extern "C" {
	int* gpu_malloc_int(int size);
	void cpu2gpu_int(int *cpu, int *gpu, int size);
	void gpu2cpu_int(int *gpu, int *cpu, int size);
	int* to_gpu_int(int *cpu, int size);
	int* from_gpu_int(int *gpu, int size);

	float* gpu_malloc_float(int size);
	void cpu2gpu_float(float *cpu, float *gpu, int size);
	void gpu2cpu_float(float *gpu, float *cpu, int size);
	float* to_gpu_float(float *cpu, int size);
	float* from_gpu_float(float *gpu, int size);

	double* gpu_malloc_double(int size);
	void cpu2gpu_double(double *cpu, double *gpu, int size);
	void gpu2cpu_double(double *gpu, double *cpu, int size);
	double* to_gpu_double(double *cpu, int size);
	double* from_gpu_double(double *gpu, int size);

}

#endif /* CUDAMEMCPY_H */
