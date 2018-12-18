
import unittest

import bsim.utils as utils
from bsim.neuron_model import NeuronModel


class TestExpressionMethods(unittest.TestCase):
    def test_standard(self):
        self.assertEqual(utils.standardize('a= b+c_ -aaa'), 'a = b + c_ - aaa')
        self.assertEqual(utils.standardize('a += b'), 'a = a + b')
        self.assertEqual(utils.standardize('a += b+c_-aaa'), 'a = a + ( b + c_ - aaa )')
        self.assertEqual(utils.standardize('a_bsfdfd ++   \n'), 'a_bsfdfd = a_bsfdfd + 1')
        self.assertEqual(utils.standardize('a_bsfdfd --   \n'), 'a_bsfdfd = a_bsfdfd - 1')

    def test_model(self):
        lif = NeuronModel(
            computation='v = C * v + v_tmp + i_exec * C_exec + i_inh * C_inh;i_exec *= Cexec; '
                        'i_inh *= Cinh',
            threshold='V > v_threshold',
            reset='v = v_reset',
            name='lif_curr_exp'
        )

        self.assertSetEqual(lif.parameters['variable'], set(('v', 'i_exec', 'i_inh')))
        self.assertSetEqual(lif.parameters['shared'], set(('v_tmp', 'C_exec', 'C', 'C_inh', 'Cexec', 'Cinh')))
        self.assertSetEqual(lif.parameters['special'], set(('refract_step', 'refract_time')))
        self.assertSetEqual(lif.parameters['external'], set(('g_i_exec', 'g_i_inh')))

        self.assertEqual(lif.expressions['assign']['computation']['v'],
                         'C * v + v_tmp + i_exec * C_exec + i_inh * C_inh')
        self.assertEqual(lif.expressions['assign']['computation']['i_exec'],
                         'i_exec * Cexec')
        self.assertEqual(lif.expressions['assign']['computation']['i_inh'],
                         'i_inh * Cinh')
        self.assertEqual(lif.expressions['assign']['current']['i_exec'],
                         'i_exec + g_i_exec')
        self.assertEqual(lif.expressions['assign']['current']['i_inh'],
                         'i_inh + g_i_inh')
        self.assertEqual(lif.expressions['fold']['computation']['C'], 'exp ( - dt / tau )')
        self.assertEqual(lif.expressions['fold']['computation']['Cexec'], 'exp ( - dt /  tau_e )')
        self.assertEqual(lif.expressions['fold']['computation']['Cinh'], 'exp ( - dt /  tau_i )')
        self.assertEqual(lif.expressions['fold']['computation']['v_tmp'],
                         '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) ')
        self.assertEqual(lif.expressions['fold']['computation']['v_tmp'],
                         '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) ')
        self.assertEqual(lif.expressions['fold']['computation']['v_tmp'],
                         '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) ')
        self.assertEqual(lif.expressions['fold']['computation']['C_exec'],
                         '( ( exp ( - dt / tau_e ) )  - ( exp ( - dt / tau ) ) ) '
                         '* ( ( tau / c ) * ( tau_e / ( tau_e - tau ) ) )')
        self.assertEqual(lif.expressions['fold']['computation']['C_inh'],
                         '( ( exp ( - dt / tau_i ) )  - ( exp ( - dt / tau ) ) ) '
                         '* ( ( tau / c ) * ( tau_i / ( tau_i - tau ) ) )')


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

