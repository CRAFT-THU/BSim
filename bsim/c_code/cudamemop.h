

#ifndef CUDAMEMCPY_H
#define CUDAMEMCPY_H


extern "C" {
	int* gpu_malloc_int(int size);
	void cpu2gpu_int(void *cpu, void *gpu, int size);
	void gpu2cpu_int(void *gpu, void *cpu, int size);

}

#endif /* CUDAMEMCPY_H */
