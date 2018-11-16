
from bsim.compiler import compile
from bsim.arrayobj import ArrayObj

class NeuronModel(object):
    def __init__(self, computation = '', threshold = 'v > vt', reset = 'v = vr', name = ''):
        """
        Create NeuronModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.expression = {'computation': computation, 'reset': reset}
        self.threshold = threshold
        self.reset = reset

        self.expression, self.parameter = compile({
            'computation': computation,
        })

class SynapseModel(object):
    def __init__(self, computation = '', pre = '', post = '', name = ''):
        """
        Create SynapseModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.expression = {'computation': computation, 'pre': pre, 'post': post}
        self.learn = not (pre=='' and post=='')
        self.expression, self.parameter = compile({
            'computation': computation,
            'pre': pre,
            'post': post
        })


class ModelArray(object):
    """
    Base class of neuron or synapse classes, we assume that the neuron or synapse belong to the same group have same constant parameters.
    TODO: deal with the neurons in same population but different constant parameters
    """

    def __init__(self, model, num = 1, name = '', **kwargs):
        """

        :param model: NeuronModel
        :param num:
        :param name:
        :param kwargs:
        """
        super(ModelArray, self).__init__(name, num)

        assert num >= 1, "%s, the number in %s is invalid" % (name, str(num))

        if set(kwargs) < model.parameters['origin']:
            raise ValueError('Projection %s Parameters not match, expect %s' % (name, model.parameters['origin'] ))

        self.expression = model.expressions['assignment']

        self.parameter_origin = model.parameters['origin']
        self.parameter_folded = model.expresions['fold']
        self.parameter_variable = model.parameters['variable']
        self.parameter_constant = model.parameters['constant']

        # TODO: deal with fold operation
        self.pre = [[] for _ in range(num)]
        self.post = [[] for _ in range(num)]








