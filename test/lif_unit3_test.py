import unittest
import numpy as np
from ctypes import *

from bsim import *
from bsim.env import c_real


class TestLIFUnit3(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(TestLIFUnit3, self).__init__(*args, **kwargs)

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

        s01 = net.projection(static, num=self.n*self.n, name='S01', weight=1e-5, delay=0.0002)
        s12 = net.projection(static, num=self.n*self.n, name='S12', weight=2e-5, delay=0.0001)

        net.all_to_all(p0, p1, s01)
        net.all_to_all(p1, p2, s12)

        self.net = net

        self.net.build()
        self.net.to_gpu()

    def test_run(self):
        self.net.run(0.1)


if __name__ == '__main__':
    print('Testing {}: '.format(__file__[:-3]))
    unittest.main()
    print('\n')
