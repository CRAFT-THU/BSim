
class NeuronModel(object):
    def __init__(self, name = 'LIF_Curr', 
            computation = '''v = exp(-dt/tau) * v + v_tmp + i_exc * rm * tau_exec / (tau_exc-tau) + i_inh * C_I
            i_exc = i_exc * exp(-dt/tau_exc)
            i_inh = i_inh * exp(-dt/tau_inh)
            ''',
            threshold = 'v > vt',
            reset = 'v = vr',
            refractory = True):
        """
        Parse model description, and find out variables and constants for futher optimization.
        This func may be modified.
        """
        self.name = name
        self.computation = computation.split(';\n')
        self.threshold = threshold
        self.reset = reset
        self.refractory = refractory
        self.elements = set()
        self.variable = set()
        self.constant = set()
        self.computation = {}

    def parser(self, description = 'i_exc * exp(-dt/tau_exec)'):
        parts = description.split('\n;')

        for line in parts:
            eq = line.count('=')
            assert eq <= 1, 'Only one assign per line'

            express = ''
            if (eq == 1):
                var, express = line.split('=')
                self.elements.add(var.strip())

            else:

            var = var.strip()







