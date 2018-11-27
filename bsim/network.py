from typing import List, Dict

from bsim.connection import Connection
from bsim.neuron import Population
from bsim.synapse import Projection


class Network(object):
    def __init__(self, dt: float=0.0001, name: str=''):
        self.dt = dt
        self.name = name

        # Original Data
        self.populations = {} # type: Dict[NeuronModel, List[Population]]
        self.projections = {} # type: Dict[SynapseModel, List[Dict]]

        # Compiled Data
        self.neuron_models = [] # type: List[NeuronModel]
        self.neuron_num = []  # type: List[Int]
        self.neuron_data = [] # type: List[Population]
        self.synapse_models = [] # type: SynapseModel
        self.synapse_num = []  # type: List[Int]
        self.synapse_data = [] # type: List[Projection]
        self.connection_data = Connection() # type: Connection

        # Temp Data
        self.population2start_id = {} # type: Dict[Population, Int]
        self.synapse2neuron_id = {}  # type: Dict[Projection, Int]
        self.neuron2synapses = {} # type: Dict[int, List[Projection]]

    def connect(self,
                pre_population: Population, pre_id: int,
                post_population: Population, post_id: int,
                synapse: Projection):
        self._add_population(pre_population, warn=False)
        self._add_population(post_population, warn=False)

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

        self._add_population(pre_population, warn=False)
        self._add_population(post_population, warn=False)

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

    def _add_population(self, population: Population, warn: bool=True):
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

    def _compile_model(self):
        for neuron in self.populations:
            neuron.compile()
            self.neuron_models.append(neuron)
            self.neuron_data.append(
                Population(model=neuron, num=0, name="%s_compact" % neuron.name)
            )
        self.neuron_num = [0] * (len(self.neuron_models) + 1)

        for synapse in self.projections:
            synapse.compile()
            self.synapse_models.append(neuron)
            self.synapse_data.append(
                Projection(model=synapse, num=0, name="%s_compact" % synapse.name)
            )
        self.synapse_num = [0] * (len(self.synapse_models) + 1)

    def _compile_neuron_data(self):
        count = 0
        for i in range(len(self.neuron_models)):
            data = self.neuron_data[i]
            for population in self.populations[i]:
                self.population2start_id[population] = count + len(data)
                data.merge(population)
            count += len(data)

        return 1

    def compile_(self):
        self._compile_model()

        self._compile_neuron_data()


        # TODO compile the network datastructure

        return 1

