
import importlib
from ctypes import *
from typing import Dict, Iterable, Union
from copy import deepcopy

from bsim import pkg_dir
from bsim.neuron_model import NeuronModel
from bsim.synapse_model import SynapseModel
from bsim.cudamemop import cudamemops
from bsim.data import Data
from bsim.generator import CUDAGenerator
from bsim.utils import to_list


class ModelOfArray(Data):
    """
    An array of Models. Base class of neuron or synapse classes, we assume that the neuron or synapse belong to
    same group have same constant parameters.
    TODO: deal with the neurons in same population but different constant parameters
    """

    def __init__(self, model: Union[NeuronModel, SynapseModel], num: int=1, name: str= '',
                 debug: bool=False, **kwargs: Dict):
        """
        Parameters
        ----------
        model: description of neuron model or synapse model
        num: number of objects in the ModelArray
        name: identifier for the ModelArray
        kwargs: init parameters for the ModelArray
        """
        super(ModelOfArray, self).__init__()

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
        self.parameter = {}  # type: Dict[str:List]
        # parameters that stored as number and shared across the array
        self.shared = {}  # type: typing.Dict[str:Union[int, float]]
        self.special = {}

        if num > 0:
            if not set(kwargs) - model.parameters['special'] >= model.parameters['original']:
                raise ValueError('{} Parameters not match, expect {}, {} is missing'
                                 .format(name, model.parameters['original'],
                                         model.parameters['original'] -
                                         (set(kwargs) - model.parameters['special'])
                                         )
                                 )

            for para in self.model.parameters['variable']:
                value = kwargs[para]
                if isinstance(value, Iterable):
                    assert len(list(value)) == self.num, \
                        'input parameters should have the same length as the population size'
                    self.parameter[para] = list(value)
                else:
                    self.parameter[para] = [value] * self.num

            for para in self.model.parameters['constant']:
                if para in model.expressions['fold']:
                    express = self.model.expressions['fold'][para]
                    # TODO: Currently, we assume that all neurons in the same population have same constant parameters
                    for i in (self.model.parameters['origin'] - self.model.parameters['variable']):
                        value = kwargs[i]
                        assert not isinstance(value, Iterable), \
                            'currently we assume that the neuron in a population has same parameters'
                        express.replace(i, str(value))
                    self.shared[para] = [express] * num
                else:
                    value = kwargs[para]
                    # TODO: Currently, we assume that all neurons in the same population have same constant parameters
                    assert not isinstance(value, Iterable), \
                        'currently we assume that the neuron in a population has same parameters'
                    # TODO: support parameter shared
                    self.shared[para] = [value] * num

            for para in self.model.parameters['special']:
                value = kwargs[para]
                if isinstance(value, Iterable):
                    assert len(list(value)) == self.num, \
                        'input parameters should have the same length as the population size'
                    self.special[para] = list(value)
                else:
                    # TODO: support parameter shared
                    self.special[para] = [value] * num

    def __len__(self):
        return self.num

    def __getitem__(self, index):
        ret = self.__class__(model=self.model, num=0, name='{}_{}'.format(self.name, index))

        for i in self.special:
            ret.special[i] = self.special[i][index]

        for i in self.shared:
            # TODO: support parameter shared
            ret.shared[i] = self.shared[i][index]

        for i in self.parameter:
            ret.parameter[i] = self.parameter[i][index]

        ret.num = index.length if isinstance(index, slice) else 1

        return ret

    def merge(self, model):
        assert isinstance(model,  ModelOfArray) and self.model == model.model, 'Only array of same models could be merged'

        if self.num == 0:
            for i in model.special:
                self.special[i] = to_list(model.special[i])

            for i in model.shared:
                # TODO: support parameter shared
                self.shared[i] = to_list(model.shared[i])

            for i in model.parameter:
                self.parameter[i] = to_list(model.parameter[i])
        else:
            for i in self.special:
                self.special[i].extend(to_list(model.special[i]))

            for i in self.shared:
                # TODO: support parameter shared
                self.shared[i].extend(to_list(model.shared[i]))

            for i in self.parameter:
                self.parameter[i].extend(to_list(model.parameter[i]))
        self.num += model.num

        return self

    def to_c(self):
        self._generate_py()
        self.c_type = getattr(importlib.import_module(
            'bsim.code_gen.{}'.format(self.model.name.lower())
        ), self.model.name.capitalize())

        c = self.c_type()

        for i in self.parameter:
            setattr(c, "p_{}".format(i), cast(pointer((c_float*self.num)(*(self.parameter[i]))), POINTER(c_float)))

        for i in self.shared:
            # TODO: support parameter shared
            setattr(c, "p_{}".format(i), cast(pointer((c_float*self.num)(*(self.shared[i]))), POINTER(c_float)))

        for i in self.special:
            setattr(c, 'p_{}'.format(i), cast(pointer((c_int*self.num)(*(self.special[i]))), POINTER(c_int)))

        return c

    def to_gpu(self):
        c_data = self.to_c()
        gpu_data = getattr(self.so(), "to_gpu_{}".format(self.model.name.lower()))(pointer(c_data), c_int(self.num))

        return gpu_data

    def from_gpu(self, gpu, num: int=0, only_struct=True):
        cpu = getattr(self.so(), "from_gpu_{}".format(self.model.name.lower()))(gpu, num)
        c = cast(cpu, POINTER(self.c_type * 1)).contents[0]

        if not only_struct:
            for i in self.special:
                data = cudamemops.from_gpu_int(getattr(c, 'p_{}'.format(i)), num)
                setattr(c, 'p_{}'.format(i), data)

            for i in self.parameter:
                data = cudamemops.from_gpu_float(getattr(c, "p_{}".format(i)), num)
                setattr(c, "p_{}".format(i), data)

            for i in self.shared:
                data = cudamemops.from_gpu_float(getattr(c, "p_{}".format(i)), num)
                setattr(c, "p_{}".format(i), data)

        return c

    def compile_(self):
        self._generate_h()
        self._generate_data_cu()
        self.model.generate_compute_cu()

        if CUDAGenerator.compile_(
                # TODO: compute.cu
                src='{}/code_gen/{}.data.cu'
                        .format(pkg_dir, self.model.name.lower()),
                # src='{}/code_gen/{}.data.cu {}/code_gen/{}.compute.cu'
                #        .format(pkg_dir, self.model.name.lower(), pkg_dir, self.model.name.lower()),
                output='{}/so_gen/{}.so'.format(pkg_dir, self.model.name.lower())
        ):
            self._so = cdll.LoadLibrary('{}/so_gen/{}.so'.format(pkg_dir, self.model.name.lower()))
            getattr(self._so, "to_gpu_{}".format(self.model.name.lower())).restype = POINTER(self.c_type)
            getattr(self._so, "from_gpu_{}".format(self.model.name.lower())).restype = POINTER(self.c_type)
        else:
            self._so = None
            raise EnvironmentError('Compile file {}/so_gen/{}.so failed'.format(pkg_dir, self.model.name.lower()))

    def _generate_h(self):
        self.model.generate_h()

    def _generate_data_cu(self):
        self.model.generate_data_cu()

    def _generate_py(self):
        self.model.generate_py()

