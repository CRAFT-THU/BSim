
from bsim.model import ModelArray


class Population(ModelArray):

    def __init__(self, model, num=2, name='', **kwargs):
        super(Population, self).__init__(model=model, num=num, name=name, **kwargs)
        self.threshold = model.threshold
        self.reset = model.reset
        self.refract_time = kwargs['refract_time'] if 'refract_time' in kwargs else 0


class Neuron(Population):

    def __init__(self, model, name='', **kwargs):
        super(Neuron, self).__init__(model=model, num=1, name=name, **kwargs)


