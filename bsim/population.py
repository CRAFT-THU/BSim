from bsim.neuron_model import NeuronModel
from bsim.array import ModelOfArray


class Population(ModelOfArray):

    def __init__(self, model: NeuronModel, num: int = 2, name: str = '', refract_step: int = 0, **kwargs):
        super(Population, self).__init__(model=model, num=num, name=name,
                                         refract_step=refract_step, i_exec=0, i_inh=0, **kwargs)


class Neuron(Population):

    def __init__(self, model: NeuronModel, name: str = '', refract_step: int = 0, **kwargs):
        super(Neuron, self).__init__(model=model, num=1, name=name, refract_step=refract_step, **kwargs)


