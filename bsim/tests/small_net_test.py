import unittest
from ctypes import *

import env
from bsim.cudamemop import cudamemops


class TestMemOpMethods(unittest.TestCase):
    def test_malloc_and_data(self):
        lif_curr_exp = NeuronModel(
            computation='v = Cm * v + v_tmp + i_exec * C_exec + i_inh * C_inh;i_exec *= Cexec; '
                        'i_inh *= Cinh; i_exec += g_i_exec; i_inh += g_i_inh',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='LIF_curr_exp'
        )

        stdp_synapse = SynapseModel(
            computation='apre*=exp(last_update-t)/tau_pre; apost*=exp(last_update-t)/tau_post',
            name='static_synapse'
        )
        data = (c_int * 1000)(*list(range(1000)))
        cpu = (c_int * 1000)(*([0]*1000))
        gpu = cudamemops.gpu_malloc_int(1000)
        gpu1 = cudamemops.to_gpu_int(data, 1000)
        cpu1 = cast(cudamemops.from_gpu_int(gpu1, 1000), POINTER(c_int*1000))
        cudamemops.cpu2gpu_int(data, gpu, 1000)
        cudamemops.gpu2cpu_int(gpu, cpu, 1000)

        self.assertListEqual(list(data), list(cpu))
        self.assertListEqual(list(data), list(cpu1.contents))


if __name__ == '__main__':
    unittest.main()

