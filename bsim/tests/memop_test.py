
import unittest
from ctypes import *

import env
from bsim.cudamemop import CUDAMemOp


class TestMemOpMethods(unittest.TestCase):
    def test_cudamemop(self):
        cudamemop = CUDAMemOp(['int']).so()
        data = (c_int * 100)(*list(range(100)))
        cpu = (c_int * 100)(*([0]*100))
        gpu = cudamemop.gpu_malloc_int(1000)
        cudamemop.cpu2gpu_int(data, gpu, 100)
        cudamemop.gpu2cpu_int(gpu, cpu, 100)

        self.assertListEqual(list(data), list(cpu))
        self.assertListEqual(list(cpu), list(range(100)))


if __name__ == '__main__':
    unittest.main()

