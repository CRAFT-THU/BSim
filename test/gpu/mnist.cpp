
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

int main(int argc, char **argv)
{
	cnpy::NpyArray arr = cnpy::npy_load("/home/yhx/train/mnist_TJ_cnn/fc2_weight:0.data.npy");
	cout << "Shape: ";
	size_t all_size = 1;
	for (size_t i=0; i<arr.shape.size(); i++) {
		cout << arr.shape[i] << " ";
		all_size *= arr.shape[i];
	}
	cout << endl;

	cout << "Data: ";
	real *loaded_data = reinterpret_cast<real*>(arr.data);
	cout << loaded_data[1024*5] << " ";
	cout << endl;

	auto dataset = mnist::read_dataset<std::vector, std::vector, uint8_t, uint8_t>("/home/yhx/data/mnist", 1, 0);
	//cout << dataset.test_images.size() << endl;
	//cout << dataset.test_images[0].size() << endl;
	//cout << (int)dataset.test_images[0][0] << endl;
	//cout << dataset.test_labels.size() << endl;
	//cout << (int)dataset.test_labels[0] << endl;

	return 0;

	const int IMAGE_SIZE = 28;
	const int IMAGE_PIXELS = IMAGE_SIZE * IMAGE_SIZE;

	Network c;
	Population<Constant_spikes> *img = c.createPopulation(0, IMAGE_PIXELS, Constant_spikes(ConstantNeuron(0.5), 1.0, 1.0));
	assert
	for (int i = 0; i < img->getNum(); i++) {
		Constant_spikes * n = static_cast<Constant_spikes*>(img->getNeuron(i));
		n->setRate(i/36.0 * 0.5 + 0.5);
	}

	Population<TJ_curr_static> **conv1 = (Population<TJ_curr_static> **)malloc(32 * sizeof(Population<TJ_curr_static>*));
	for (int i=0; i<32; i++) {
		conv1[i] = c.createPopulation(i + 1, IMAGE_PIXELS, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0, 0.0), 1.0, 1.0));
	}

	Population<Max_pooling> **pool1 = (Population<Max_pooling> **)malloc(32 * sizeof(Population<Max_pooling>*));
	for (int i=0; i<32; i++) {
		pool1[i] = c.createPopulation(33 + 1, IMAGE_PIXELS/4, Max_pooling(MaxNeuron(4), 0.0, 0.0));
	}

	Population<TJ_curr_static> **conv2 = (Population<TJ_curr_static> **)malloc(64 * sizeof(Population<TJ_curr_static>*));
	for (int i=0; i<64; i++) {
		conv2[i] = c.createPopulation(65 + i, IMAGE_PIXELS/4, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0, 0.0), 1.0, 1.0));
	}

	Population<Max_pooling> **pool2 = (Population<Max_pooling> **)malloc(64 * sizeof(Population<Max_pooling>*));
	for (int i=0; i<64; i++) {
		pool2[i] = c.createPopulation(129 + i, IMAGE_PIXELS/16, Max_pooling(MaxNeuron(4), 0.0, 0.0));
	}

	Population<TJ_curr_static> *fc1 = c.createPopulation(193, 1024, TJ_curr_static(TJNeuron(0.0, 0.0, 0.5, 4e-3, 1.0, 0.0), 1.0, 1.0));

	real * weight0 = NULL;
	real * weight1 = NULL;

	//real * array = getConstArray(weight_value, num);
	weight0 = getConstArray((real)20e-4, 9);
	weight1 = getConstArray((real)10e-4, 81);
	//weight2 = getRandomArray((real)20e-3, N*N);
	real * conv_delay = getConstArray((real)1e-3, 25);
	real * fc1_delay = getConstArray((real)1e-3, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);

	for (int i=0; i<32; i++) {
		c.connectConv(img, conv1[i], weight0, conv_delay, NULL, IMAGE_SIZE, IMAGE_SIZE, 5, 5);
	}

	for (int i=0; i<32; i++) {
		c.connectPooling(conv1[i], pool1[i], 1.0e-3, IMAGE_SIZE, IMAGE_SIZE, 2, 2);
	}


	for (int i=0; i<64; i++) {
		for (int j=0; j<32; j++) {
			c.connectConv(pool1[j], conv2[i], weight0, conv_delay, NULL, IMAGE_SIZE/2, IMAGE_SIZE/2, 5, 5);
		}
	}

	for (int i=0; i<64; i++) {
		c.connectPooling(conv2[i], pool2[i], 1.0e-3, IMAGE_SIZE/2, IMAGE_SIZE/2, 2, 2);
	}

	for (int i=0; i<64; i++) {
		c.connect(pool2[i], fc1, weight1, fc1_delay, NULL, IMAGE_SIZE/4 * IMAGE_SIZE/4 * 1024);
	}

	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)
	c.connectConv(pn0, pn1, weight0, delay, NULL, 6, 6, 3, 3);
	c.connectPooling(pn1, pn2, 1.0e-3, 6, 6, 2, 2);
	c.connect(pn2, pn3, weight1, delay, NULL, 81);
	//c.connect(pn0, pn1, weight0, delay, NULL, 6);

	STSim st(&c, 1.0e-3);
	st.run(1);

	SGSim sg(&c, 1.0e-3);
	sg.run(1);

	return 0;
} 
