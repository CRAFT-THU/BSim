
from bsim.model import ModelArray


class Projection(ModelArray):

    def __init__(self, model, num=2, name='', **kwargs):
        super(Projection, self).__init__(model=model, num=num, name=name, **kwargs)


class Synapse(Projection):
    def __init__(self, model, name='', **kwargs):
        super(Synapse, self).__init__(model= model, num=1, name=name, **kwargs)
