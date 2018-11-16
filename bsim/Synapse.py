
from bsim.arrayobj import ArrayObj
from bsim.model import ModelArray

class Synapse(ModelArray):

    def __init__(self, model, num = 1, name = '', **kwargs):
        super(Synapse, self).__init__(model = model, num = num, name = name, **kwargs)
        self.type = model.name

Projection = Synapse
