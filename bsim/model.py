
import os
from ctypes import *
from abc import ABC, abstractmethod
from typing import Dict, Iterable

import importlib

from bsim.cudamemop import cudamemops
from bsim.data import Data
from bsim.generator import CUDAGenerator


class Model(ABC):
    def __init__(self, debug=False):
        self.dir = os.path.dirname(__file__)

    @abstractmethod
    def generate_h(self):
        pass

    @abstractmethod
    def generate_data_cu(self, debug=False):
        pass

    @abstractmethod
    def generate_compute_cu(self, debug=False):
        pass

    @abstractmethod
    def generate_py(self):
        pass


class ModelOfArray(Data):
    """
    An array of Models. Base class of neuron or synapse classes, we assume that the neuron or synapse belong to
    same group have same constant parameters.
    TODO: deal with the neurons in same population but different constant parameters
    """

    def __init__(self, model: Data, num: int=1, name: str= '', debug: bool=False, **kwargs: Dict):
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
        self.num = num  # type: int
        self.name = name  # type: str
        self.debug = debug  # type: bool
        # name of the model
        # self.type = model.name # type: str
        # description of the model
        self.model = model  # type: Union[NeuronModel, SynapseModel]
        # computations of the model
        # self.expressions = model.expressions['assignment'] # type: Dict
        # parameters that should stored as a List
        self.parameters = {}  # type: Dict[str:List]
        # parameters that stored as number and shared across the array
        self.shared = {}  # type: typing.Dict[str:Union[int, float]] # type: typing.Dict[str:

        if num > 0:
            if set(kwargs) < model.parameters['original']:
                raise ValueError('Projection %s Parameters not match, expect %s' % (name, model.parameters['origin']))

            for para in self.model.parameters['variable']:
                value = kwargs[para]
                if isinstance(value, Iterable):
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
                        assert not isinstance(value, Iterable), \
                            'currently we assume that the neuron in a population has same parameters'
                        express.replace(i, str(value))
                    self.shared[para] = express
                else:
                    value = kwargs[para]
                    # TODO: Currently, we assume that all neurons in the same population have same constant parameters
                    # TODO: support parameter shared
                    assert not isinstance(value, Iterable), \
                        'currently we assume that the neuron in a population has same parameters'
                    self.shared[para] = value

    def __len__(self):
        return self.num

    def __getitem__(self, index):
        ret = self.__class__(model=self.model, num=0, name='%s_%s' % (self.name, index))
        for i in ret.parameters:
            ret.parameters[i] = self.parameters[i] if i in ret.shared else self.parameters[i][index]
        return ret

    def merge(self, model):
        assert isinstance(model,  ModelOfArray) and self.model == model.model, 'Only array of same models could be merged'

        self.num += model.num
        for i in self.parameters:
            self.parameters[i].extend(model.parameters[i])

        return self

    def to_c(self):
        self._generate_py()
        self.c_type = getattr(importlib.import_module(
            'bsim.py_code.%s'.format(self.mode.name)
        ), self.model.name.capitalize())

        c = self.c_type()

        for i in self.parameters:
            t = getattr(c, i)._type_
            setattr(c, i, pointer((t*self.num)(*(self.parameters[i]))))
        return c


    def to_gpu(self):
        c_data = self.to_c()
        gpu_data = getattr(self.so(), "to_gpu_{}".format(self.model.name))(pointer(c_data))

        return gpu_data


    def from_gpu(self, gpu, only_struct=True):
        cpu = getattr(self.so(), "from_gpu_{}".format(self.model.name))(gpu)
        c = cast(cpu, POINTER(self.c_type * 1)).contents[0]

        if not only_struct:
            for i in self.parameters:
                t = getattr(c, i)._type_
                data = cudamemops.from_gpu_int(getattr(c, i), self.num) if t is c_int else cudamemops.from_gpu_float(getattr(c, i), self.num)
                setattr(c, i, data)

        return c

    def compile_(self):
        self._generate_h()
        self._generate_data_cu()
        self.model.generate_compute_cu()

        if CUDAGenerator.compile_(
                src='{}/c_code/{}.data.cu {}/c_code/{}.compute.cu'
                        .format(self.dir, self.model.name, self.model.name),
                output='{}/c_so/{}.so'.format(self.dir, self.model.name)
        ):
            self._so = cdll.LoadLibrary('{}/c_so/{}.so'.format(self.dir, self.model.name))
            getattr(self._so, "to_gpu_{}".format(self.model.name)).restype = POINTER(self.c_type)
            getattr(self._so, "from_gpu_{}".format(self.model.name)).restype = POINTER(self.c_type)
        else:
            self._so = None
            raise EnvironmentError('Compile file connection.data.so failed')

    def _generate_h(self):
        self.model.generate_h()

    def _generate_data_cu(self):
        self.model.generate_data_cu()

    def _generate_py(self):
        self.model.generate_py




