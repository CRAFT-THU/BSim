
class Synapse(ArrayObj):
    def __init__(self, name = '', num = 1):
        self.name = name
        self.num = 1
        self.pre_neuron = None 
        self.post_neuron = None

    def __len__(self):
        return self.num
