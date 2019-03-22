/* This header file is writen by qp09
 * usually just for fun
 * Fri December 23 2016
 */
#ifndef GFFTNEURONS_H
#define GFFTNEURONS_H

#include <cufft.h>

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GFFTNeurons {
	cufftComplex *p_input;
	cufftComplex *p_res;
	int *p_fire_count;
};

NEURON_GPU_FUNC_DEFINE(FFT)

#endif /* GFFTNEURONS_H */

