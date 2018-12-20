import unittest
import numpy as np
from ctypes import *

from bsim.population import *


class TestPopulationMethods(unittest.TestCase):

    def test_model(self):
        lif = NeuronModel(
            computation='v = exp(-dt/tau) * v + (1-exp(-dt/tau)) * (i_offset * (tau/c) + v_rest) + '
                        'i_exec * (exp(-dt/tau_e) - exp(-dt/tau)) * ((tau/c) * (tau_e/(tau_e-tau))) + '
                        'i_inh * (exp(-dt/tau_i) - exp(-dt/tau)) * ((tau/c) * (tau_i/(tau_i-tau))); '
                        'i_exec *= exp(-dt/tau_e); '
                        'i_inh *= exp(-dt/tau_i)',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='lif_curr_exp',
            compile_config='constant_folding'
        )

        p1 = Population(lif, num=10, name='P1', v=0.0, v_rest=0.0, v_reset=0.0,
                        c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                        i_offset=1.0, refract_time=0.001, dt=0.001)
        p2 = Population(lif, num=10, name='P1', v=0.0, v_rest=0.0, v_reset=0.0,
                        c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                        i_offset=1.0, refract_time=0.001, dt=0.001)

    def test_data(self):
        lif_curr_exp = NeuronModel(
            computation='v = Cm * v + v_tmp + i_exec * C_exec + i_inh * C_inh;'
                        'i_exec *= Cexec; i_inh *= Cinh',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='LIF_curr_exp'
        )

        p1 = Population(lif_curr_exp, num=10, name='P1', v=0.0, Cm=1.0, v_tmp=2.0,
                        C_exec=0.8, C_inh=0.6, Cexec=0.5, Cinh=0.3, v_threshold=20.0,
                        v_reset=0.0, refract_time=0.001)
        p2 = Population(lif_curr_exp, num=5, name='P1', v=1.0, Cm=2.0, v_tmp=3.0,
                        C_exec=0.7, C_inh=0.5, Cexec=0.6, Cinh=0.4, v_threshold=25.0,
                        v_reset=-10.0, refract_time=0.002)

        p = Population(model=p1.model, num=0, name="%s_compact" % p1.model.name, debug=False)
        p.merge(p1)
        p.merge(p2)

        gpu = p.to_gpu()
        cpu = p.from_gpu(gpu, 15, only_struct=False)
        np.testing.assert_array_almost_equal([0.0] * 10 + [1.0] * 5,
                                             list(cast(cpu.p_v, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([1.0] * 10 + [2.0] * 5,
                                             list(cast(cpu.p_Cm, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([2.0] * 10 + [3.0] * 5,
                                             list(cast(cpu.p_v_tmp, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.8] * 10 + [0.7] * 5,
                                             list(cast(cpu.p_C_exec, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.6] * 10 + [0.5] * 5,
                                             list(cast(cpu.p_C_inh, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.5] * 10 + [0.6] * 5,
                                             list(cast(cpu.p_Cexec, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.3] * 10 + [0.4] * 5,
                                             list(cast(cpu.p_Cinh, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([20.0] * 10 + [25.0] * 5,
                                             list(cast(cpu.p_v_threshold, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.0] * 10 + [-10.0] * 5,
                                             list(cast(cpu.p_v_reset, POINTER(c_float*15)).contents))

        self.assertListEqual([1] * 10 + [2] * 5, list(cast(cpu.p_refract_time, POINTER(c_int*15)).contents))
        self.assertListEqual([0] * 10 + [0] * 5, list(cast(cpu.p_refract_step, POINTER(c_int*15)).contents))


if __name__ == '__main__':
    unittest.main()

