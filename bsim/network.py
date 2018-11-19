
class Network(object):
    def __init__(self, dt=0.0001, name = ''):
        self.dt = dt
        self.populations = {}
        self.projections = {}
        self.name = name

    def add_population(self, population, warn=True):
        """
        Add a population to the network
        :param population: Population
        :param warn: bool
        :return: >=0 success else failed
        """
        if population.type in self.populations:
            for p in self.populations[population.type]:
                if population.name == p.name:
                    if warn:
                        print('Population %s is already in the network %s', population.name, self.name)
                    return -1

        if population.type in self.populations:
            self.populations[population.type].append(population)
        else:
            self.populations[population.type] = [population]

        return 1

    def connect(self, pre_neuron, post_neuron, synapse):
        self.add_population(pre_neuron if len(pre_neuron) == 1 else pre_neuron['population'], warn=False)
        self.add_population(post_neuron if len(post_neuron) == 1 else post_neuron['population'], warn=False)

        if 'neuron2neuron' in self.projections:
            self.projections['neuron2neuron'].append({
                'pre_neuron': pre_neuron,
                'post_neuron': post_neuron,
                'synapse': synapse
                 })
        else:
            self.projections['neuron2neruon'] = [{
                'pre_neuron': pre_neuron,
                'post_neuron': post_neuron,
                'synapse': synapse
                 }]


        return 1

    def connect_alltoall(self, pre_population, post_population, prejction):
        return 1





        

            
