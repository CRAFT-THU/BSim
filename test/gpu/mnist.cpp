
#include <iostream>
#include <assert.h>

#include "../../src/third_party/cnpy/cnpy.h"
#include "../../src/third_party/mnist/mnist_reader.hpp"
#include "../../include/BSim.h"

using std::cout;
using std::endl;

size_t calcAllSize(cnpy::NpyArray &array)
{
	size_t all_size = 1;
	for (size_t i=0; i<array.shape.size(); i++) {
		all_size *= array.shape[i];
	}

	return all_size;
}

const int IMAGE_SIZE = 28;
const int KERNEL_SIZE = 5;
const int IMAGE_PIXELS = IMAGE_SIZE * IMAGE_SIZE;

int main(int argc, char **argv)
{
	auto dataset = mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>("/home/yhx/data/mnist", 1, 0);

	Network c;
	Population<Constant_spikes> *img = c.createPopulation(0, IMAGE_PIXELS, Constant_spikes(ConstantNeuron(0.5), 1.0, 1.0));
	assert(dataset.test_images[0].size() == IMAGE_PIXELS);
	for (int i = 0; i < img->getNum(); i++) {
		Constant_spikes * n = static_cast<Constant_spikes*>(img->getNeuron(i));
		n->setRate(dataset.test_images[0][i]/1000.0);
	}

	cnpy::NpyArray conv1_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/conv1_bias:0.data.npy");
	assert(calcAllSize(conv1_bias) == 32);
	real * conv1_bias_data = reinterpret_cast<real*>(conv1_bias.data);
	Population<TJ_curr_static> **conv1 = (Population<TJ_curr_static> **)malloc(32 * sizeof(Population<TJ_curr_static>*));
	for (int i=0; i<32; i++) {
		conv1[i] = c.createPopulation(i + 1, IMAGE_PIXELS, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0, conv1_bias_data[i]), 1.0, 1.0));
	}

	Population<Max_pooling> **pool1 = (Population<Max_pooling> **)malloc(32 * sizeof(Population<Max_pooling>*));
	for (int i=0; i<32; i++) {
		pool1[i] = c.createPopulation(33 + 1, IMAGE_PIXELS/4, Max_pooling(MaxNeuron(4), 0.0, 0.0));
	}

	cnpy::NpyArray conv2_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/conv2_bias:0.data.npy");
	assert(calcAllSize(conv2_bias) == 64);
	real * conv2_bias_data = reinterpret_cast<real*>(conv2_bias.data);
	Population<TJ_curr_static> **conv2 = (Population<TJ_curr_static> **)malloc(64 * sizeof(Population<TJ_curr_static>*));
	for (int i=0; i<64; i++) {
		conv2[i] = c.createPopulation(65 + i, IMAGE_PIXELS/4, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0, conv2_bias_data[i]), 1.0, 1.0));
	}

	Population<Max_pooling> **pool2 = (Population<Max_pooling> **)malloc(64 * sizeof(Population<Max_pooling>*));
	for (int i=0; i<64; i++) {
		pool2[i] = c.createPopulation(129 + i, IMAGE_PIXELS/16, Max_pooling(MaxNeuron(4), 0.0, 0.0));
	}

	Population<TJ_curr_static> *fc1 = c.createPopulation(193, 1024, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0, 0.0), 1.0, 1.0));
	cnpy::NpyArray fc1_bias = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/fc1_bias:0.data.npy");
	assert(calcAllSize(fc1_bias) == fc1->getNum());
	real * fc1_bias_data = reinterpret_cast<real*>(fc1_bias.data);
	for (int i = 0; i < fc1->getNum(); i++) {
		TJ_curr_static* n = static_cast<TJ_curr_static*>(fc1->getNeuron(i));
		n->setI(fc1_bias_data[i]);
	}

	real * conv_delay = getConstArray((real)1e-3, 25);
	real * fc1_delay = getConstArray((real)1e-3, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);

	cnpy::NpyArray kernel1 = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_conv1_k.npy");
	assert(calcAllSize(kernel1) == 32 * KERNEL_SIZE * KERNEL_SIZE);
	real * kernel1_data = reinterpret_cast<real*>(kernel1.data);

	for (int i=0; i<32; i++) {
		c.connectConv(img, conv1[i], kernel1_data + i*KERNEL_SIZE*KERNEL_SIZE, conv_delay, NULL, IMAGE_SIZE, IMAGE_SIZE, KERNEL_SIZE, KERNEL_SIZE);
	}

	for (int i=0; i<32; i++) {
		c.connectPooling(conv1[i], pool1[i], 1.0e-3, IMAGE_SIZE, IMAGE_SIZE, 2, 2);
	}

	cnpy::NpyArray kernel2 = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_conv2_k.npy");
	assert(calcAllSize(kernel2) == 32 * KERNEL_SIZE * KERNEL_SIZE * 64);
	real * kernel2_data = reinterpret_cast<real*>(kernel2.data);

	for (int i=0; i<64; i++) {
		for (int j=0; j<32; j++) {
			c.connectConv(pool1[j], conv2[i], kernel2_data + j*64*KERNEL_SIZE*KERNEL_SIZE + i*KERNEL_SIZE*KERNEL_SIZE, conv_delay, NULL, IMAGE_SIZE/2, IMAGE_SIZE/2, KERNEL_SIZE, KERNEL_SIZE);
		}
	}

	for (int i=0; i<64; i++) {
		c.connectPooling(conv2[i], pool2[i], 1.0e-3, IMAGE_SIZE/2, IMAGE_SIZE/2, 2, 2);
	}

	cnpy::NpyArray weight1 = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_fc1_w.npy");
	assert(calcAllSize(weight1) == IMAGE_SIZE/4*IMAGE_SIZE/4*64*1024);
	real * weight1_data = reinterpret_cast<real*>(weight1.data);
	for (int i=0; i<64; i++) {
		c.connect(pool2[i], fc1, weight1_data + i*IMAGE_SIZE/4*IMAGE_SIZE/4, fc1_delay, NULL, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);
	}


	//STSim st(&c, 1.0e-3);
	//st.run(1);

	SGSim sg(&c, 1.0e-3);
	sg.run(1);

	return 0;
} 