
import unittest

import env

from bsim import NeuronModel
from bsim.population import *


class TestPopulationMethods(unittest.TestCase):
    def test_data(self):
        LIF_curr_exp = NeuronModel(
            computation='v = Cm * v + v_tmp + i_exec * C_exec + i_inh * C_inh;i_exec *= Cexec; i_inh *= Cinh',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='LIF_curr_exp'
        )

        p1 = Population(LIF_curr_exp, num=10, name='P1', v=0.0, Cm=1.0, v_tmp=2.0, C_exec=0.8, C_inh=0.6, Cexec=0.5, Cinh=0.3, v_threshold=20.0, v_reset=0.0, refract_time=1)
        p2 = Population(LIF_curr_exp, num=5, name='P1', v=1.0, Cm=2.0, v_tmp=3.0, C_exec=0.7, C_inh=0.5, Cexec=0.6, Cinh=0.4, v_threshold=25.0, v_reset=-10.0, refract_time=2)

        p = Population(model=p1.model, num=0, name="%s_compact" % p1.model.name, debug=False)
        p.merge(p1)
        p.merge(p2)

        gpu = p.to_gpu()
        cpu = p.from_gpu(gpu, only_struct=False)
        self.assertListEqual([0.0] * 10 + [1.0] * 5, list(cpu.v.contents))
        self.assertListEqual([1.0] * 10 + [2.0] * 5, list(cpu.Cm.contents))
        self.assertListEqual([2.0] * 10 + [3.0] * 5, list(cpu.v_tmp.contents))
        self.assertListEqual([0.8] * 10 + [0.7] * 5, list(cpu.C_exec.contents))
        self.assertListEqual([0.6] * 10 + [0.5] * 5, list(cpu.C_inh.contents))
        self.assertListEqual([0.5] * 10 + [0.6] * 5, list(cpu.Cexec.contents))
        self.assertListEqual([0.3] * 10 + [0.4] * 5, list(cpu.Cinh.contents))
        self.assertListEqual([20.0] * 10 + [25.0] * 5, list(cpu.v_threshold.contents))
        self.assertListEqual([0.0] * 10 + [-10.0] * 5, list(cpu.v_reset.contents))
        self.assertListEqual([1] * 10 + [2] * 5, list(cpu.refractime.contents))


if __name__ == '__main__':
    unittest.main()

