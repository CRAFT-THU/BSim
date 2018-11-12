/* This header file is writen by qp09
 * usually just for fun
 * 三 三月 22 2017
 */
#ifndef GMAXNEURONS_H
#define GMAXNEURONS_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GMaxNeurons {
	int max_N;
	int *p_N;
	int *p_count;
	int *p_record;
};

NEURON_GPU_FUNC_DEFINE(Max)



#endif /* GMAXNEURONS_H */

