
class Network(object):
    def __init__(self, name = ''):
        self.populations = []
        self.synapses = []
        self.projections = []
        self.name = name

    def add_population(self, population, warn=True):
        if population not in self.populations:
            self.populations.append(population);
        else:
            print('Population %s is already in the network %s', population.name, network.name)

        return 0;

    def connect(self, pre_neuron, post_neuron, synapse):
        add_population(pre_population, warn=False)
        add_population(post_population, warn=False)

        pre_neuron.post_synapses.append(synapse)
        post_neuron.pre_synapses.append(synapse)

        synapse.pre_neuron = pre_neuron
        synapse.post_neruon = post_neuron

        self.synapses.append(synapse)

    def connect_alltoall(self, pre_population, post_population, prejction):






        

            
