import unittest
import numpy as np
from ctypes import *

from bsim import *
from bsim.env import c_real


class TestLIFUnit1(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(TestLIFUnit1, self).__init__(*args, **kwargs)

        self.n = 10

        lif = Neuron(
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

        static = Synapse(
            name='static'
        )

        net = Net(dt=0.0001)
        p0 = net.population(lif, num=self.n, name='P0', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=10.0, refract_time=0.0000)
        p1 = net.population(lif, num=self.n, name='P1', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.0000)
        p2 = net.population(lif, num=self.n, name='P2', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.0000)

        s01 = net.projection(static, num=self.n*self.n, name='S01', weight=1e-5, delay=0.0001)
        s12 = net.projection(static, num=self.n*self.n, name='S12', weight=2e-5, delay=0.0001)

        net.all_to_all(p0, p1, s01)
        net.all_to_all(p1, p2, s12)

        self.net = net

        self.net.build()
        self.net.to_gpu()

    def test_neuron_data(self):
        self.assertEqual(1, len(self.net.neuron_data))

        n = self.net.neuron_nums[1] - self.net.neuron_nums[0]
        self.assertEqual(n, 30)

        cpu = self.net.neuron_data[0].from_gpu(self.net.neuron_data_gpu[0],
                                               self.net.neuron_nums[1]-self.net.neuron_nums[0],
                                               only_struct=False)

        np.testing.assert_array_almost_equal([0.0] * 30,
                                             list(cast(cpu.p_v, POINTER(c_real*n)).contents))
        np.testing.assert_array_almost_equal([0.0] * 30,
                                             list(cast(cpu.p_i_exec, POINTER(c_real*n)).contents))
        np.testing.assert_array_almost_equal([0.0] * 30,
                                             list(cast(cpu.p_i_inh, POINTER(c_real*n)).contents))

        np.testing.assert_array_almost_equal([0.0149999997] * 30,
                                             list(cast(cpu.p_v_threshold, POINTER(c_real*n)).contents))
        np.testing.assert_array_almost_equal([0.0] * 30,
                                             list(cast(cpu.p_v_reset, POINTER(c_real*n)).contents))

        self.assertListEqual([0] * 30, list(cast(cpu.p_refract_time, POINTER(c_int*n)).contents))
        self.assertListEqual([0] * 30, list(cast(cpu.p_refract_step, POINTER(c_int*n)).contents))

        np.testing.assert_array_almost_equal([0.999899983] * 30,
                                             list(cast(cpu.p_Cexec, POINTER(c_real*n)).contents))
        np.testing.assert_array_almost_equal([0.999899983] * 30,
                                             list(cast(cpu.p_Cinh, POINTER(c_real*n)).contents))

        np.testing.assert_array_almost_equal([0.000998942531] * 30,
                                             list(cast(cpu.p_C_exec, POINTER(c_real*n)).contents))
        np.testing.assert_array_almost_equal([0.000998942531] * 30,
                                             list(cast(cpu.p_C_inh, POINTER(c_real*n)).contents))

        np.testing.assert_array_almost_equal([0.00999003649] * 10 + [0.0] * 20,
                                             list(cast(cpu.p_v_tmp, POINTER(c_real*n)).contents))

        np.testing.assert_array_almost_equal([0.998001993] * 30,
                                             list(cast(cpu.p_C, POINTER(c_real*n)).contents))

    def test_synapse_data(self):
        self.assertEqual(1, len(self.net.synapse_data))
        n = self.net.synapse_nums[1] - self.net.synapse_nums[0]
        self.assertEqual(n, 200)
        cpu = self.net.synapse_data[0].from_gpu(self.net.synapse_data_gpu[0],
                                                self.net.synapse_nums[1]-self.net.synapse_nums[0],
                                                only_struct=False)

        np.testing.assert_array_almost_equal([9.99999975e-06] * 100 + [1.99999995e-05] * 100,
                                             list(cast(cpu.p_weight, POINTER(c_real*n)).contents))

        self.assertListEqual([i for i in range(10, 20)] * 10 + [i for i in range(20, 30)] * 10,
                             list(cast(cpu.p_dst, POINTER(c_int*n)).contents))

    def test_connection(self):
        self.assertEqual(1, len(self.net.connection_data))
        cpu = self.net.connection_data[0].from_gpu(self.net.connection_data_gpu[0], only_struct=False)
        self.assertListEqual([i for i in range(0, 200, 10)] + [0] * 10,
                             list(cast(cpu.delay_start, POINTER(c_int*cpu.n_len)).contents))
        self.assertListEqual([10] * 20 + [0] * 10,
                             list(cast(cpu.delay_num, POINTER(c_int*cpu.n_len)).contents))

    def test_run(self):
        self.net.run(0.1)


if __name__ == '__main__':
    unittest.main()

