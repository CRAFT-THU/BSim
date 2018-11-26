from typing import List, Dict

from bsim.neuron import Population
from bsim.synapse import Projection


class Network(object):
    def __init__(self, dt: float=0.0001, name: str=''):
        self.dt = dt
        self.name = name
        self.populations = {} # type: Dict[NeuronModel, List[Population]]
        self.projections = {} # type: Dict[SynapseModel, List[Projection]]
        self.data = {}
        self.connection = {}

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
                'pre_population': pre_population,
                'pre_neuron': pre_id,
                'post_neuron': post_id,
                'post_population': post_population,
                'synapse': synapse
                 })
        else:
            self.projections[synapse.model] = [{
                'pre_population': pre_population,
                'pre_neuron': pre_id,
                'post_neuron': post_id,
                'post_population': post_population,
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

    def compile_(self):

        for model in self.populations:
            model.compile()
        for model in self.projections:
            model.compile()

        # TODO compile the network datastructure
        for model in self.populations:
            count = 0
            data = self.populations[model][0].__class__(model=model, num=0, name="%s_compact" % model.name)
            for i in self.populations[model]:
                self._merge_parameters(data.parameters, i.parameters)
                self._compile_synapse_and_connection(i)
                count += len(i)

        return 1

    def __compile_synapse_and_connection(self, population: Population):
        return 1

    @staticmethod
    def _merge_parameters(para1: Dict[str, List], para2: Dict[str: List]):
        assert set(para1.keys()) == set(para2.keys()), 'Only neurons/synapse of the same type could be merged'
        for i in para1:
            para1[i].extend(para2[i])

        return para1

