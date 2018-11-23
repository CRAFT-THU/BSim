
from bsim.model_array import ModelArray


class Population(ModelArray):

    def __init__(self, model, num=2, name='', **kwargs):
        super(Population, self).__init__(model=model, num=num, name=name, **kwargs)


class Neuron(Population):

    def __init__(self, model, name='', **kwargs):
        super(Neuron, self).__init__(model=model, num=1, name=name, **kwargs)


