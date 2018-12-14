import unittest

from bsim import *


class TestSmallNet(unittest.TestCase):
    def test_run(self):
        lif_curr_exp = Neuron(
            computation='v = Cm * v + v_tmp + i_exec * C_exec + i_inh * C_inh;i_exec *= Cexec; '
                        'i_inh *= Cinh; i_exec += g_i_exec; i_inh += g_i_inh',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='lif_curr_exp'
        )

        static = Synapse(
            name='static'
        )

        N = 500

        net = Net(dt=0.0001)

        p0 = net.population(lif_curr_exp, num=N, name='P0', v=0.0, Cm=1.0, v_tmp=2.0, C_exec=2.0, C_inh=0.6, Cexec=0.5,
                            Cinh=0.3, v_threshold=1.0, v_reset=0.0, refract_time=1)
        p1 = net.population(lif_curr_exp, num=N, name='P1', v=1.0, Cm=2.0, v_tmp=3.0, C_exec=3.0, C_inh=1.0, Cexec=1.6,
                            Cinh=0.4, v_threshold=20.0, v_reset=-10.0, refract_time=2)
        p2 = net.population(lif_curr_exp, num=N, name='P2', v=0.0, Cm=3.0, v_tmp=4.0, C_exec=2.0, C_inh=0.5, Cexec=0.5,
                            Cinh=0.3, v_threshold=20.0, v_reset=-30.0, refract_time=5)
        p3 = net.population(lif_curr_exp, num=N, name='P3', v=1.0, Cm=4.0, v_tmp=1.0, C_exec=1.0, C_inh=1.5, Cexec=1.6,
                            Cinh=0.4, v_threshold=1.0, v_reset=-20.0, refract_time=3)
        p4 = net.population(lif_curr_exp, num=N, name='P4', v=1.0, Cm=4.0, v_tmp=1.0, C_exec=1.0, C_inh=1.5, Cexec=1.6,
                            Cinh=0.4, v_threshold=1.0, v_reset=-20.0, refract_time=3)
        p5 = net.population(lif_curr_exp, num=N, name='P5', v=1.0, Cm=4.0, v_tmp=1.0, C_exec=1.0, C_inh=1.5, Cexec=1.6,
                            Cinh=0.4, v_threshold=1.0, v_reset=-20.0, refract_time=3)

        s01 = net.projection(static, num=N*N, name='S01', weight=6.7, delay=0)
        s12 = net.projection(static, num=N*N, name='S12', weight=7.3, delay=2)
        s23 = net.projection(static, num=N*N, name='S23', weight=7.4, delay=1)
        s34 = net.projection(static, num=N*N, name='S34', weight=7.5, delay=2)
        s45 = net.projection(static, num=N*N, name='S45', weight=7.5, delay=2)

        net.all_to_all(p0, p1, s01)
        net.all_to_all(p1, p2, s12)
        net.all_to_all(p2, p3, s23)
        net.all_to_all(p3, p4, s34)
        net.all_to_all(p4, p5, s45)

        net.build()
        net.to_gpu()


if __name__ == '__main__':
    unittest.main()

