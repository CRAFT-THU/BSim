
import typing
from typing import Dict, Union, List

from bsim.model_compiler import compile_

class BaseModel(object):
    def __init__(self):
        self.name =''
        self.parameters = {}

    def compile_(self):
        c_file = open(self.name + ".cpp")
        cu_file = open(self.name + ".cpp")
        py_file = open(self.name + ".py")

    def _compile_h(self):
        h_file = open(self.name + ".h", "w+")
        h_file.write("#ifndef " + self.name.upper() + "_H\n")
        h_file.write("#define " + self.name.upper() + "_H\n")
        h_file.write("\n\n")

        h_file.write("struct" + self.name + " {\n")
        for i in self.parameters['variable']:
            h_file.write("\tfloat *p_" + i + ";")
        h_file.write("};")
        h_file.write('\n')

        h_file.write('extern "C" {')
        h_file.write("void update_" + self.name.lower() + "(" + self.name + " *neuron_data, int num, int start_id);")
        h_file.write("}\n")

        h_file.write('extern "C" {')
        h_file.write("void to_gpu_" + self.name.lower() + "(" + self.name + " *neuron_data, int num, int start_id);")
        h_file.write("}\n")

        h_file.write("\n")
        h_file.write("__global void update_" + self.name.lower() + "_gpu(" + self.name + " *neuron_data, int num, int start_id);")
        h_file.write("\n")

        h_file.write("#endif /* " + self.name.upper() + "_H */\n")

        return

    def _compile_c(self):
        return

    def _compile_cu(self):
        return

    def _compile_py(self):
        return


class NeuronModel(object):
    def __init__(self, computation: str='', threshold: str='v > vt', reset: str='v = vr', refract: bool=True, name: str=''):
        """
        Create NeuronModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.threshold = threshold
        self.reset = reset
        self.refract = refract

        self.expressions, self.parameters = compile_({
            'computation': computation,
        })


class SynapseModel(object):
    def __init__(self, computation: str='', pre: str='', post: str='', name: str=''):
        """
        Create SynapseModel: find out variables and constants for further optimization.
        This func may be modified.
        """
        self.name = name
        self.learn = not (pre == '' and post == '')
        self.expressions, self.parameters = compile_({
            'computation': computation,
            'pre': pre,
            'post': post
        })
        self.parameters['variable'].add('weight')
        self.parameters['constant'].add('delay')


class ModelArray(object):
    """
    An array of Models. Base class of neuron or synapse classes, we assume that the neuron or synapse belong to
    same group have same constant parameters.
    TODO: deal with the neurons in same population but different constant parameters
    """

    def __init__(self, model:typing.Union[NeuronModel, SynapseModel], num:int=1, name:str='', **kwargs:typing.Dict):
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

LIF_curr_exp = NeuronModel(
    computation='v = Cm * v + v_tmp + i_exc * C_exc + i_inh * C_inh;i_exec *= Cexc; i_inh *= Cinh',
    threshold='V > v_threshold',
    reset='v = v_reset',
    name='LIF_curr_exp')

STDPSynapse = SynapseModel(
    computation='apre*=exp(last_update-t)/tau_pre; apost*=exp(last_update-t)/tau_post',
    pre='apre += d_apre; weight=clip(weight+apost, gMin, gMax); last_update=t',
    post='apost+=d_apost; weight=clip(weight+apre, gMin, gMax); last_update=t',
    name='STDPSynapse'
)


