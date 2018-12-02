
import unittest

import env

from bsim import SynapseModel
from bsim.projection import *


class TestProjectionMethods(unittest.TestCase):
    def test_data(self):
        STDPSynapse = SynapseModel(
            computation='apre*=exp(last_update-t)/tau_pre; apost*=exp(last_update-t)/tau_post',
            pre='apre += d_apre; weight=clip(weight+apost, gMin, gMax); last_update=t',
            post='apost+=d_apost; weight=clip(weight+apre, gMin, gMax); last_update=t',
            name='STDPSynapse'
        )

        p1 = Projection(STDPSynapse, num=10, name='S1', last_update=1, apre=1.0, tau_pre=2.0, apost=0.8, tau_post=0.6, d_apre=0.5, d_apost=0.3)
        p2 = Projection(STDPSynapse, num=5, name='S1', last_update=0, apre=2.0, tau_pre=3.0, apost=0.7, tau_post=0.5, d_apre=0.6, d_apost=0.4)

        p = Projection(model=p1.model, num=0, name="%s_compact" % p1.model.name, debug=False)
        p.merge(p1)
        p.merge(p2)

        gpu = p.to_gpu()
        cpu = p.from_gpu(gpu, only_struct=False)
        self.assertListEqual([1] * 10 + [0] * 5, list(cpu.last_update.contents))
        self.assertListEqual([1.0] * 10 + [2.0] * 5, list(cpu.apre.contents))
        self.assertListEqual([2.0] * 10 + [3.0] * 5, list(cpu.tau_apre.contents))
        self.assertListEqual([0.8] * 10 + [0.7] * 5, list(cpu.apost.contents))
        self.assertListEqual([0.6] * 10 + [0.5] * 5, list(cpu.tau_apost.contents))
        self.assertListEqual([0.5] * 10 + [0.6] * 5, list(cpu.d_apre.contents))
        self.assertListEqual([0.3] * 10 + [0.4] * 5, list(cpu.d_apost.contents))


if __name__ == '__main__':
    unittest.main()

