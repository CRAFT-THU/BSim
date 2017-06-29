#include "../../include/BSim.h"
#include <stdlib.h>
#include<time.h>

using namespace std;

int main(int argc, char **argv)
{
	time_t start,end;
	start=time(NULL);
	if(argc !=3)
	{
		printf("need 2 para. for example ./a.out depth num_neuron\n");
		return 0;
	}
	const int depth=atoi(argv[1]);
	const int N=atoi(argv[2]);
	printf("depth=%d N=%d\n",depth,N);
const double fthreshold=-54e-3;
const double freset=-60e-3;
const double c_m=0.2e-9; //*nF
const double v_rest=-74e-3;//*mV
const double tau_syn_e =2e-3;//*ms
const double tau_syn_i= 0.6e-3;//*ms

//# 论文没有这些参数
const double tau_m=10e-3;//*ms
const double i_offset =2e-9;//*nA
const double frefractory=0;
const double fv=-74e-3;
//const double fie=0;
//const double fii=0;

	const double run_time=100e-3;
	const double dt=1e-4;
	Network c;
	/*createPopulation(int id, int N, LIFENeuron(ID id, 
real v_init, real v_rest, real v_reset, 
real cm, real tau_m, 
real tau_refrac, real tau_syn_E, real tau_syn_I, 
real v_thresh, real i_offset
)), ID(0, 0), real tau_syn_E, real tau_syn_I);
	*/
	/*
	Population<LIF_brian> *pn0 = c.createPopulation(0, N, LIF_brian(LIFENeuron(ID(0,0), 0.0, 0.0, 0.0, 1.0e-1, 50.0e-3, 0.0, 1.0, 1.0, 15.0e-3, 10.0e-1), ID(0, 0), 1.0, 1.0));
	*/
	Population<LIF_brian> *g[depth+1];
	
	g[1]=c.createPopulation(1, N, LIF_brian(LIFENeuron( 
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,i_offset
	), tau_syn_e, tau_syn_i));
	
	for(int i=2;i<=depth;i++)
		g[i] = c.createPopulation(i, N, LIF_brian(LIFENeuron( 
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,0
	), tau_syn_e, tau_syn_i));
	
	real * weight6 = getConstArray((real)(6e-9/N), N*N);
	real * weight6_30 = getConstArray((real)(180e-9/N), N*N);
	real * weight5 = getConstArray((real)(5e-9/N), N*N);
	real * weight3 = getConstArray((real)(-3e-9/N), N*N);
	real * delay = getConstArray((real)0.1e-3, N*N);
	enum SpikeType type=Inhibitory;
	SpikeType *ii = getConstArray(type, N*N);
	
	for(int i=2;i<=depth;i++)
	{
		if (i<50)
		c.connect(g[i-1], g[i], weight6, delay, NULL, N*N);
		else
		c.connect(g[i-1], g[i], weight6_30, delay, NULL, N*N);	
	}

	Population<LIF_brian> *p[depth+1];
	int i=1;
	while(i+1<=depth)
	{
		p[i] = c.createPopulation(i+depth, N, LIF_brian(LIFENeuron( 
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,0
	), tau_syn_e, tau_syn_i));
		c.connect(g[i], p[i], weight5, delay, NULL, N*N);
		c.connect(p[i], g[i+1], weight3, delay, ii, N*N);
		i+=4;

		
	}
	//Network.connect(population1, population2, weight_array, delay_array, Exec or Inhi array, num)

	#if 0	
		STSim st(&c, dt);
		st.run(run_time);
	#else
		SGSim sg(&c, dt);	
		sg.run(run_time);	
		//MGSim mg(&c, dt);	
		//mg.run(run_time);
	#endif
	
	end=time(NULL);
	printf("exec time=%ld\n",end-start);
		return 0;
	}
