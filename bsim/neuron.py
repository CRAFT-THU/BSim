
from bsim.model import ModelArray

class Neuron(ModelArray):

    def __init__(self, model, num = 1, name = '', **kwargs):
        super(Neuron, self).__init__(model = model, num = num, name = name, **kwargs)
        self.type = model.name
        self.threshold = model.threshold
        self.reset = model.reset
        self.refract_time = kwargs['refract_time'] if 'refract_time' in kwargs else 0



Population = Neuron

