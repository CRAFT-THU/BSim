
import typing

from bsim.model_compiler import compile

class NeuronModel(object):
    def __init__(self, computation = '', threshold = 'v > vt', reset = 'v = vr', name = ''):
        """
        Create NeuronModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
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
        self.learn = not (pre=='' and post=='')
        self.expression, self.parameter = compile({
            'computation': computation,
            'pre': pre,
            'post': post
        })


class ModelArray(object):
    """
    An array of Models. Base class of neuron or synapse classes, we assume that the neuron or synapse belong to
    same group have same constant parameters.
    TODO: deal with the neurons in same population but different constant parameters
    """

    def __init__(self, model, num = 1, name = '', **kwargs):
        """
        Initialize model,
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

            for para in self.parameters:
                    if para in self.model.parameters['variable']:
                        value = kwargs[para]
                        if isinstance(value, typing.Iterable):
                            assert len(list(value)) == self.num, 'input parameters should have the same length as the population size'
                            self.parameters[para] = list(value)
                        else:
                            self.parameters[para] = [value ] * self.num
                    elif para in self.model.parameters['constant']:
                        if para in model.expressions['fold']:
                            express = self.model.expressions['fold'][para]
                            self.parameters[para] = [express] * self.num
                            for i in (self.model.parameters['origin'] & self.model.parameters['constant']):
                                for e, j in enumerate(self.parameters[para]):
                                    value = kwargs[i]
                                    assert not isinstance(value, typing.Iterable), 'currently we assume that the neuron in a population has same parameters'
                                    if isinstance(value, typing.Iterable):
                                        assert len(list(value)) == self.num, 'input parameters should have the same length as the population size'
                                        j.replace(i, str(value[e]))
                                    else:
                                        j.replace(i, str(value))
                        else:
                            value = kwargs[para]
                            if isinstance(value, typing.Iterable):
                                assert len(list(value)) == self.num, 'input parameters should have the same length as the population size'
                                self.parameters[para] = list(value)
                            else:
                                self.parameters[para] = [value ] * self.num

    def __len__(self):
        return self.num

    def __getitem__(self, item):

        parameters = {}
        for i in self.parameters:
            if isinstance(self.parameters[i], typing.Iterable):
                parameters[i] = self.parameters[i][item]
            else:
                parameters[i] = self.parameters[i]

        return parameters





