
import re
from collections import OrderedDict
from typing import Dict, Tuple

from bsim.utils import standardize

Operators = set(['+', '-', '*', '/', 'exp', 'log', '(', ')'])


def constant_folding_propagation(expressions, parameters):
    # TODO: constant folding and propagation

    expressions['fold']['computation']['C'] = 'exp ( - dt / tau )'
    expressions['fold']['computation']['Cexec'] = 'exp ( - dt /  tau_e )'
    expressions['fold']['computation']['Cinh'] = 'exp ( - dt /  tau_i )'
    expressions['fold']['computation']['v_tmp'] = \
        '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) '
    expressions['fold']['computation']['v_tmp'] = \
        '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) '
    expressions['fold']['computation']['v_tmp'] =  \
        '(1 - ( exp ( - dt / tau ) ) ) * ( i_offset * ( tau / c )  + v_rest ) '
    expressions['fold']['computation']['C_exec'] = \
        '( ( exp ( - dt / tau_e ) )  - ( exp ( - dt / tau ) ) ) * ( ( tau / c ) * ( tau_e / ( tau_e - tau ) ) )'
    expressions['fold']['computation']['C_inh'] = \
        '( ( exp ( - dt / tau_i ) )  - ( exp ( - dt / tau ) ) ) * ( ( tau / c ) * ( tau_i / ( tau_i - tau ) ) )'

    parameters['shared'] = set(('v_tmp', 'C_exec', 'C_inh', 'Cexec', 'Cinh', 'C'))

    return expressions, parameters


def compile_(formula: Dict[str, str] = {'i_exec': 'i_exc * exp(-dt/tau_exec)'}) -> Tuple[Dict, Dict]:
    """
    Optimize computation expressions, constant folding and propagation
    :param formula: dict, labeled equations as dict of strings, each string separated by '\n' or ';'
    :return: (expression:{}, parameters: {})
    !elements are the elements in the original expression
    """
    expressions = {'assign': {i: OrderedDict({}) for i in formula}, 'fold': OrderedDict({i: OrderedDict({}) for i in formula})}
    parameters = {'variable': set(), 'constant': set(), 'original': set()}
    formulas = {i: [] for i in formula}

    for i in formula:
        formulas[i] = re.split('[\n;]+', formula[i])

    for label in formulas:
        for line in formulas[label]:
            line = standardize(line)

            if line == '':
                break

            if line.count('=') == 1:
                var, expression = line.split('=')
                var = var.strip()
                expression = expression.strip()
                assert var not in expressions['assign'], 'One variable could have only one assignment'
                assert re.match('^[0-9a-zA-Z_]+$', var), 'variable name could have only [0-9a-zA-Z_]'
                expressions['assign'][label][var] = expression
                parameters['variable'].add(var)
                parameters['original'] |= set(re.findall('[0-9a-zA-Z_]+', var))
                parameters['original'] |= set(re.findall('[0-9a-zA-Z_]+', expression))
            else:
                raise ValueError(line + ' does not contain assignments, you should eliminate it!')

    parameters['original'] = parameters['original'] - Operators
    parameters['constant'] = parameters['original'] - parameters['variable']
    expressions, parameters = constant_folding_propagation(expressions, parameters)

    return expressions, parameters




