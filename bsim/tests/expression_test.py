
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

        self.assertSetEqual(lif.parameters['variable'], set(('v', 'i_exec', 'i_inh')))
        self.assertSetEqual(lif.parameters['constant'], set(('v_tmp', 'C_exec', 'C', 'C_inh',
                                                           'Cexec', 'Cinh', 'v_reset', 'v_threshold')))
        self.assertSetEqual(lif.parameters['special'], set(('refract_step', 'refract_time')))
        self.assertSetEqual(lif.parameters['external'], set(('g_i_exec', 'g_i_inh')))
        self.assertSetEqual(lif.parameters['original'], set(('v', 'v_rest', 'v_reset', 'c', 'tau',
                                                             'tau_i', 'tau_e', 'v_threshold',
                                                             'i_offset', 'i_exec', 'i_inh', 'dt')))

        self.assertEqual(lif.expressions['assign']['computation']['v'],
                         'C * v + v_tmp + i_exec * C_exec + i_inh * C_inh')
        self.assertEqual(lif.expressions['assign']['computation']['i_exec'],
                         'i_exec * Cexec')
        self.assertEqual(lif.expressions['assign']['computation']['i_inh'],
                         'i_inh * Cinh')
        # self.assertEqual(lif.expressions['assign']['external']['i_exec'],
        #                  'i_exec + g_i_exec')
        # self.assertEqual(lif.expressions['assign']['external']['i_inh'],
        #                  'i_inh + g_i_inh')
        self.assertEqual(lif.expressions['fold']['C'], 'exp ( - dt / tau )')
        self.assertEqual(lif.expressions['fold']['Cexec'], 'exp ( - dt /  tau_e )')
        self.assertEqual(lif.expressions['fold']['Cinh'], 'exp ( - dt /  tau_i )')
        self.assertEqual(lif.expressions['fold']['v_tmp'],
                         '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) ')
        self.assertEqual(lif.expressions['fold']['C_exec'],
                         '( ( exp ( - dt / tau_e ) )  - ( exp ( - dt / tau ) ) ) '
                         '* ( ( tau / c ) * ( tau_e / ( tau_e - tau ) ) )')
        self.assertEqual(lif.expressions['fold']['C_inh'],
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

