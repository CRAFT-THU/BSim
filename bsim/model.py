
import typing

from bsim.compiler import compile

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
        :param model: Model
        :param num: ArrayNumber
        :param name: Name of the array of models
        :param kwargs: Model parameters
        """

        assert num >= 0 and isinstance(num, int), "%s, the number in %s is invalid" % (num, name)


        self.name = name
        self.type = model.name
        self.expressions = model.expressions['assignment']
        self.parameters = {i:[] for i in model.parameters['variable'] + model.parameters['constant']}
        self.shared = model.expressions['constant']
        self.idx_shared = []
        self.pre = [[] for _ in range(num)]
        self.post = [[] for _ in range(num)]

        if num > 0:
            # TODO: deal with data
            if set(kwargs) < model.parameters['origin']:
                raise ValueError('Projection %s Parameters not match, expect %s' % (name, model.parameters['origin'] ))

                for i in self.parameters:
                    if i in model.parameters['origin']:
                        value = kwargs[i]
                        if i in model.parameters['variable']:
                            if isinstance(value, typing.Iterable) and len(value) == self.num:
                                self.parameters[i] = value
                            else:
                                self.parameters[i] = [value ] * self.num









    def __len__(self):
        return self.num

    def __getitem__(self, item):
        t = self.__class__(model=self.model, name=self.name, num=0)







