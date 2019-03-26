

#include "../utils/constant.h"
#include "../net/Connection.h"

// Constant
extern __constant__ int MAX_DELAY;
extern __constant__ int gTimeTableCap;
extern __constant__ int gFiredTableCap;
extern __constant__ int gSynapsesTableCap;
extern __constant__ real DT;

// Variable
// extern __device__ int gCurrentIdx;
// extern __device__ int gCurrentCycle;
// extern __device__ int gFiredTableSize;
// extern __device__ int gSynapsesActiveTableSize;

#define gMin   0
#define gMax 0.01

// Arrays
//extern __device__ int *gTimeTable;

// Neuron Arrays
// extern __device__ real *gNeuronInput;
// extern __device__ real *gNeuronInput_I;

// Neuron Tables
// extern __device__ int *gFiredTable;
// extern __device__ int *gFiredTableSizes;
extern __device__ int *gActiveTable;
extern __device__ int gActiveTableSize;

// Synapse Tables
//extern __device__ int *gSynapsesActiveTable;
//extern __device__ int *gSynapsesLogTable;

// Log Arrays
extern __device__ int *gLayerInput;
extern __device__ real *gXInput;
extern __device__ int *gFireCount;

// Connection
extern __device__ N2SConnection *gConnection;
