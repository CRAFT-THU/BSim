
import re

from bsim.utils import standardize

Operators = set('+', '-', '*', '/', 'exp', 'log', '(', ')')

def constant_folding_propagation(expressions, parameters):
    # TODO: constant folding and propagation

    return expressions, parameters





def compile(formular={'i_exec':'i_exc * exp(-dt/tau_exec)'}):
    """
    Optimize computation expressions, constant folding and propagation
    :param formular: dict, labeled equations as dict of strings, each string separated by '\n' or ';'
    :return: (expression:{}, parameters: {})
    !elements are the elements in the original expression
    """
    expressions = {'assign': { {} for i in formular}, 'fold': {} }
    parameters = {'variable': set(), 'constant': set(), 'original': set()}

    for i in formular:
        formular[i] = re.split('\n;', formular[i])

    for label in formular:
        for line in formular[label]:
            line = standardize(line)

            if line.count('=') == 1:
                var, expression = line.split('=')
                var = var.strip()
                assert var not in expressions['assign'], 'One variable could have only one assignment'
                assert re.match('^[0-9a-zA-Z_]$', var), 'variable name could have only [0-9a-zA-Z_]'
                expressions['assign'][label][var] = expression
                parameters['variables'].add(var)
            else:
                raise ValueError(line + ' does not contain assignments, you should eliminate it!')

    parameters['original'] = parameters['original'] - Operators
    parameters['constant'] = parameters['original'] - parameters['variable']
    expressions, parameters = constant_folding_propagation(expressions, parameters)

    return expressions, parameters




