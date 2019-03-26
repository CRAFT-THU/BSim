/* This program is writen by qp09.
 * usually just for fun.
 * Sat October 24 2015
 */

#include <sys/time.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>
#include <iostream>

#include "../utils/utils.h"
#include "../utils/TypeFunc.h"
#include "../gpu_utils/mem_op.h"
#include "../gpu_utils/gpu_func.h"
#include "../gpu_utils/gpu_utils.h"
#include "../gpu_utils/runtime.h"
#include "../gpu_utils/GBuffers.h"
#include "../net/MultiNetwork.h"
#include "MultiNodeSimulator.h"

using std::cout;
using std::endl;

pthread_barrier_t nodeCycleBarrier;

CrossNodeDataGPU * gCrossDataNode;

MultiNodeSimulator::MultiNodeSimulator(Network *network, real dt) : SimulatorBase(network, dt)
{
}

MultiNodeSimulator::~MultiNodeSimulator()
{
}

void *run_thread_node(void *para);

int MultiNodeSimulator::run(real time, FireInfo &log)
{
	return 0;
}

