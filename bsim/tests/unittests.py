
import unittest
from ctypes import c_int

import bsim.utils as utils
from bsim.cudamemop import CUDAMemOp


class TestExpressionMethods(unittest.TestCase):
    def test_standard(self):
        self.assertEqual(utils.standardize('a= b+c_ -aaa'), 'a = b + c_ - aaa')
        self.assertEqual(utils.standardize('a += b+c_-aaa'), 'a = a + ( b + c_ - aaa )')
        self.assertEqual(utils.standardize('a_bsfdfd ++   \n'), 'a_bsfdfd = a_bsfdfd + 1')
        self.assertEqual(utils.standardize('a_bsfdfd --   \n'), 'a_bsfdfd = a_bsfdfd - 1')

    def test_cudamemcpy(self):
        cudamemop = CUDAMemOp(['int']).so()
        data = (c_int * 1000)(*list(range(1000)))
        cpu = (c_int * 1000)(*([0]*1000))
        gpu = cudamemop.gpu_malloc_int(1000)
        cudamemop.cpu2gpu_int(data, gpu, 1000)
        cudamemop.gpu_cpu_int(gpu, cpu, 1000)

        self.assertListEqual(data, cpu)



    #def test_model(self):
    #    self.assertEqual(LIF_curr_exp.expressions['computation'])

    # def test_compiler(self):
    #     assignments = {'test': {'d': 'a + exp ( a + c * b ) - ( a + c ) * b', 'b':'a + 1'}}
    #     expressions = {'test': 'b + a'}
    #     variables = ['b', 'd']
    #     elements = ['a', 'b', 'c', 'd']
    #     folded = parser.constant_folding_propagation(assignments, expressions, variables, elements)
    #     self.assertEqual(folded['test'], 'Temp1 = a + c')
    #     self.assertEqual(assignments['test']['d'], 'a + exp ( a + c * b ) - ( Temp1 ) * b')
    #     self.constants = ['Temp', ]


if __name__ == '__main__':
    unittest.main()

