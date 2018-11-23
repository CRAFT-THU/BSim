
from bsim.neuron_model import NeuronModel
from bsim.synapse_model import SynapseModel


LIF_curr_exp = NeuronModel(
    computation='v = Cm * v + v_tmp + i_exc * C_exc + i_inh * C_inh;i_exec *= Cexc; i_inh *= Cinh',
    threshold='V > v_threshold',
    reset='v = v_reset',
    name='LIF_curr_exp')

STDPSynapse = SynapseModel(
    computation='apre*=exp(last_update-t)/tau_pre; apost*=exp(last_update-t)/tau_post',
    pre='apre += d_apre; weight=clip(weight+apost, gMin, gMax); last_update=t',
    post='apost+=d_apost; weight=clip(weight+apre, gMin, gMax); last_update=t',
    name='STDPSynapse'
)
