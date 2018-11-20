
from bsim.neuron import Population
from bsim.synapse import Projection


class Network(object):
    def __init__(self, dt: float=0.0001, name: str=''):
        self.dt = dt
        self.populations = {}
        self.projections = {}
        self.name = name

    def add_population(self, population: Population, warn: bool=True):
        """
        Add a population to the network
        :param population: Population
        :param warn: bool
        :return: >=0 success else failed
        """
        if (population.model in self.populations) and (population in self.populations[population.model]):
            if warn:
                print('Population %s is already in the network %s', population.name, self.name)
            return -1

        if population.type in self.populations:
            self.populations[population.type].append(population)
        else:
            self.populations[population.type] = [population]

        return 0

    def connect(self,
                pre_population: Population, pre_id: int,
                post_population: Population, post_id: int,
                synapse: Projection):
        self.add_population(pre_population, warn=False)
        self.add_population(post_population, warn=False)

        if synapse.model in self.projections:
            self.projections[synapse.model].append({
                'pre_neuron': (pre_population, pre_id),
                'post_neuron': (post_population, post_id),
                'synapse': synapse
                 })
        else:
            self.projections[synapse.model] = [{
                'pre_neuron': (pre_population, pre_id),
                'post_neuron': (post_population, post_id),
                'synapse': synapse
                 }]

        return 1

    def connect_alltoall(self,
                         pre_population: Population,
                         post_population: Population,
                         projection: Projection):

        self.add_population(pre_population, warn=False)
        self.add_population(post_population, warn=False)

        if projection.model in self.projections:
            self.projections[projection.model].append({
                'pre_population': pre_population,
                'post_population': post_population,
                'projection': projection
            })
        else:
            self.projections[projection.model] = [{
                'pre_population': pre_population,
                'post_population': post_population,
                'projection': projection
            }]

        return 1

