#include "../../include/BSim.h"
#include <stdlib.h>
#include<time.h>

using namespace std;

int main(int argc, char **argv)
{
	time_t start,end;
	start=clock(); //time(NULL);
	if(argc !=8)
	{
		printf("Need 7 paras. For example\n FR 1%%: %s depth num_neuron 0.7 0.5 0.6 0.3 6\n FR 5%%: %s depth num_neuron 0.7 0.9 0.6 0.2 6\n FR 20%%: %s depth num_neuron 1.3 1 2 1 50\n FR 50%%: %s depth num_neuron 2.7 3 2 1 20\n FR 65%%: %s depth num_neuron 4 2 5 3 20\n", argv[0], argv[0], argv[0], argv[0], argv[0]);
		return 0;
	}
	const int depth=atoi(argv[1]);
	const int N=atoi(argv[2]);
	
	const real w1=atof(argv[3]);
	const real w2=atof(argv[4]);
	const real w3=atof(argv[5]);
	const real w4=atof(argv[6]);
	const int who=atoi(argv[7]);
	
	printf("depth=%d N=%d\n",depth,N);
const real fthreshold=-54e-3;
const real freset=-60e-3;
const real c_m=0.2e-9; //*nF
const real v_rest=-74e-3;//*mV
const real tau_syn_e =2e-3;//*ms
const real tau_syn_i= 0.6e-3;//*ms

//# 论文没有这些参数
const real tau_m=10e-3;//*ms
const real i_offset =2e-9;//*nA
const real frefractory=0;
const real fv=-74e-3;

	const real run_time=100e-3;
	const real dt=1e-4;
	Network c;

	Population *g[depth+1];
	
	g[1]=c.createPopulation(1, N, LIF_curr_exp(LIFNeuron(
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,i_offset
	),  tau_syn_e, tau_syn_i));
	
	for(int i=2;i<=depth;i++)
		g[i] = c.createPopulation(i, N, LIF_curr_exp(LIFNeuron(
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,0
	),  tau_syn_e, tau_syn_i));
	
	real * weight6 = getConstArray((real)(1e-9)*w1 /N, N*N);
	real * weight6_30 = getConstArray((real)(1e-9)*w2 /N, N*N);
	real * weight5 = getConstArray((real)(1e-9)*w3 /N, N*N);
	real * weight3 = getConstArray((real)(-1e-9)*w4 /N, N*N);
	real * delay = getConstArray((real)0.1e-3, N*N);
	
	enum SpikeType type=Inhibitory;
	SpikeType *ii = getConstArray(type, N*N);
	
	for(int i=2;i<=depth;i++)
	{
		
		c.connect(g[i-1], g[i], weight6, delay, NULL, N*N);
		if (i % who ==0)
		c.connect(g[1], g[i], weight6_30, delay, NULL, N*N);	
	}

	Population *p[depth+1];
	int i=1;
	while(i+1<=depth)
	{
		p[i] = c.createPopulation(i+depth, N, LIF_curr_exp(LIFNeuron(
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,0
	),  tau_syn_e, tau_syn_i));
		c.connect(g[i], p[i], weight5, delay, NULL, N*N);
		c.connect(p[i], g[i+1], weight3, delay, ii, N*N);
		i+=4;

		
	}
	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)

	#if 0
		STSim st(&c, dt);	// cpu
		st.run(run_time);
	#else
		SGSim sg(&c, dt);	//gpu
		sg.run(run_time);	
	#endif
	
	end=clock(); //time(NULL);
	printf("exec time=%lf seconds\n",(double)(end-start) / CLOCKS_PER_SEC);
		return 0;
	}
