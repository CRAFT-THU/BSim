/* This header file is writen by qp09
 * usually just for fun
 * 一 三月 20 2017
 */
#ifndef GARRAYNEURONS_H
#define GARRAYNEURONS_H

#include "../../base/type.h"
#include "../../utils/macros.h"

struct GArrayNeurons {
	int *p_start;
	int *p_end;
	int *p_fire_time;
};

NEURON_GPU_FUNC_DEFINE(Array)

#endif /* GARRAYNEURONS_H */

