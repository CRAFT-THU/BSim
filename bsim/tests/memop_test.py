
import inspect
import unittest
from ctypes import *

import env
from bsim.cudamemop import cudamemops


class TestMemOpMethods(unittest.TestCase):
    def test_cudamemop(self):
        print("\nTesting: %s\n" % inspect.currentframe().f_code.co_name)
        data = (c_int * 1000)(*list(range(1000)))
        cpu = (c_int * 1000)(*([0]*1000))
        gpu = cudamemops.gpu_malloc_int(1000)
        gpu1 = cudamemops.to_gpu_int(data, 1000)
        cpu1 = cast(cudamemops.from_gpu_int(gpu1, 1000), POINTER(c_int*1000))
        cudamemops.cpu2gpu_int(data, gpu, 1000)
        cudamemops.gpu2cpu_int(gpu, cpu, 1000)


        self.assertListEqual(list(data), list(cpu))
        self.assertListEqual(list(data), list(cpu1.contents))

if __name__ == '__main__':
    unittest.main()

