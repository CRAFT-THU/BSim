
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
	for (int i=0; i<1; i++) {
		conv1[i] = c.createPopulation(i + 1, IMAGE_PIXELS, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 0.001, conv1_bias_data[i]/1000.0), 1.0, 1.0));
	}

	real * conv_delay = getConstArray((real)1e-3, 25);
	real * fc1_delay = getConstArray((real)1e-3, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);

	cnpy::NpyArray kernel1 = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/t_conv1_k.npy");
	assert(calcAllSize(kernel1) == 32 * KERNEL_SIZE * KERNEL_SIZE);
	real * kernel1_data = reinterpret_cast<real*>(kernel1.data);

	for (int i=0; i<1; i++) {
		c.connectConv(img, conv1[i], kernel1_data + i*KERNEL_SIZE*KERNEL_SIZE, conv_delay, NULL, IMAGE_SIZE, IMAGE_SIZE, KERNEL_SIZE, KERNEL_SIZE);
	}


	//STSim st(&c, 1.0e-3);
	//st.run(1);

	SGSim sg(&c, 1.0e-3);
	sg.run(1);

	return 0;
} 
