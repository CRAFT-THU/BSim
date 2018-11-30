
import inspect
import unittest
from ctypes import *

import env
import bsim.utils as utils
from bsim.cudamemop import CUDAMemOp


class TestExpressionMethods(unittest.TestCase):
    def test_standard(self):
        print("\nTesting: %s\n" % inspect.currentframe().f_code.co_name)
        self.assertEqual(utils.standardize('a= b+c_ -aaa'), 'a = b + c_ - aaa')
        self.assertEqual(utils.standardize('a += b+c_-aaa'), 'a = a + ( b + c_ - aaa )')
        self.assertEqual(utils.standardize('a_bsfdfd ++   \n'), 'a_bsfdfd = a_bsfdfd + 1')
        self.assertEqual(utils.standardize('a_bsfdfd --   \n'), 'a_bsfdfd = a_bsfdfd - 1')

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

