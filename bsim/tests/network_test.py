
import unittest
import numpy as np

from bsim.network import *


class TestNetworkMethods(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(TestNetworkMethods, self).__init__(*args, **kwargs)

        lif_curr_exp = NeuronModel(
            computation='v = Cm * v + v_tmp + i_exec * C_exec + i_inh * C_inh;i_exec *= Cexec; '
                        'i_inh *= Cinh; i_exec += g_i_exec; i_inh += g_i_inh',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='LIF_curr_exp'
        )

        stdp_synapse = SynapseModel(
            computation='apre*=exp(last_update-t)/tau_pre; apost*=exp(last_update-t)/tau_post',
            pre='apre += d_apre; last_update=t',
            post='apost+=d_apost; last_update=t',
            name='stdp_synapse'
        )

        p0 = Population(lif_curr_exp, num=2, name='P0', v=0.0, Cm=1.0, v_tmp=2.0, C_exec=2.0, C_inh=0.6, Cexec=0.5,
                        Cinh=0.3, v_threshold=1.0, v_reset=0.0, refract_time=1)
        p1 = Population(lif_curr_exp, num=3, name='P1', v=1.0, Cm=2.0, v_tmp=3.0, C_exec=3.0, C_inh=1.0, Cexec=1.6,
                        Cinh=0.4, v_threshold=20.0, v_reset=-10.0, refract_time=2)
        p2 = Population(lif_curr_exp, num=2, name='P2', v=0.0, Cm=3.0, v_tmp=4.0, C_exec=2.0, C_inh=0.5, Cexec=0.5,
                        Cinh=0.3, v_threshold=20.0, v_reset=-30.0, refract_time=5)
        p3 = Population(lif_curr_exp, num=1, name='P3', v=1.0, Cm=4.0, v_tmp=1.0, C_exec=1.0, C_inh=1.5, Cexec=1.6,
                        Cinh=0.4, v_threshold=1.0, v_reset=-20.0, refract_time=3)

        s01_00 = Projection(stdp_synapse, num=1, name='S0', last_update=0, apre=0.1, tau_pre=1.2,
                            apost=2.3, tau_post=3.4, d_apre=4.5, d_apost=5.6, weight=6.7, delay=0)
        s01_01 = Projection(stdp_synapse, num=1, name='S0', last_update=0, apre=0.2, tau_pre=1.3,
                            apost=2.4, tau_post=3.5, d_apre=4.6, d_apost=5.7, weight=6.8, delay=0)
        s01_02 = Projection(stdp_synapse, num=1, name='S0', last_update=0, apre=0.3, tau_pre=1.4,
                            apost=2.5, tau_post=3.6, d_apre=4.7, d_apost=5.8, weight=6.9, delay=0)
        s01_10 = Projection(stdp_synapse, num=1, name='S0', last_update=0, apre=0.4, tau_pre=1.5,
                            apost=2.6, tau_post=3.7, d_apre=4.8, d_apost=5.9, weight=7.0, delay=0)
        s01_11 = Projection(stdp_synapse, num=1, name='S0', last_update=0, apre=0.5, tau_pre=1.6,
                            apost=2.7, tau_post=3.8, d_apre=4.9, d_apost=6.0, weight=7.1, delay=0)
        s01_12 = Projection(stdp_synapse, num=1, name='S0', last_update=0, apre=0.6, tau_pre=1.7,
                            apost=2.8, tau_post=3.9, d_apre=5.0, d_apost=6.1, weight=7.2, delay=0)
        s02 = Projection(stdp_synapse, num=4, name='S1', last_update=0, apre=0.7, tau_pre=1.8,
                        apost=2.9, tau_post=4.0, d_apre=5.1, d_apost=6.2, weight=7.3, delay=2)
        s12 = Projection(stdp_synapse, num=3, name='S2', last_update=0, apre=0.8, tau_pre=1.9,
                        apost=3.0, tau_post=4.1, d_apre=5.2, d_apost=6.3, weight=7.4, delay=1)
        s23 = Projection(stdp_synapse, num=2, name='S3', last_update=0, apre=0.9, tau_pre=2.0,
                        apost=3.1, tau_post=4.2, d_apre=5.3, d_apost=6.4, weight=7.5, delay=2)

        self.net = Network(dt=0.0001)

        self.net.one_to_one(p0, 0, p1, 0, s01_00)
        self.net.one_to_one(p0, 0, p1, 1, s01_01)
        self.net.one_to_one(p0, 0, p1, 2, s01_02)
        self.net.one_to_one(p0, 1, p1, 0, s01_10)
        self.net.one_to_one(p0, 1, p1, 1, s01_11)
        self.net.one_to_one(p0, 1, p1, 2, s01_12)
        self.net.all_to_all(p0, p2, s02)
        self.net.all_to_all(p1, p3, s12)
        self.net.all_to_all(p2, p3, s23)

        self.net.build()
        self.net.to_gpu()

    def test_connection(self):
        self.assertEqual(1, len(self.net.connection_data))
        cpu = self.net.connection_data[0].from_gpu(self.net.connection_data_gpu[0], only_struct=False)
        self.assertListEqual([0, 3, 0, 0, 0, 0, 0, 0,
                              0, 0, 6, 7, 8, 0, 0, 0,
                              9, 11, 0, 0, 0, 13, 14, 0],
                             list(cast(cpu.delay_start, POINTER(c_int*cpu.n_len)).contents))
        self.assertListEqual([3, 3, 0, 0, 0, 0, 0, 0,
                              0, 0, 1, 1, 1, 0, 0, 0,
                              2, 2, 0, 0, 0, 1, 1, 0],
                             list(cast(cpu.delay_num, POINTER(c_int*cpu.n_len)).contents))
        self.assertListEqual([0, 0, 0, 2, 4, 6, 8, 10],
                             list(cast(cpu.rev_delay_start, POINTER(c_int*cpu.r_n_len)).contents))
        self.assertListEqual([0, 0, 2, 2, 2, 2, 2, 5],
                             list(cast(cpu.rev_delay_num, POINTER(c_int*cpu.r_n_len)).contents))
        self.assertListEqual([0, 3, 1, 4, 2, 5, 9, 11, 10, 12, 6, 7, 8, 13, 14],
                             list(cast(cpu.rev_map2sid, POINTER(c_int*cpu.s_len)).contents))

    def test_neuron_data(self):
        self.assertEqual(1, len(self.net.neuron_data))
        cpu = self.net.neuron_data[0].from_gpu(self.net.neuron_data_gpu[0],
                                               self.net.neuron_nums[1]-self.net.neuron_nums[0],
                                               only_struct=False)

        np.testing.assert_array_almost_equal([0.0] * 2 + [1.0] * 3 + [0.0] * 2 + [1.0] * 1,
                                             list(cast(cpu.p_v, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([1.0] * 2 + [2.0] * 3 + [3.0] * 2 + [4.0] * 1,
                                             list(cast(cpu.p_Cm, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([2.0] * 2 + [3.0] * 3 + [4.0] * 2 + [1.0] * 1,
                                             list(cast(cpu.p_v_tmp, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([2.0] * 2 + [3.0] * 3 + [2.0] * 2 + [1.0] * 1,
                                             list(cast(cpu.p_C_exec, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([0.6] * 2 + [1.0] * 3 + [0.5] * 2 + [1.5] * 1,
                                             list(cast(cpu.p_C_inh, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([0.5] * 2 + [1.6] * 3 + [0.5] * 2 + [1.6] * 1,
                                             list(cast(cpu.p_Cexec, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([0.3] * 2 + [0.4] * 3 + [0.3] * 2 + [0.4] * 1,
                                             list(cast(cpu.p_Cinh, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([1.0] * 2 + [20.0] * 3 + [20.0] * 2 + [1.0] * 1,
                                             list(cast(cpu.p_v_threshold, POINTER(c_float*8)).contents))
        np.testing.assert_array_almost_equal([0.0] * 2 + [-10.0] * 3 + [-30.0] * 2 + [-20.0] * 1,
                                             list(cast(cpu.p_v_reset, POINTER(c_float*8)).contents))

        self.assertListEqual([1] * 2 + [2] * 3 + [5] * 2 + [3] * 1, list(cast(cpu.p_refract_time, POINTER(c_int*8)).contents))
        self.assertListEqual([0] * 2 + [0] * 3 + [0] * 2 + [0] * 1, list(cast(cpu.p_refract_step, POINTER(c_int*8)).contents))

    def test_synapse_data(self):
        self.assertEqual(1, len(self.net.synapse_data))
        cpu = self.net.synapse_data[0].from_gpu(self.net.synapse_data_gpu[0],
                                               self.net.synapse_nums[1]-self.net.synapse_nums[0],
                                               only_struct=False)

        start = np.array([0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.8, 0.8,
                          0.8, 0.7, 0.7, 0.7, 0.7, 0.9, 0.9])
        np.testing.assert_array_almost_equal(list(start),
                                             list(cast(cpu.p_apre, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal(list(start+1.1),
                                             list(cast(cpu.p_tau_pre, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal(list(start+2.2),
                                             list(cast(cpu.p_apost, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal(list(start+3.3),
                                             list(cast(cpu.p_tau_post, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal(list(start+4.4),
                                             list(cast(cpu.p_d_apre, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal(list(start+5.5),
                                             list(cast(cpu.p_d_apost, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal(list(start+6.6),
                                             list(cast(cpu.p_weight, POINTER(c_float*15)).contents))

        self.assertListEqual([0, 0, 0, 0, 0, 0,
                              1, 1, 1, 2, 2, 2,
                              2 ,2, 2], list(cast(cpu.p_delay, POINTER(c_int*15)).contents))
        self.assertListEqual([0]*15, list(cast(cpu.p_last_update, POINTER(c_int*15)).contents))
        self.assertListEqual([2, 3, 4, 2, 3, 4,
                              7, 7, 7, 5, 6, 5,
                              6, 7, 7], list(cast(cpu.p_dst, POINTER(c_int*15)).contents))


if __name__ == '__main__':
    unittest.main()
