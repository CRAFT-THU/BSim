

#include "mem_op.h"
#include "cudamemop.h"


extern "C" {
	int* gpu_malloc_int(int size)
	{
		return gpuMalloc<int>(size);
	}

	void cpu2gpu_int(int *cpu, int *gpu, int size);
	{
		copyToGPU<int>(gpu, cpu, size);
	}

	void gpu2cpu_int(int *gpu, int *cpu, int size);
	{
		copyFromGPU<int>(cpu, gpu, size);
	}

}

