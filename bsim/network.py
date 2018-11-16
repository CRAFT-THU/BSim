
class Network(object):
    def __init__(self, dt=0.0001, name = ''):
        self.dt = dt
        self.populations = {}
        self.projections = {}
        self.neuron_types = set()
        self.synapse_types = set()
        self.name = name

    def add_population(self, population, warn=True):
        """
        Add a population to the network
        :param population:
        :param warn:
        :return: >=0 idx of population in self.populations else failed
        """
        for p in self.populations:
            if population.name == p.name:
                if warn:
                    print('Population %s is already in the network %s', population.name, self.name)
                return -1

        self.populations.append(population);
        self.neuron_types.add(population.type)

        return len(self.populations) - 1;

    def connect(self, pre_neuron, post_neuron, synapse):
        self.add_population(pre_neuron, warn=False)
        self.add_population(post_neuron, warn=False)

        pre_neuron.post_synapses.append(synapse)
        post_neuron.pre_synapses.append(synapse)

        synapse.pre_neuron = pre_neuron
        synapse.post_neruon = post_neuron

        self.synapses.append(synapse)
        return 1

    def connect_alltoall(self, pre_population, post_population, prejction):
        return 1





        

            
