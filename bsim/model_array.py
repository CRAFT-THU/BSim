
import typing
from typing import Dict, Union, List

from bsim.neuron_model import NeuronModel
from bsim.synapse_model import SynapseModel


class ModelArray(object):
    """
    An array of Models. Base class of neuron or synapse classes, we assume that the neuron or synapse belong to
    same group have same constant parameters.
    TODO: deal with the neurons in same population but different constant parameters
    """

    def __init__(self, model:typing.Union[NeuronModel, SynapseModel], num:int=1, name:str= '', **kwargs:typing.Dict):
        """
        Parameters
        ----------
        model: description of neuron model or synapse model
        num: number of objects in the ModelArray
        name: identifier for the ModelArray
        kwargs: init parameters for the ModelArray
        """
        assert num >= 0 and isinstance(num, int), "%s, the number in %s is invalid" % (num, name)

        # name of this array
        self.name = name # type: str
        # name of the model
        self.type = model.name # type: str
        # description of the model
        self.model = model # type: Union[NeuronModel, SynapseModel]
        ## computations of the model
        #self.expressions = model.expressions['assignment'] # type: Dict
        # parameters that should stored as a List
        self.parameters = {} #  type: Dict[str:List]
        # parameters that stored as number and shared across the array
        self.shared = {} # type: typing.Dict[str:Union[int, float]] # type: typing.Dict[str:

        if num > 0:
            if set(kwargs) < model.parameters['origin']:
                raise ValueError('Projection %s Parameters not match, expect %s' % (name, model.parameters['origin']))

            for para in self.model.parameters['variable']:
                value = kwargs[para]
                if isinstance(value, typing.Iterable):
                    assert len(list(value)) == self.num, \
                        'input parameters should have the same length as the population size'
                    self.parameters[para] = list(value)
                else:
                    self.parameters[para] = [value] * self.num

            for para in self.model.parameters['constant']:
                if para in model.expressions['fold']:
                    express = self.model.expressions['fold'][para]
                    # TODO: Currently, we assume that all neurons in the same population have same constant parameters
                    for i in (self.model.parameters['origin'] - self.model.parameters['variable']):
                        value = kwargs[i]
                        assert not isinstance(value, typing.Iterable), \
                            'currently we assume that the neuron in a population has same parameters'
                        express.replace(i, str(value))
                    self.shared[para] = express
                else:
                    value = kwargs[para]
                    # TODO: Currently, we assume that all neurons in the same population have same constant parameters
                    assert not isinstance(value, typing.Iterable), \
                        'currently we assume that the neuron in a population has same parameters'
                    self.shared[para] = value

    def __len__(self):
        return self.num

    def __getitem__(self, index):
        ret = self.__class__(model=self.model, num=0, name='%s_%s' % (self.name, index))
        for i in ret.parameters:
            ret.parameters[i] = self.parameters[i] if i in ret.shared else self.parameters[i][index]

        return ret

        # parameters = {}
        # for i in self.parameters:
        #     if isinstance(self.parameters[i], typing.Iterable):
        #         parameters[i] = self.parameters[i][item]
        #     else:
        #         parameters[i] = self.parameters[i]
        # return parameters
