
#ifndef FFT_H
#define FFT_H

#include "../../utils/BlockSize.h"

#include "GFFTNeurons.h"


__global__ void update_fft_neuron(GFFTNeurons *d_neurons, int num, int start_id);

#endif  // FFT_H
