import unittest

from bsim import *


class TestLIF(unittest.TestCase):
    def test_run(self):

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

        N = 500

        net = Net(dt=0.0001)
        p0 = net.population(lif, num=N, name='P0', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=1.0, refract_time=0.001, dt=0.001)
        p1 = net.population(lif, num=N, name='P1', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.001, dt=0.001)
        p2 = net.population(lif, num=N, name='P2', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.001, dt=0.001)
        p3 = net.population(lif, num=N, name='P3', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.001, dt=0.001)
        p4 = net.population(lif, num=N, name='P4', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.001, dt=0.001)
        p5 = net.population(lif, num=N, name='P5', v=0.0, v_rest=0.0, v_reset=0.0,
                            c=0.1, tau=50e-3, tau_i=1.0, tau_e=1.0, v_threshold=15e-3,
                            i_offset=0.0, refract_time=0.001, dt=0.001)

        s01 = net.projection(static, num=N*N, name='S01', weight=0.002, delay=0.001)
        s12 = net.projection(static, num=N*N, name='S12', weight=0.002, delay=0.001)
        s23 = net.projection(static, num=N*N, name='S23', weight=0.002, delay=0.001)
        s34 = net.projection(static, num=N*N, name='S34', weight=0.002, delay=0.001)
        s45 = net.projection(static, num=N*N, name='S45', weight=0.002, delay=0.001)

        net.all_to_all(p0, p1, s01)
        net.all_to_all(p1, p2, s12)
        net.all_to_all(p2, p3, s23)
        net.all_to_all(p3, p4, s34)
        net.all_to_all(p4, p5, s45)

        net.build()
        net.to_gpu()

        net.run(0.1)


if __name__ == '__main__':
    unittest.main()

