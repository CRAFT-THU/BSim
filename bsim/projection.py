
from bsim.synapse_model import SynapseModel
from bsim.array import ModelOfArray


class Projection(ModelOfArray):
    def __init__(self, model: SynapseModel, num: int=2, name: str='', dst: int=0, **kwargs):
        super(Projection, self).__init__(model=model, num=num, name=name, dst=dst, **kwargs)


class Synapse(Projection):
    def __init__(self, model: SynapseModel, name: str='', dst: int=0, **kwargs):
        super(Synapse, self).__init__(model=model, num=1, name=name, dst=dst, **kwargs)
