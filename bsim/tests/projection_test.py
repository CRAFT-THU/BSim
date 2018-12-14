
import unittest
import numpy as np
from ctypes import *

from bsim.projection import *


class TestProjectionMethods(unittest.TestCase):
    def test_data(self):
        stdp_synapse = SynapseModel(
            computation='apre*=exp(last_update-t)/tau_pre; apost*=exp(last_update-t)/tau_post',
            pre='apre += d_apre; last_update=t',
            post='apost+=d_apost; last_update=t',
            name='stdp_synapse'
        )

        p1 = Projection(stdp_synapse, num=10, name='S1', last_update=1, apre=1.0, tau_pre=2.0,
                        apost=0.8, tau_post=0.6, d_apre=0.5, d_apost=0.3, weight=0.1, delay=3)
        p2 = Projection(stdp_synapse, num=5, name='S1', last_update=0, apre=2.0, tau_pre=3.0,
                        apost=0.7, tau_post=0.5, d_apre=0.6, d_apost=0.4, weight=0.2, delay=4)

        p = Projection(model=p1.model, num=0, name="%s_compact" % p1.model.name, debug=False)
        p.merge(p1)
        p.merge(p2)

        gpu = p.to_gpu()
        cpu = p.from_gpu(gpu, 15, only_struct=False)
        np.testing.assert_array_almost_equal([1.0] * 10 + [2.0] * 5,
                                             list(cast(cpu.p_apre, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([2.0] * 10 + [3.0] * 5,
                                             list(cast(cpu.p_tau_pre, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.8] * 10 + [0.7] * 5,
                                             list(cast(cpu.p_apost, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.6] * 10 + [0.5] * 5,
                                             list(cast(cpu.p_tau_post, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.5] * 10 + [0.6] * 5,
                                             list(cast(cpu.p_d_apre, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.3] * 10 + [0.4] * 5,
                                             list(cast(cpu.p_d_apost, POINTER(c_float*15)).contents))
        np.testing.assert_array_almost_equal([0.1] * 10 + [0.2] * 5,
                                             list(cast(cpu.p_weight, POINTER(c_float*15)).contents))

        self.assertListEqual([3] * 10 + [4] * 5, list(cast(cpu.p_delay, POINTER(c_int*15)).contents))
        self.assertListEqual([1] * 10 + [0] * 5, list(cast(cpu.p_last_update, POINTER(c_int*15)).contents))
        self.assertListEqual([0] * 10 + [0] * 5, list(cast(cpu.p_dst, POINTER(c_int*15)).contents))


if __name__ == '__main__':
    unittest.main()

