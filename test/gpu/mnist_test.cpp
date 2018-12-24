
#include <iostream>
#include <assert.h>

extern "C" {
#include <cblas.h>
}

#include "../../src/third_party/cnpy/cnpy.h"
#include "../../src/third_party/mnist/mnist_reader.hpp"
#include "../../include/BSim.h"

using std::cout;
using std::endl;

int getMaxLoc(real *array, int size);
size_t calcAllSize(cnpy::NpyArray &array);
//int logFireInfo(FireInfo log, string type, int start, int batch, int size, string name);

const int IMAGE_SIZE = 28;
const int KERNEL_SIZE = 5;
const int IMAGE_PIXELS = IMAGE_SIZE * IMAGE_SIZE;

int main(int argc, char **argv)
{
	auto dataset = mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>("/home/yhx/data/mnist", 1, 0);

	cnpy::NpyArray conv1_kernel = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_conv1_k.npy");
	assert(calcAllSize(conv1_kernel) == 32 * KERNEL_SIZE * KERNEL_SIZE);
	real * conv1_kernel_data = reinterpret_cast<real*>(conv1_kernel.data);

	cnpy::NpyArray conv1_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/conv1_bias:0.data.npy");
	assert(calcAllSize(conv1_bias) == 32);
	real * conv1_bias_data = reinterpret_cast<real*>(conv1_bias.data);

	cnpy::NpyArray conv2_kernel = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_conv2_k.npy");
	assert(calcAllSize(conv2_kernel) == 32 * KERNEL_SIZE * KERNEL_SIZE * 64);
	real * conv2_kernel_data = reinterpret_cast<real*>(conv2_kernel.data);

	cnpy::NpyArray conv2_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/conv2_bias:0.data.npy");
	assert(calcAllSize(conv2_bias) == 64);
	real * conv2_bias_data = reinterpret_cast<real*>(conv2_bias.data);

	cnpy::NpyArray fc1_weight = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_fc1_w.npy");
	assert(calcAllSize(fc1_weight) == IMAGE_SIZE/4*IMAGE_SIZE/4*64*1024);
	real * fc1_weight_data = reinterpret_cast<real*>(fc1_weight.data);

	cnpy::NpyArray fc1_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/fc1_bias:0.data.npy");
	real * fc1_bias_data = reinterpret_cast<real*>(fc1_bias.data);

	cnpy::NpyArray fc2_weight = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/fc2_weight:0.data.npy");
	assert(calcAllSize(fc2_weight) == 1024*10);
	real * fc2_weight_data = reinterpret_cast<real*>(fc2_weight.data);

	cnpy::NpyArray fc2_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/fc2_bias:0.data.npy");
	assert(calcAllSize(fc2_bias) == 10);
	real * fc2_bias_data = reinterpret_cast<real*>(fc2_bias.data);

	real * conv_delay = getConstArray((real)1e-3, 25);
	real * fc1_delay = getConstArray((real)1e-3, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);

	Network c;
	Population<Constant_spikes> *img = c.createPopulation(0, IMAGE_PIXELS, Constant_spikes(ConstantNeuron(0.5), 1.0, 1.0));
	assert(dataset.test_images[0].size() == IMAGE_PIXELS);

	Population<TJ_curr_static> **conv1 = (Population<TJ_curr_static> **)malloc(32 * sizeof(Population<TJ_curr_static>*));
	for (int i=0; i<32; i++) {
		conv1[i] = c.createPopulation(i + 1, IMAGE_PIXELS, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0e-3, conv1_bias_data[i]/1000.0), 1.0, 1.0));
	}

	Population<Max_pooling> **pool1 = (Population<Max_pooling> **)malloc(32 * sizeof(Population<Max_pooling>*));
	for (int i=0; i<32; i++) {
		pool1[i] = c.createPopulation(33 + 1, IMAGE_PIXELS/4, Max_pooling(MaxNeuron(4), 0.0, 0.0));
	}

	Population<TJ_curr_static> **conv2 = (Population<TJ_curr_static> **)malloc(64 * sizeof(Population<TJ_curr_static>*));
	for (int i=0; i<64; i++) {
		conv2[i] = c.createPopulation(65 + i, IMAGE_PIXELS/4, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0e-3, conv2_bias_data[i]/1000.0), 1.0, 1.0));
	}

	Population<Max_pooling> **pool2 = (Population<Max_pooling> **)malloc(64 * sizeof(Population<Max_pooling>*));
	for (int i=0; i<64; i++) {
		pool2[i] = c.createPopulation(129 + i, IMAGE_PIXELS/16, Max_pooling(MaxNeuron(4), 0.0, 0.0));
	}

	Population<TJ_curr_static> *fc1 = c.createPopulation(193, 1024, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0e-3, 0.0), 1.0, 1.0));
	assert(calcAllSize(fc1_bias) == fc1->getNum());
	for (int i = 0; i < fc1->getNum(); i++) {
		TJ_curr_static* n = static_cast<TJ_curr_static*>(fc1->getNeuron(i));
		n->setI(fc1_bias_data[i]/1000.0);
	}

	for (int i=0; i<32; i++) {
		c.connectConv(img, conv1[i], conv1_kernel_data + i*KERNEL_SIZE*KERNEL_SIZE, conv_delay, NULL, IMAGE_SIZE, IMAGE_SIZE, KERNEL_SIZE, KERNEL_SIZE);
	}

	for (int i=0; i<32; i++) {
		c.connectPooling(conv1[i], pool1[i], 1.0e-3, IMAGE_SIZE, IMAGE_SIZE, 2, 2);
	}

	for (int i=0; i<64; i++) {
		for (int j=0; j<32; j++) {
			c.connectConv(pool1[j], conv2[i], conv2_kernel_data + j*64*KERNEL_SIZE*KERNEL_SIZE + i*KERNEL_SIZE*KERNEL_SIZE, conv_delay, NULL, IMAGE_SIZE/2, IMAGE_SIZE/2, KERNEL_SIZE, KERNEL_SIZE);
		}
	}

	for (int i=0; i<64; i++) {
		c.connectPooling(conv2[i], pool2[i], 1.0e-3, IMAGE_SIZE/2, IMAGE_SIZE/2, 2, 2);
	}

	for (int i=0; i<64; i++) {
		c.connect(pool2[i], fc1, fc1_weight_data + i*IMAGE_SIZE/4*IMAGE_SIZE/4, fc1_delay, NULL, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);
	}

	real *real_rate = (real*)malloc(sizeof(real)*1024);
	real *res = (real*)malloc(sizeof(real)* 10);
	//STSim st(&c, 1.0e-3);
	//st.run(1);
	ArrayInfo zero_array = {0, NULL};
	FireInfo fire_info;
	fire_info["count"] = zero_array;
	fire_info["X"] = zero_array;
	fire_info["Y"] = zero_array;

	SGSim sg(&c, 1.0e-3);
	//STSim st(&c, 1.0e-3);
	
	int matched_batches = 0;
	for (int batch = 0; batch < 1; batch++) {
		for (int i = 0; i < img->getNum(); i++) {
			Constant_spikes * n = static_cast<Constant_spikes*>(img->getNeuron(i));
			n->setRate(dataset.test_images[batch][i]/1000.0);
		}

		//st.run(1, fire_info);
		sg.run(1, fire_info);

		int *rate = (int*) fire_info["count"].data;
		int *X = (int*) fire_info["X"].data;
		real *Y = (real*) fire_info["Y"].data;

		logFireInfo(fire_info, "count", 0, 1, IMAGE_PIXELS, "img");
		logFireInfo(fire_info, "count", IMAGE_PIXELS, 32, IMAGE_PIXELS, "conv1");
		logFireInfo(fire_info, "X", IMAGE_PIXELS, 32, IMAGE_PIXELS, "conv1");
		logFireInfo(fire_info, "Y", IMAGE_PIXELS, 32, IMAGE_PIXELS, "conv1");

		//FILE *img_file = fopen("img.data", "w+");
		//if (img_file == NULL) {
		//	printf("ERROR: Open file img.data failed\n");
		//	return -1;
		//}

		//for (int i=0; i<IMAGE_PIXELS; i++) {
		//	fprintf(img_file, "%d ", rate[i]);
		//}

		//fflush(img_file);
		//fclose(img_file);

		
		//FILE *conv1_file = fopen("conv1.data", "w+");
		//if (conv1_file == NULL) {
		//	printf("ERROR: Open file conv1.data failed\n");
		//	return -1;
		//}

		//
		//for (int i=0; i<32; i++) {
		//	for (int j=0; j<IMAGE_PIXELS; j++) {
		//		fprintf(conv1_file, "%d ", rate[(i+1)*IMAGE_PIXELS + j]);
		//	}
		//	fprintf(conv1_file, "\n");
		//}

		//fflush(conv1_file);
		//fclose(conv1_file);


		for (int i=0; i<1024; i++) {
			real_rate[i] = rate[fire_info["count"].size - 1024 + i];
		}

		for (int i=0; i<10; i++) {
			res[i] = fc2_bias_data[i];
		}

		//cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, 1024, 1, 10, 1, fc2_weight_data, 10, real_rate, 1, 0, res, 1);
#ifdef USE_DOUBLE
		cblas_dgemv(CblasRowMajor, CblasTrans, 1024, 10, 1, fc2_weight_data, 10, real_rate, 1, 1, res, 1);
#else
		cblas_sgemv(CblasRowMajor, CblasTrans, 1024, 10, 1, fc2_weight_data, 10, real_rate, 1, 1, res, 1);
#endif

		for (int i=0; i< 10; i++) {
			printf("%f ", res[i]);
		}
		printf("\n");
		printf("%d", getMaxLoc(res, 10)); 
		printf("\n");
		printf("%d", dataset.test_labels[batch]);
		printf("\n");
		
	}

	return 0;
} 


int getMaxLoc(real *array, int size)
{
	real max = array[0];
	int idx = 0;
	for (int i=1; i<size; i++) {
		if (max < array[i]) {
			max = array[i];
			idx = i;
		}
	}

	return idx;
}

size_t calcAllSize(cnpy::NpyArray &array)
{
	size_t all_size = 1;
	for (size_t i=0; i<array.shape.size(); i++) {
		all_size *= array.shape[i];
	}

	return all_size;
}
