
#include "../../include/BSim.h"

using namespace std;

int main(int argc, char **argv)
{

const double fthreshold=-54e-3;
const double freset=-60e-3;
const double c_m=0.2e-9; //*nF
const double v_rest=-74e-3;//*mV
const double tau_syn_e =5e-3;//*ms
const double tau_syn_i= 0e-3;//*ms

//# 论文没有这些参数
const double tau_m=10e-3;//*ms
const double i_offset =2e-9;//*nA
const double frefractory=0;
const double fv=-74e-3;

        const int N = 10;
        Network c;
        Population<LIF_brian> *pn0 = c.createPopulation(0, N, LIF_brian(LIFENeuron(
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,i_offset
	),  tau_syn_e, tau_syn_i));
	
        Population<LIF_brian> *pn1 = c.createPopulation(1, N, LIF_brian(LIFENeuron(
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,0
	), tau_syn_e, tau_syn_i));

        Population<LIF_brian> *pn2 = c.createPopulation(2, N, LIF_brian(LIFENeuron(
	fv,v_rest,freset,
	c_m,tau_m,
	frefractory,tau_syn_e,tau_syn_i,
	fthreshold,0
	), tau_syn_e, tau_syn_i));
	
        real * weight0 = NULL;
        real * weight1 = NULL;
        real * delay = NULL;

        printf("GENERATE DATA...\n");
        //real * array = getConstArray(weight_value, num);
        weight0 = getConstArray((real)4.0e-10, N*N);
        weight1 = getConstArray((real)4.0e-10, N*N);
        delay = getConstArray((real)1.0e-4, N*N);
        printf("GENERATE DATA FINISHED\n");

        c.connect(pn0, pn1, weight0, delay, NULL, N*N);
        c.connect(pn1, pn2, weight1, delay, NULL, N*N);
	

        SGSim sg(&c, (real)1.0e-4);		//这是gpu的代码
        sg.run((real)0.1);			//这是gpu的代码

        STSim st(&c, (real)1.0e-4);
        st.run((real)0.1);

        return 0;
}
